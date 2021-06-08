#pragma once
#include "PluginParameter.h"

class ParameterHolder
{
	PluginParameter **params;
	int numOfParams;
public:
	ParameterHolder(int numOfParams);
	PluginParameter *addParameter(PluginParameter *param);
	PluginParameter *getParameterById(int id);
	PluginParameter *getParameterByIndex(int index);
	int serialize(char **writeBuffer);
	void deserialize(char *readBuffer);
	~ParameterHolder();
};

