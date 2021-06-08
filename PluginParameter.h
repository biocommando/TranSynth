#pragma once
#include"CallbackUpdatable.h"

class PluginParameter
{
	float value;
	int id;
	char name[64];
	char shortName[8];
	CallbackUpdatable *callbackUpdatable;
public:
	PluginParameter(const char *name, const char *shortName, int id, float defaultValue = 0);
	~PluginParameter();
	void setOnChanged(CallbackUpdatable *callbackUpdatable);
	void setValue(float value);
	float getValue();
	int getId();
	int serialize(char *writeBuffer);
	void getName(char *name, int maxLen);
	void getShortName(char *name);
	int deserialize(const char *readBuffer);
};

