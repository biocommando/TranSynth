// TranSynth.h
#pragma once
#include "audioeffectx.h"
#include "ParameterHolder.h"
#include "SubSynthVoiceManagement.h"

#define PARAM_WITH_GROUP(group, param) ((group) * 200 + (param))
#define PARAM_TRI 0
#define PARAM_SAW 1
#define PARAM_SQR 2
#define PARAM_DETUNE 3
#define PARAM_CUTOFF 4
#define PARAM_RESONANCE 5
#define PARAM_LFO_FREQ 6
#define PARAM_LFO_TO_PITCH 7
#define PARAM_LFO_TO_CUTOFF 8
#define PARAM_VOLUME 9
#define PARAM_DISTORTION 10
#define GROUPLESS_PARAM(param) (10000 + (param))
#define PARAM_ATTACK 0
#define PARAM_DECAY 1
#define PARAM_RELEASE 2
#define NUM_PARAMS 47

class TranSynth : public AudioEffectX
{
private:
	ParameterHolder *parameterHolder;
	SubSynthVoiceManagement *voiceMgmt;
public:
	TranSynth(audioMasterCallback audioMaster);
	~TranSynth();
	VstInt32 getChunk(void** data, bool isPreset);
	VstInt32 setChunk(void* data, VstInt32 byteSize, bool isPreset);

	void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
	float getParameter(VstInt32 index);
	void setParameter(VstInt32 index, float value);
	void getParameterName(VstInt32 index, char *label);
	void getParameterDisplay(VstInt32 index, char *text);
	void getParameterLabel(VstInt32 index, char *label);
	bool getEffectName(char* name);
	bool getProductString(char* text);
	bool getVendorString(char* text);
	VstInt32 processEvents(VstEvents* events);
	void open();
};
