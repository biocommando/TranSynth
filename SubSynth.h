#pragma once
#include "BasicOscillator.h"
#include "MS20Filter.h"
#include "MoogFilter.h"
#include "HonestEarRapeFilter.h"
#include "VAStateVariableFilter.h"
#include "SubSynthParams.h"
#include "CallbackUpdatable.h"
class SubSynth : CallbackUpdatable
{
    BasicOscillator osc1;
    BasicOscillator osc2;
    BasicOscillator lfo;
    MS20Filter ms20Filter;
    MicrotrackerMoog moogFilter;
    HonestEarRapeFilter herFilter;
    VAStateVariableFilter svFilter;

    FilterBase *filter;

    SubSynthParams params;
    void updateParams();
    float value;
    float noteFrequency1;
    float noteFrequency2;
    float midiNote;
    float lfoToPitch;
    float lfoToCutoff;
    float externalCutoffModulation = 0;
    float wtPos = 0;
    float lfoRange = 5;
    // This is used to soften the transition from
    // detuned to no detune situation
    float osc2MixLevel = 0;
    bool released;
    float distort(float value);
    float getOscValue(enum OscType osc);
    bool updated = false;

public:
    SubSynthParams *getParams();
    SubSynth();
    void setNoteFrequency(float midiNote);
    void calculateNext();
    void setExternalCutoffModulation(float value);
    void setWavetablePos(float pos);
    // When note is released certain parameters may
    // immediately jump from value to another,
    // so we need a callback to make the transition
    // softer
    void notifyNoteOff() { released = true; }
    void onUpdate();
    float getValue();
    void setSamplerate(int rate);
    void setWavetable(float *wt);
    void setLfoRange(float maxHz);
    void setFilterType(int type);
    ~SubSynth();
};
