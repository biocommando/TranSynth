// TranSynth.h
#pragma once
#include "audioeffectx.h"
#include "ParameterHolder.h"
#include "SubSynthVoiceManagement.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 0

#define PARAM_WITH_GROUP(group, param) ((group)*200 + (param))
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
#define PARAM_WT_MIX 11
#define PARAM_WT_WIN 12
#define GROUPLESS_PARAM(param) (10000 + (param))
#define PARAM_ATTACK 0
#define PARAM_DECAY 1
#define PARAM_RELEASE 2
#define PARAM_VEL_TO_VOLUME 3
#define PARAM_VEL_TO_FILTER 4
#define PARAM_STEREO_UNISON_DETUNE 5
#define PARAM_WT_POS 6
#define PARAM_WT_TYPE 7
#define PARAM_PATCH_VOLUME 8
#define NUM_PARAMS 61

class TranSynth : public AudioEffectX
{
private:
    ParameterHolder parameterHolder;
    SubSynthVoiceManagement voiceMgmt;
    char *chunk = nullptr;
    char curProgramName[24];

    void addParameter(const std::string &name, const std::string &shortName, int id,
                      CallbackUpdatable *cu, float defaultValue = 0);

public:
    TranSynth(audioMasterCallback audioMaster);
    ~TranSynth();
    VstInt32 getChunk(void **data, bool isPreset);
    VstInt32 setChunk(void *data, VstInt32 byteSize, bool isPreset);

    void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
    float getParameter(VstInt32 index);
    void setParameter(VstInt32 index, float value);
    float getParameterById(int id);
    void setParameterById(int id, float value);
    int getIdByIndex(int index);
    int getIndexById(int id);
    void getParameterName(VstInt32 index, char *label);
    void getParameterDisplay(VstInt32 index, char *text);
    void getParameterLabel(VstInt32 index, char *label);
    bool getEffectName(char *name);
    bool getProductString(char *text);
    bool getVendorString(char *text);
    VstInt32 processEvents(VstEvents *events);
    void open();
};

int createId(int group, int param);

int createId(int param);