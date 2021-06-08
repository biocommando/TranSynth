// This is the main DLL file.

#include "stdafx.h"
#include "TranSynth.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

AudioEffect* createEffectInstance(audioMasterCallback audioMaster) {
	return new TranSynth(audioMaster);
}

TranSynth::TranSynth(audioMasterCallback audioMaster) :
AudioEffectX(audioMaster, 0, NUM_PARAMS) {
	setNumInputs(2); // stereo in
	setNumOutputs(2); // stereo out
	setUniqueID(-809164751); // identify
}
void log(char *s)
{
	FILE *f = fopen("C:\\Users\\K�ytt�j�\\Documents\\Visual Studio 2013\\Projects\\TransitionSynth\\Release\\log-2.txt", "a");
	fprintf(f, "%s\n", s);
	fclose(f);
}
void logf(char *s, float ff)
{
	FILE *f = fopen("C:\\Users\\K�ytt�j�\\Documents\\Visual Studio 2013\\Projects\\TransitionSynth\\Release\\log-2.txt", "a");
	fprintf(f, "%s: %f\n", s, ff);
	fclose(f);
}

void TranSynth::open()
{
	srand((int)time(NULL));
	int sampleRate = (int) this->getSampleRate();
	voiceMgmt = new SubSynthVoiceManagement(sampleRate);

	parameterHolder = new ParameterHolder(NUM_PARAMS);
	parameterHolder->addParameter(new PluginParameter("1:Tri amount", "1:Tri", PARAM_WITH_GROUP(0, PARAM_TRI)))->setOnChanged(voiceMgmt->getParams(0)->setTriAmount());
	parameterHolder->addParameter(new PluginParameter("1:Saw amount", "1:Saw", PARAM_WITH_GROUP(0, PARAM_SAW), 1))->setOnChanged(voiceMgmt->getParams(0)->setSawAmount());
	parameterHolder->addParameter(new PluginParameter("1:Sqr amount", "1:Sqr", PARAM_WITH_GROUP(0, PARAM_SQR)))->setOnChanged(voiceMgmt->getParams(0)->setSqrAmount());
	parameterHolder->addParameter(new PluginParameter("1:Detune", "1:Dtn", PARAM_WITH_GROUP(0, PARAM_DETUNE)))->setOnChanged(voiceMgmt->getParams(0)->setDetuneAmount());
	parameterHolder->addParameter(new PluginParameter("1:Cutoff", "1:Cut", PARAM_WITH_GROUP(0, PARAM_CUTOFF), 1))->setOnChanged(voiceMgmt->getParams(0)->setFilterCutoff());
	parameterHolder->addParameter(new PluginParameter("1:Resonance", "1:Res", PARAM_WITH_GROUP(0, PARAM_RESONANCE)))->setOnChanged(voiceMgmt->getParams(0)->setFilterResonance());
	parameterHolder->addParameter(new PluginParameter("1:Distortion", "1:Dist", PARAM_WITH_GROUP(0, PARAM_DISTORTION)))->setOnChanged(voiceMgmt->getParams(0)->setDistortion());
	parameterHolder->addParameter(new PluginParameter("1:LFO frequency", "1:LFO-f", PARAM_WITH_GROUP(0, PARAM_LFO_FREQ), 0.2f))->setOnChanged(voiceMgmt->getParams(0)->setLfoFrequency());
	parameterHolder->addParameter(new PluginParameter("1:LFO to pitch", "1:ModPt", PARAM_WITH_GROUP(0, PARAM_LFO_TO_PITCH)))->setOnChanged(voiceMgmt->getParams(0)->setLfoToPitchAmount());
	parameterHolder->addParameter(new PluginParameter("1:LFO to filter", "1:ModFl", PARAM_WITH_GROUP(0, PARAM_LFO_TO_CUTOFF)))->setOnChanged(voiceMgmt->getParams(0)->setLfoToCutoffAmount());
	parameterHolder->addParameter(new PluginParameter("1:Volume", "1:Vol", PARAM_WITH_GROUP(0, PARAM_VOLUME), 1))->setOnChanged(voiceMgmt->getParams(0)->setVolume());

	parameterHolder->addParameter(new PluginParameter("2:Tri amount", "2:Tri", PARAM_WITH_GROUP(1, PARAM_TRI)))->setOnChanged(voiceMgmt->getParams(1)->setTriAmount());
	parameterHolder->addParameter(new PluginParameter("2:Saw amount", "2:Saw", PARAM_WITH_GROUP(1, PARAM_SAW), 1))->setOnChanged(voiceMgmt->getParams(1)->setSawAmount());
	parameterHolder->addParameter(new PluginParameter("2:Sqr amount", "2:Sqr", PARAM_WITH_GROUP(1, PARAM_SQR)))->setOnChanged(voiceMgmt->getParams(1)->setSqrAmount());
	parameterHolder->addParameter(new PluginParameter("2:Detune", "2:Dtn", PARAM_WITH_GROUP(1, PARAM_DETUNE)))->setOnChanged(voiceMgmt->getParams(1)->setDetuneAmount());
	parameterHolder->addParameter(new PluginParameter("2:Cutoff", "2:Cut", PARAM_WITH_GROUP(1, PARAM_CUTOFF), 1))->setOnChanged(voiceMgmt->getParams(1)->setFilterCutoff());
	parameterHolder->addParameter(new PluginParameter("2:Resonance", "2:Res", PARAM_WITH_GROUP(1, PARAM_RESONANCE)))->setOnChanged(voiceMgmt->getParams(1)->setFilterResonance());
	parameterHolder->addParameter(new PluginParameter("2:Distortion", "2:Dist", PARAM_WITH_GROUP(1, PARAM_DISTORTION)))->setOnChanged(voiceMgmt->getParams(1)->setDistortion());
	parameterHolder->addParameter(new PluginParameter("2:LFO frequency", "2:LFO-f", PARAM_WITH_GROUP(1, PARAM_LFO_FREQ), 0.2f))->setOnChanged(voiceMgmt->getParams(1)->setLfoFrequency());
	parameterHolder->addParameter(new PluginParameter("2:LFO to pitch", "2:ModPt", PARAM_WITH_GROUP(1, PARAM_LFO_TO_PITCH)))->setOnChanged(voiceMgmt->getParams(1)->setLfoToPitchAmount());
	parameterHolder->addParameter(new PluginParameter("2:LFO to filter", "2:ModFl", PARAM_WITH_GROUP(1, PARAM_LFO_TO_CUTOFF)))->setOnChanged(voiceMgmt->getParams(1)->setLfoToCutoffAmount());
	parameterHolder->addParameter(new PluginParameter("2:Volume", "2:Vol", PARAM_WITH_GROUP(1, PARAM_VOLUME), 1))->setOnChanged(voiceMgmt->getParams(1)->setVolume());

	parameterHolder->addParameter(new PluginParameter("3:Tri amount", "3:Tri", PARAM_WITH_GROUP(2, PARAM_TRI)))->setOnChanged(voiceMgmt->getParams(2)->setTriAmount());
	parameterHolder->addParameter(new PluginParameter("3:Saw amount", "3:Saw", PARAM_WITH_GROUP(2, PARAM_SAW), 1))->setOnChanged(voiceMgmt->getParams(2)->setSawAmount());
	parameterHolder->addParameter(new PluginParameter("3:Sqr amount", "3:Sqr", PARAM_WITH_GROUP(2, PARAM_SQR)))->setOnChanged(voiceMgmt->getParams(2)->setSqrAmount());
	parameterHolder->addParameter(new PluginParameter("3:Detune", "3:Dtn", PARAM_WITH_GROUP(2, PARAM_DETUNE)))->setOnChanged(voiceMgmt->getParams(2)->setDetuneAmount());
	parameterHolder->addParameter(new PluginParameter("3:Cutoff", "3:Cut", PARAM_WITH_GROUP(2, PARAM_CUTOFF), 1))->setOnChanged(voiceMgmt->getParams(2)->setFilterCutoff());
	parameterHolder->addParameter(new PluginParameter("3:Resonance", "3:Res", PARAM_WITH_GROUP(2, PARAM_RESONANCE)))->setOnChanged(voiceMgmt->getParams(2)->setFilterResonance());
	parameterHolder->addParameter(new PluginParameter("3:Distortion", "3:Dist", PARAM_WITH_GROUP(2, PARAM_DISTORTION)))->setOnChanged(voiceMgmt->getParams(2)->setDistortion());
	parameterHolder->addParameter(new PluginParameter("3:LFO frequency", "3:LFO-f", PARAM_WITH_GROUP(2, PARAM_LFO_FREQ)))->setOnChanged(voiceMgmt->getParams(2)->setLfoFrequency());
	parameterHolder->addParameter(new PluginParameter("3:LFO to pitch", "3:ModPt", PARAM_WITH_GROUP(2, PARAM_LFO_TO_PITCH)))->setOnChanged(voiceMgmt->getParams(2)->setLfoToPitchAmount());
	parameterHolder->addParameter(new PluginParameter("3:LFO to filter", "3:ModFl", PARAM_WITH_GROUP(2, PARAM_LFO_TO_CUTOFF)))->setOnChanged(voiceMgmt->getParams(2)->setLfoToCutoffAmount());
	parameterHolder->addParameter(new PluginParameter("3:Volume", "3:Vol", PARAM_WITH_GROUP(2, PARAM_VOLUME), 0.5))->setOnChanged(voiceMgmt->getParams(2)->setVolume());

	parameterHolder->addParameter(new PluginParameter("4:Tri amount", "4:Tri", PARAM_WITH_GROUP(3, PARAM_TRI)))->setOnChanged(voiceMgmt->getParams(3)->setTriAmount());
	parameterHolder->addParameter(new PluginParameter("4:Saw amount", "4:Saw", PARAM_WITH_GROUP(3, PARAM_SAW), 1))->setOnChanged(voiceMgmt->getParams(3)->setSawAmount());
	parameterHolder->addParameter(new PluginParameter("4:Sqr amount", "4:Sqr", PARAM_WITH_GROUP(3, PARAM_SQR)))->setOnChanged(voiceMgmt->getParams(3)->setSqrAmount());
	parameterHolder->addParameter(new PluginParameter("4:Detune", "4:Dtn", PARAM_WITH_GROUP(3, PARAM_DETUNE)))->setOnChanged(voiceMgmt->getParams(3)->setDetuneAmount());
	parameterHolder->addParameter(new PluginParameter("4:Cutoff", "4:Cut", PARAM_WITH_GROUP(3, PARAM_CUTOFF), 1))->setOnChanged(voiceMgmt->getParams(3)->setFilterCutoff());
	parameterHolder->addParameter(new PluginParameter("4:Resonance", "4:Res", PARAM_WITH_GROUP(3, PARAM_RESONANCE)))->setOnChanged(voiceMgmt->getParams(3)->setFilterResonance());
	parameterHolder->addParameter(new PluginParameter("4:Distortion", "4:Dist", PARAM_WITH_GROUP(3, PARAM_DISTORTION)))->setOnChanged(voiceMgmt->getParams(3)->setDistortion());
	parameterHolder->addParameter(new PluginParameter("4:LFO frequency", "4:LFO-f", PARAM_WITH_GROUP(3, PARAM_LFO_FREQ), 0.2f))->setOnChanged(voiceMgmt->getParams(3)->setLfoFrequency());
	parameterHolder->addParameter(new PluginParameter("4:LFO to pitch", "4:ModPt", PARAM_WITH_GROUP(3, PARAM_LFO_TO_PITCH)))->setOnChanged(voiceMgmt->getParams(3)->setLfoToPitchAmount());
	parameterHolder->addParameter(new PluginParameter("4:LFO to filter", "4:ModFl", PARAM_WITH_GROUP(3, PARAM_LFO_TO_CUTOFF)))->setOnChanged(voiceMgmt->getParams(3)->setLfoToCutoffAmount());
	parameterHolder->addParameter(new PluginParameter("4:Volume", "4:Vol", PARAM_WITH_GROUP(3, PARAM_VOLUME)))->setOnChanged(voiceMgmt->getParams(3)->setVolume());

	PluginParameter *parameter;
	parameter = parameterHolder->addParameter(new PluginParameter("Env. Attack (1->2)", "Attack", GROUPLESS_PARAM(PARAM_ATTACK)));
	parameter->setOnChanged(voiceMgmt->getAttackUpdater());
	parameter->setValue(0.05f);
	parameter = parameterHolder->addParameter(new PluginParameter("Env. Decay (2->3)", "Decay", GROUPLESS_PARAM(PARAM_DECAY)));
	parameter->setOnChanged(voiceMgmt->getDecayUpdater());
	parameter->setValue(0.2f);
	parameter = parameterHolder->addParameter(new PluginParameter("Env. Release (3->4)", "Release", GROUPLESS_PARAM(PARAM_RELEASE)));
	parameter->setOnChanged(voiceMgmt->getReleaseUpdater());
	parameter->setValue(0.2f);
	isSynth(true);
	programsAreChunks();
}

TranSynth::~TranSynth() {
	delete parameterHolder;
	delete voiceMgmt;
}

VstInt32 TranSynth::getChunk(void** data, bool isPreset)
{
	return parameterHolder->serialize((char**)data);
}

VstInt32 TranSynth::setChunk(void* data, VstInt32 byteSize, bool isPreset)
{
	parameterHolder->deserialize((char*)data);
	return 0;
}

float TranSynth::getParameter(VstInt32 index)
{
	PluginParameter *param = parameterHolder->getParameterByIndex(index);
	if (param == NULL)
	{
		return 0;
	}
	return param->getValue();
}
void TranSynth::setParameter(VstInt32 index, float value)
{
	PluginParameter *param = parameterHolder->getParameterByIndex(index);
	if (param == NULL)
	{
		return;
	}
	param->setValue(value);
}
void TranSynth::getParameterName(VstInt32 index, char *label)
{
	PluginParameter *param = parameterHolder->getParameterByIndex(index);
	if (param == NULL)
	{
		return;
	}
	param->getShortName(label);
}
void TranSynth::getParameterDisplay(VstInt32 index, char *text)
{
	PluginParameter *param = parameterHolder->getParameterByIndex(index);
	if (param == NULL)
	{
		return;
	}
	sprintf_s(text, 8, "%.2f", param->getValue());
}
void TranSynth::getParameterLabel(VstInt32 index, char *label)
{
	strcpy_s(label, 8, "");
}
bool TranSynth::getEffectName(char* name)
{
	strcpy_s(name, 32, "Transition Synth");
	return true;
}
bool TranSynth::getProductString(char* text)
{
	strcpy_s(text, 64, "Transition Synth");
	return true;
}
bool TranSynth::getVendorString(char* text)
{
	strcpy_s(text, 64, "(c) 2017 Joonas Salonpaa");
	return true;
}

VstInt32 TranSynth::processEvents(VstEvents* events)
{
	for (int i = 0; i < events->numEvents; i++)
	{
		if (!(events->events[i]->type&kVstMidiType))
		{
			continue;
		}
		VstMidiEvent *midievent = (VstMidiEvent*)(events->events[i]);
		const char *midiMessage = midievent->midiData;
		if (midiMessage[0] >> 4 == -8)
		{
			// Note off, key = midievent->midiData[1]
			voiceMgmt->noteOff(midiMessage[1]);
		}
		else if (midiMessage[0] >> 4 == -7)
		{
			// Note on, key/velocity=midievent->midiData[1]/midievent->midiData[2]
			char key = midiMessage[1];
			char velocity = midiMessage[2];
			voiceMgmt->noteOn(key, velocity);
		}
	}
	return 0;
}

void TranSynth::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames) {
	// Real processing goes here
	for (int i = 0; i < sampleFrames; i++)
	{
		voiceMgmt->calculateNext();
		float value = voiceMgmt->getValue();
		for (int channel = 0; channel < 2; channel++)
		{
			outputs[channel][i] = value;
		}
	}
}
