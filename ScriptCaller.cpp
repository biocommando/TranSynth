#include "ScriptCaller.h"
#include "Log.h"

extern std::string workDir;
extern void resolveWorkDir();

ScriptCaller::ScriptCaller(const std::string &file)
{
    resolveWorkDir();
    const auto path = workDir + "\\" + file;
    script = std::make_unique<SimpleScript>(path);
}

void ScriptCaller::execute(const std::string &entryPoint, AudioEffectX *eff,
                           std::map<std::string, double> *extraParams)
{
    LOG_DEBUG("execute", "Start script: %s", entryPoint.c_str());
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

    script->execute(
        variables, [](const auto &s) {}, entryPoint);

    // As idx is at the end, we just need to decrease it until zero and we
    // can be sure that the parameters exist
    for (idx = idx - 1; idx >= 0; idx--)
    {
        eff->getParameterName(idx, paramName);
        eff->setParameter(idx, variables[paramName]);
    }
}

std::vector<std::string> ScriptCaller::getEntryPoints()
{
    return script->listEntryPoints();
}