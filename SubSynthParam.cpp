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

void SubSynthParam::setValueBypassingCallback(float value)
{
	this->value = value;
}

