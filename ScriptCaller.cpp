#include "ScriptCaller.h"
#include "Log.h"
#include "ExternalPluginExecutor.h"
#include "util.h"
#include <stdio.h>
#include <fstream>

extern std::string workDir;
extern void resolveWorkDir();

ScriptCaller::ScriptCaller()
{
    resolveWorkDir();
    const auto path = workDir + "\\plugins\\plugin_list.txt";
    std::ifstream ifs;
    ifs.open(path);
    for (std::string s; std::getline(ifs, s);)
    {
        const auto pluginDef = Util::splitString(s, ';');
        if (pluginDef.size() >= 2)
        {
            pluginList.push_back(pluginDef);
        }
    }
    // script = std::make_unique<SimpleScript>(path);
}

void ScriptCaller::execute(const std::string &strParam, AudioEffectX *eff,
                           std::map<std::string, double> *extraParams)
{
    LOG_DEBUG("execute", "Start script: %s", strParam.c_str());
    std::string pluginName;
    // This is a special mechanism that makes the executor to use
    // the parameter itself (starting from position 1) as the plugin definition.
    // This makes it possible to save plugin parameter data in presets
    if (strParam[0] != '\1')
        pluginName = strParam;
    std::map<std::string, double> variables;
    char paramName[32];
    int idx = 0;
    while (true)
    {
        paramName[0] = 0;
        eff->getParameterName(idx, paramName);
        if (paramName[0])
            variables[paramName] = eff->getParameter(idx);
        else
            break;
        idx++;
    }
    if (extraParams)
    {
        for (const auto &e : *extraParams)
        {
            variables[e.first] = e.second;
        }
    }
    variables["tempo"] = eff->getTimeInfo(kVstTempoValid)->tempo;
    variables["samplerate"] = eff->getTimeInfo(kVstTempoValid)->sampleRate;
    variables["samplepos"] = eff->getTimeInfo(kVstTempoValid)->samplePos;

    std::string pluginExecutable;
    for (const auto &pluginDef : pluginList)
    {
        if (pluginDef[0] == pluginName || strParam[0] == '\1')
        {
            const auto def = strParam[0] != '\1' ? pluginDef : Util::splitString(strParam.substr(1), ';');
            if (def.size() < 2)
                break;
            pluginExecutable = def[1];
            for (int i = 2; i < def.size(); i++)
            {
                const auto kv = Util::splitString(def[i], '=');
                if (kv.size() == 2)
                {
                    if (kv[1][0] == 'a')
                        variables[kv[0]] = static_cast<int>(kv[1][1]);
                    else
                        variables[kv[0]] = std::stod(kv[1]);
                }
            }
            break;
        }
    }

    if (pluginExecutable == "")
        return;

    const auto fullPathToExecutable = workDir + "\\plugins\\" + pluginExecutable;

    ExternalPluginExecutor extPlugExecutor(fullPathToExecutable);

    for (const auto &item : variables)
    {
        extPlugExecutor.addParameter({item.first, item.second});
    }
    LOG_DEBUG("execute", "Executing plugin %s", fullPathToExecutable.c_str());
    if (!extPlugExecutor.execute())
        return;
    
    wtDataGenerated = false;

    for (const auto &item : extPlugExecutor.getOutput())
    {
        variables[item.getName()] = item.getValue();
        if (item.getName() == "wt_generated" && item.getValue() > 0.1)
        {
            LOG_DEBUG("execute", "wt_generated found",1);
            wtDataGenerated = true;
        }
    }

    /*script->execute(
        variables, [](const auto &s) {}, pluginName);*/

    // As idx is at the end, we just need to decrease it until zero and we
    // can be sure that the parameters exist
    for (idx = idx - 1; idx >= 0; idx--)
    {
        eff->getParameterName(idx, paramName);
        eff->setParameter(idx, variables[paramName]);
    }
}

std::vector<std::string> ScriptCaller::getPluginNames()
{
    std::vector<std::string> names;
    for (const auto &pluginDef : pluginList)
        names.push_back(pluginDef[0]);
    return names;
}

std::unique_ptr<float[]> ScriptCaller::getGeneratedWtData()
{
    const auto fileName = workDir + "\\plugins\\wavedata.bin";
    FILE *f = fopen(fileName.c_str(), "rb");
    auto ptr = std::make_unique<float[]>(10000);
    fread(ptr.get(), sizeof(float), 10000, f);
    fclose(f);
    return ptr;
}