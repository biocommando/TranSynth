#pragma once
#include "CallbackUpdatable.h"
class SubSynthParam :
	public CallbackUpdatable
{
	float value;
	CallbackUpdatable *onUpdate;
	int id;
public:
	void onUpdateWithValue(float value);
	float getValue();
	void setValueBypassingCallback(float value);

	SubSynthParam(CallbackUpdatable *onUpdate);
	~SubSynthParam();
};

