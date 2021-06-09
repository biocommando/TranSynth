#include "SubSynthParam.h"
#include<stdio.h>
#include<stdlib.h>

SubSynthParam::SubSynthParam() : onUpdate(nullptr), value(0)
{
}

void SubSynthParam::init(CallbackUpdatable *callbackUpdatable)
{
	this->onUpdate = callbackUpdatable;
}

SubSynthParam::~SubSynthParam()
{
}

void SubSynthParam::onUpdateWithValue(float value)
{
	this->value = value;
	onUpdate->onUpdate();
}

float SubSynthParam::getValue()
{
	return value;
}

bool SubSynthParam::setValueBypassingCallback(float value)
{
	if (this->value != value)
	{
		this->value = value;
		return true;
	}
	return false;
}

