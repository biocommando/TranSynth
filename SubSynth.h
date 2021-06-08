#pragma once
#include "BasicOscillator.h"
#include "MS20Filter.h"
#include "SubSynthParams.h"
#include "CallbackUpdatable.h"
class SubSynth : CallbackUpdatable
{
	BasicOscillator *osc1;
	BasicOscillator *osc2;
	BasicOscillator *lfo;
	MS20Filter *filter;
	SubSynthParams *params;
	void updateParams();
	float value;
	float noteFrequency1;
	float noteFrequency2;
	int midiNote;
	float lfoToPitch;
	float lfoToCutoff;
	float distort(float value);
public:
	SubSynthParams *getParams();
	SubSynth(int samplerate);
	void setNoteFrequency(int midiNote);
	void calculateNext();
	void onUpdate();
	float getValue();
	~SubSynth();
};
