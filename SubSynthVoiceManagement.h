#pragma once
#include <math.h>
#include "SubSynth.h"
#include "AdsrEnvelope.h"
#include "CallbackUpdatable.h"

#define SS_VOICEMNGMT_VOICES 128
#define SS_VOICEMNGMT_PARAMS 4

class SubSynthVoice
{
public:
    AdsrEnvelope envelope;
    SubSynth synth;
    float velocity;
    int midiNote;
    bool active;
    long counter;
    int channel;

    SubSynthVoice() : active(false), midiNote(-1)
    {
    }
};

class ParamUpdateListener
{
public:
    virtual void onParamUpdated(int id, float value) = 0;
    virtual ~ParamUpdateListener() {}
};

class GenericCallbackUpdatable : public CallbackUpdatable
{

public:
    ParamUpdateListener *listener = nullptr;
    int id = 0;
    float value = 0;

    void onUpdateWithValue(float value)
    {
        this->value = value;
        if (listener)
        {
            listener->onParamUpdated(id, value);
        }
    }

    virtual ~GenericCallbackUpdatable() {}
};

constexpr int wtSize = 10000;

class SubSynthVoiceManagement : public ParamUpdateListener, CallbackUpdatable
{
    SubSynthVoice voices[SS_VOICEMNGMT_VOICES];
    SubSynthParams params[SS_VOICEMNGMT_PARAMS];
    SubSynthParams interpolated; // temporary variable, defined as a member because of optimization
    float value;
    int sampleRate;
    void updateSynthParams();
    long nextCounter();
    void activateVoice(int voiceIndex, float midiNote, int noteNumber, int velocity, int channel);
    long counter;

    GenericCallbackUpdatable attackUpdater;
    GenericCallbackUpdatable decayUpdater;
    GenericCallbackUpdatable releaseUpdater;

    GenericCallbackUpdatable velocityToVolume;
    GenericCallbackUpdatable velocityToFilter;
    GenericCallbackUpdatable stereoEffect;
    GenericCallbackUpdatable wtPos;
    GenericCallbackUpdatable volume;

    GenericCallbackUpdatable lfoMaxRate;
    GenericCallbackUpdatable envelopeSpeed;

    GenericCallbackUpdatable filterType;
    int currentFilterType = -1;
    
    float envelopeScaleFactor = 4;
    
    GenericCallbackUpdatable cycleEnvelope;

    GenericCallbackUpdatable wtUpdater;
    int currentWt = -1;
    void generateWavetable(int id);
    float wt[wtSize]; // the wavetable samples

    void noteOn(float midiNote, int noteNumber, int velocity, int channel);

    int voiceLimit = 32;
public:
    SubSynthVoiceManagement();
    ~SubSynthVoiceManagement();
    void onUpdate();
    void calculateNext();
    void noteOn(int midiNote, int velocity);
    void noteOff(int midiNote);

    void setSampleRate(int rate);

    CallbackUpdatable *getAttackUpdater() { return &attackUpdater; }
    CallbackUpdatable *getDecayUpdater() { return &decayUpdater; }
    CallbackUpdatable *getReleaseUpdater() { return &releaseUpdater; }

    CallbackUpdatable *getVelocityToFilterUpdater();
    CallbackUpdatable *getVelocityToVolumeUpdater();
    CallbackUpdatable *getStereoEffectUpdater();
    CallbackUpdatable *getWtPosUpdater();
    CallbackUpdatable *getWtTypeUpdater();
    CallbackUpdatable *getVolumeUpdater();
    CallbackUpdatable *getLfoMaxRateUpdater() { return &lfoMaxRate; }
    CallbackUpdatable *getEnvelopeSpeedUpdater() { return &envelopeSpeed; }
    CallbackUpdatable *getFilterTypeUpdater() { return &filterType; }
    CallbackUpdatable *getCycleEnvelopeUpdater() { return &cycleEnvelope; }

    void onParamUpdated(int id, float value);

    bool isStereoEnabled();

    void setVoiceLimit(int limit);

    void getValue(float *ch1, float *ch2);
    SubSynthParams *getParams(int paramSet);
};
