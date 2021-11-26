#pragma once
#include "audioeffectx.h"
#include "SimpleScript.h"
#include <string>
#include <memory>

class ScriptCaller
{
    std::unique_ptr<SimpleScript> script = nullptr;
public:
    ScriptCaller(const std::string &file);

    void execute(const std::string &, AudioEffectX *eff, std::map<std::string, double> *extraParams);

    std::vector<std::string> getEntryPoints();
};