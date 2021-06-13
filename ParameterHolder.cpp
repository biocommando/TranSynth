#include "ParameterHolder.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

char workDir[1024] = {0};
void resolveWorkDir()
{
    if (workDir[0])
        return;
    // work out the resource directory
    // first we get the DLL path from windows API
    extern void *hInstance;
    GetModuleFileName((HMODULE)hInstance, workDir, 1024);
    // let's get rid of the DLL file name
    for (int i = strlen(workDir) - 1; i >= 0; i--)
        if (workDir[i] == '\\')
        {
            workDir[i] = 0;
            break;
        }
}

ParameterHolder::ParameterHolder()
{
    resolveWorkDir();
}

ParameterHolder::~ParameterHolder()
{
}

void ParameterHolder::readProgram(int number, char *name)
{
    std::string presetFileName = std::string(workDir) + "\\" + "TranSynPresets.dat";
    FILE *f = fopen(presetFileName.c_str(), "r");
    char buf[256];
    bool pgFound = false;
    name[0] = 0;
    while (!feof(f))
    {
        fgets(buf, sizeof(buf), f);
        char cmd = 0;
        int id = 0;
        float value = 0;
        sscanf(buf, "%c %d %f", &cmd, &id, &value);
        if (cmd == '{' && id == number)
        {
            pgFound = true;
        }
        if (!pgFound)
            continue;
        if (cmd == '+')
        {
            auto p = getParameterById(id);
            if (p)
                p->setValue(value);
        }
        if (cmd == '$')
        {
            strncpy(name, &buf[2], 24);
            name[23] = 0;
        }

        if (cmd == '}')
            break;
    }
    fclose(f);
}

void ParameterHolder::addParameter(PluginParameter param)
{
    params.push_back(param);
}

PluginParameter *ParameterHolder::getParameterById(int id)
{
    for (int i = params.size() - 1; i >= 0; i--)
    {
        if (params[i].getId() == id)
        {
            return &params[i];
        }
    }
    return nullptr;
}

PluginParameter *ParameterHolder::getParameterByIndex(int index)
{
    if (index >= 0 && index < params.size())
    {
        return &params[index];
    }
    return nullptr;
}

int ParameterHolder::serialize(char **writeBuffer, const char *header, int headerSize)
{
    auto s = serializeToString();
    char *buf = (char *)malloc(s.size() + headerSize);
    memcpy(buf, header, headerSize);
    memcpy(buf + headerSize, s.c_str(), s.size());
    *writeBuffer = buf;
    return s.size() + headerSize;
}

extern void logf(const char *, float);
std::string ParameterHolder::serializeToString()
{
    char tempBuf[100];
    std::string s;
    for (int i = 0; i < params.size(); i++)
    {
        PluginParameter *p = &params[i];
        s.append(p->serializeToString());
    }
    s.append("End");
    return s;
}
void ParameterHolder::deserialize(const char *readBuffer)
{
    // Set initial value to parameters that were not read from the buffer
    for (int i = 0; i < params.size(); i++)
    {
        params[i].setInitialValue();
    }
    int idx = 0;
    int readBytes;
    do
    {
        int id;
        float value;
        readBytes = deserializeParameter(&readBuffer[idx], &id, &value);
        if (readBytes > 0)
        {
            PluginParameter *p = getParameterById(id);
            if (p != nullptr)
            {
                p->setValue(value);
            }
        }
        idx += readBytes;
    } while (readBytes);
}