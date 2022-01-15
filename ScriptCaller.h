#pragma once
#include "audioeffectx.h"
#include <map>
#include <memory>
#include <string>
#include <vector>

class ScriptCaller
{
    std::vector<std::vector<std::string>> pluginList;
    bool wtDataGenerated = false;

public:
    ScriptCaller();

    void execute(const std::string &, AudioEffectX *eff, std::map<std::string, double> *extraParams);

    std::vector<std::string> getPluginNames();

    bool getWtDataGenerated() { return wtDataGenerated; }

    std::unique_ptr<float[]> getGeneratedWtData();

    std::string getPluginInvocationData(const std::string &name)
    {
        for (const auto &data : pluginList)
        {
            if (data[0] == name)
            {
                std::string ss;
                for(const auto &s : data)
                {
                    if (ss != "")
                        ss += ';';
                    ss += s;
                }
                return ss;
            }
        }
        return "";
    }
};