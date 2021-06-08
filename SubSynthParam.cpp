#include "SubSynthParam.h"
#include<stdio.h>
#include<stdlib.h>

int globalId = 0;
SubSynthParam::SubSynthParam(CallbackUpdatable *callbackUpdatable)
{
	id = ++globalId;
	value = 0;
	this->onUpdate = callbackUpdatable;
}


SubSynthParam::~SubSynthParam()
{
}

extern void log(char*);

void SubSynthParam::onUpdateWithValue(float value)
{
	this->value = value;
	onUpdate->onUpdate();
}

float SubSynthParam::getValue()
{
	return value;
}

void SubSynthParam::setValueBypassingCallback(float value)
{
	this->value = value;
}

