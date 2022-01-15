#pragma once
#include "audioeffectx.h"
#include <map>
#include <string>
#include <vector>

class ScriptCaller
{
    std::vector<std::vector<std::string>> pluginList;
public:
    ScriptCaller();

    void execute(const std::string &, AudioEffectX *eff, std::map<std::string, double> *extraParams);

    std::vector<std::string> getPluginNames();
};