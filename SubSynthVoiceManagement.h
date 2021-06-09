#pragma once
#include <math.h>
#include "SubSynth.h"
#include "AdsrEnvelope.h"
#include "CallbackUpdatable.h"
#include "VCMgmtEnvUpdater.h"

#define SS_VOICEMNGMT_VOICES 16
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

class GenericCallbackUpdatable : public CallbackUpdatable
{
public:
	float value = 0;

	void onUpdateWithValue(float value)
	{
		this->value = value;
	}
};

class SubSynthVoiceManagement : CallbackUpdatable, ADSRUpdater
{
	SubSynthVoice voices[SS_VOICEMNGMT_VOICES];
	SubSynthParams params[SS_VOICEMNGMT_PARAMS];
	SubSynthParams interpolated; // temporary variable, defined as a member because of optimization
	float value;
	int sampleRate;
	void updateSynthParams();
	VCMgmtEnvUpdater envelopeUpdaters[3];
	long nextCounter();
	void activateVoice(int voiceIndex, float midiNote, int noteNumber, int velocity, int channel);
	long counter;

	GenericCallbackUpdatable velocityToVolume;
	GenericCallbackUpdatable velocityToFilter;
	GenericCallbackUpdatable stereoEffect;
	GenericCallbackUpdatable wtPos;

	void noteOn(float midiNote, int noteNumber, int velocity, int channel);
public:
	SubSynthVoiceManagement();
	~SubSynthVoiceManagement();
	void onUpdate();
	void calculateNext();
	void setAttack(float value);
	void setDecay(float value);
	void setRelease(float value);
	void noteOn(int midiNote, int velocity);
	void noteOff(int midiNote);

	void setSampleRate(int rate);

	CallbackUpdatable *getAttackUpdater();
	CallbackUpdatable *getDecayUpdater();
	CallbackUpdatable *getReleaseUpdater();

	CallbackUpdatable *getVelocityToFilterUpdater();
	CallbackUpdatable *getVelocityToVolumeUpdater();
	CallbackUpdatable *getStereoEffectUpdater();
	CallbackUpdatable *getWtPosUpdater();

	bool isStereoEnabled();

	void getValue(float *ch1, float *ch2);
	SubSynthParams *getParams(int paramSet);
};
