#pragma once
#include "CallbackUpdatable.h"
#include <string>

class PluginParameter
{
    float initialValue;
    float value;
    int id;
    std::string name;
    std::string shortName;
    CallbackUpdatable *callbackUpdatable;
    int serialize(char *writeBuffer);

public:
    PluginParameter(const std::string &name, const std::string &shortName, int id,
                    CallbackUpdatable *cu, float defaultValue = 0);
    ~PluginParameter();
    void setValue(float value);
    void setInitialValue();
    float getValue();
    int getId();
    std::string serializeToString();
    void getName(char *name, int maxLen);
    void getShortName(char *name);
    //int deserialize(const char *readBuffer);
};

int deserializeParameter(const char *readBuffer, int *id, float *value);