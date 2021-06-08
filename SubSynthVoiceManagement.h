#pragma once
#include <math.h>
#include "SubSynth.h"
#include "AdsrEnvelope.h"
#include "CallbackUpdatable.h"
#include "VCMgmtEnvUpdater.h"

#define SS_VOICEMNGMT_VOICES 16
#define SS_VOICEMNGMT_PARAMS 4

typedef struct {
public:
	AdsrEnvelope *envelope;
	SubSynth *synth;
	float velocity;
	int midiNote;
	bool active;
	long counter;

	void clear()
	{
		delete envelope;
		delete synth;
	}
} SubSynthVoice;


class SubSynthVoiceManagement : CallbackUpdatable, ADSRUpdater
{
	SubSynthVoice voices[SS_VOICEMNGMT_VOICES];
	SubSynthParams *params[SS_VOICEMNGMT_PARAMS];
	SubSynthParams interpolated; // temporary variable, defined as a member because of optimization
	float value;
	int sampleRate;
	void updateSynthParams();
	VCMgmtEnvUpdater *envelopeUpdaters[3];
	long nextCounter();
	void activateVoice(int voiceIndex, int midiNote, int velocity);
	long counter;
public:
	SubSynthVoiceManagement(int samplerate);
	~SubSynthVoiceManagement();
	void onUpdate();
	void calculateNext();
	void setAttack(float value);
	void setDecay(float value);
	void setRelease(float value);
	void noteOn(int midiNote, int velocity);
	void noteOff(int midiNote);

	CallbackUpdatable *getAttackUpdater();
	CallbackUpdatable *getDecayUpdater();
	CallbackUpdatable *getReleaseUpdater();

	float getValue();
	SubSynthParams *getParams(int paramSet);
};

