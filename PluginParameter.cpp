#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "PluginParameter.h"

void log(PluginParameter *p)
{
    FILE *f = fopen("C:\\Users\\K�ytt�j�\\Documents\\Visual Studio 2013\\Projects\\TransitionSynth\\Release\\log.txt", "a");
    char n[100], ns[8];
    p->getName(n, 100);
    p->getShortName(ns);
    fprintf(f, "Parameter [id=%d]: name=%s (shortname=%s) = %f\n", p->getId(), n, ns, p->getValue());
    fclose(f);
}

PluginParameter::PluginParameter(const std::string &name, const std::string &shortName, int id,
                                 CallbackUpdatable *cu, float defaultValue)
    : name(name), shortName(shortName), callbackUpdatable(cu), id(id), value(defaultValue), initialValue(defaultValue)
{
    if (callbackUpdatable)
        callbackUpdatable->onUpdateWithValue(value);
}

PluginParameter::~PluginParameter()
{
}

void PluginParameter::setValue(float value)
{
    if (fabs(value - this->value) > 1e-6)
    {
        this->value = value;
        if (callbackUpdatable != nullptr)
        {
            this->callbackUpdatable->onUpdateWithValue(value);
        }
    }
}

void PluginParameter::setInitialValue()
{
    setValue(initialValue);
}

float PluginParameter::getValue()
{
    return value;
}

int PluginParameter::getId()
{
    return id;
}

void PluginParameter::getName(char *name, int maxLen)
{
    strncpy(name, this->name.c_str(), maxLen);
    name[maxLen - 1] = 0;
}

void PluginParameter::getShortName(char *name)
{
    strcpy(name, shortName.c_str());
}

constexpr int serialize_magicSize = sizeof(int);
constexpr int serialize_payloadSizeInfoSize = sizeof(int);
constexpr int serialize_idSize = sizeof(int);
constexpr int serialize_valueSize = sizeof(float);
constexpr int serialize_payloadSize = serialize_idSize + serialize_valueSize;
constexpr int serialize_totalSize = serialize_magicSize + serialize_payloadSizeInfoSize + serialize_payloadSize;

/*
 * The serialized format is:
 * content			size		description
 * --- header ---
 * 0xDADA			int			magic number
 * payload size		int			size of serialized data excluding the magic number and payload size
 * --- payload ---
 * id				int			the id of the parameter
 * value			float		the value of the parameter
 */
int PluginParameter::serialize(char *writeBuffer)
{
    const int magic = 0xDADA;
    const int payloadSize = serialize_payloadSize;
    char *write = writeBuffer;
    memcpy(write, &magic, serialize_magicSize);
    write += serialize_magicSize;
    memcpy(write, &payloadSize, serialize_payloadSizeInfoSize);
    write += serialize_payloadSizeInfoSize;
    memcpy(write, &id, serialize_idSize);
    write += serialize_idSize;
    memcpy(write, &value, serialize_valueSize);
    return serialize_totalSize;
}

std::string PluginParameter::serializeToString()
{
    char buf[serialize_totalSize];
    serialize(buf);
    return std::string(buf, serialize_totalSize);
}

int deserializeParameter(const char *readBuffer, int *id, float *value)
{
    const int magic = 0xDADA;
    const char *read = readBuffer;
    int mgc = 0;
    memcpy(&mgc, read, serialize_magicSize);
    if (mgc == magic)
    {
        read += serialize_magicSize;
        int payloadSize = 0;
        memcpy(&payloadSize, read, serialize_payloadSizeInfoSize);
        if (payloadSize >= serialize_payloadSize)
        {
            read += serialize_payloadSizeInfoSize;
            memcpy(id, read, serialize_idSize);
            read += serialize_idSize;
            memcpy(value, read, serialize_valueSize);
            return serialize_totalSize;
        }
    }
    return 0;
}