#pragma once
#include "PluginParameter.h"
#include <string>
#include <vector>

class ParameterHolder
{
    std::vector<PluginParameter> params;

public:
    ParameterHolder();
    void addParameter(PluginParameter param);
    PluginParameter *getParameterById(int id);
    PluginParameter *getParameterByIndex(int index);
    int serialize(char **writeBuffer, const char *header, int headerSize);
    std::string serializeToString();
    void deserialize(const char *readBuffer);
    ~ParameterHolder();
};
