#pragma once
#include "BasicOscillator.h"
#include "MS20Filter.h"
#include "SubSynthParams.h"
#include "CallbackUpdatable.h"
class SubSynth : CallbackUpdatable
{
	BasicOscillator osc1;
	BasicOscillator osc2;
	BasicOscillator lfo;
	MS20Filter filter;
	SubSynthParams params;
	void updateParams();
	float value;
	float noteFrequency1;
	float noteFrequency2;
	int midiNote;
	float lfoToPitch;
	float lfoToCutoff;
	float externalCutoffModulation = 0;
	float distort(float value);
	float getOscValue(enum OscType osc);
public:
	SubSynthParams *getParams();
	SubSynth();
	void setNoteFrequency(int midiNote);
	void calculateNext();
	void setExternalCutoffModulation(float value);
	void onUpdate();
	float getValue();
	void setSamplerate(int rate);
	~SubSynth();
};
