#pragma once
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "SubSynthParam.h"
#include "CallbackUpdatable.h"

#define NUM_SUB_SYNTH_PARAMS 11

class SubSynthParams : CallbackUpdatable
{
	SubSynthParam params[NUM_SUB_SYNTH_PARAMS];
	CallbackUpdatable *onParamsChangedUpdatable;
public:
	float getTriAmount();
	float getSawAmount();
	float getSqrAmount();
	float getDetuneAmount();
	float getFilterCutoff();
	float getFilterResonance();
	float getLfoFrequency();
	float getLfoToPitchAmount();
	float getLfoToCutoffAmount();
	float getVolume();
	float getDistortion();

	CallbackUpdatable *setTriAmount();
	CallbackUpdatable *setSawAmount();
	CallbackUpdatable *setSqrAmount();
	CallbackUpdatable *setDetuneAmount();
	CallbackUpdatable *setFilterCutoff();
	CallbackUpdatable *setFilterResonance();
	CallbackUpdatable *setLfoFrequency();
	CallbackUpdatable *setLfoToPitchAmount();
	CallbackUpdatable *setLfoToCutoffAmount();
	CallbackUpdatable *setVolume();
	CallbackUpdatable *setDistortion();

	void setOnParamsChanged(CallbackUpdatable *callbackUpdatable);

	void paramsChanged();

	void paramValuesFromInterpolatedParams(SubSynthParams &p1, SubSynthParams &p2, float ratio);
	void fromParams(SubSynthParams &p);

	void onUpdate();

	SubSynthParams();
	~SubSynthParams();
};

