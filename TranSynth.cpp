// This is the main DLL file.

#include "stdafx.h"
#include "TranSynth.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
	return new TranSynth(audioMaster);
}

TranSynth::TranSynth(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 0, NUM_PARAMS)
{
	setNumInputs(2);		 // stereo in
	setNumOutputs(2);		 // stereo out
	setUniqueID(-809164751); // identify
}
void log(const char *s)
{
#ifdef DEBUG_LOG
	FILE *f = fopen("D:\\translog.txt", "a");
	fprintf(f, "%s\n", s);
	fclose(f);
#endif
}
void logf(const char *s, float ff)
{
#ifdef DEBUG_LOG
	FILE *f = fopen("D:\\translog.txt", "a");
	fprintf(f, "%s: %f\n", s, ff);
	fclose(f);
#endif
}

int createId(int group, int param)
{
	if (group == -1)
	{
		return 10000 + param;
	}
	return 200 * group + param;
}

int createId(int param)
{
	return createId(-1, param);
}

void TranSynth::addParameter(const std::string &name, const std::string &shortName, int id,
							 CallbackUpdatable *cu, float defaultValue)
{
	parameterHolder.addParameter(PluginParameter(name, shortName, id, cu, defaultValue));
}

void TranSynth::open()
{
	log("open");
	srand((int)time(NULL));
	int sampleRate = (int)this->getSampleRate();
	voiceMgmt.setSampleRate(sampleRate);
	const float volumes[] = {1, 1, 0.5, 0};

	for (int group = 0; group < 4; group++)
	{
		std::string groupPrefix = std::to_string(group + 1) + ":";
		auto params = voiceMgmt.getParams(group);

		auto name = groupPrefix + "Tri amount";
		auto shortName = groupPrefix + "Tri";
		addParameter(name, shortName, createId(group, PARAM_TRI), params->setTriAmount());

		name = groupPrefix + "Saw amount";
		shortName = groupPrefix + "Saw";
		addParameter(name, shortName, createId(group, PARAM_SAW), params->setSawAmount(), 1);

		name = groupPrefix + "Sqr amount";
		shortName = groupPrefix + "Sqr";
		addParameter(name, shortName, createId(group, PARAM_SQR), params->setSqrAmount());

		name = groupPrefix + "Detune";
		shortName = groupPrefix + "Dtn";
		addParameter(name, shortName, createId(group, PARAM_DETUNE), params->setDetuneAmount());

		name = groupPrefix + "Cutoff";
		shortName = groupPrefix + "Cut";
		addParameter(name, shortName, createId(group, PARAM_CUTOFF), params->setFilterCutoff(), 1);

		name = groupPrefix + "Resonance";
		shortName = groupPrefix + "Res";
		addParameter(name, shortName, createId(group, PARAM_RESONANCE), params->setFilterResonance());

		name = groupPrefix + "Distortion";
		shortName = groupPrefix + "Dist";
		addParameter(name, shortName, createId(group, PARAM_DISTORTION), params->setDistortion());

		name = groupPrefix + "LFO frequency";
		shortName = groupPrefix + "LFO-f";
		addParameter(name, shortName, createId(group, PARAM_LFO_FREQ), params->setLfoFrequency(), 0.2f);

		name = groupPrefix + "LFO to pitch";
		shortName = groupPrefix + "ModPt";
		addParameter(name, shortName, createId(group, PARAM_LFO_TO_PITCH), params->setLfoToPitchAmount());

		name = groupPrefix + "LFO to filter";
		shortName = groupPrefix + "ModFl";
		addParameter(name, shortName, createId(group, PARAM_LFO_TO_CUTOFF), params->setLfoToCutoffAmount());

		name = groupPrefix + "Volume";
		shortName = groupPrefix + "Vol";
		addParameter(name, shortName, createId(group, PARAM_VOLUME), params->setVolume(), volumes[group]);
	}

	addParameter("Env. Attack (1->2)", "Attack", createId(PARAM_ATTACK), voiceMgmt.getAttackUpdater(), 0.05f);
	addParameter("Env. Decay (2->3)", "Decay", createId(PARAM_DECAY), voiceMgmt.getDecayUpdater(), 0.2f);
	addParameter("Env. Release (3->4)", "Release", createId(PARAM_RELEASE), voiceMgmt.getReleaseUpdater(), 0.2f);

	addParameter("Velocity to volume", "Vel>Vol", createId(PARAM_VEL_TO_VOLUME), voiceMgmt.getVelocityToVolumeUpdater(), 1);
	addParameter("Velocity to filter", "Vel>Flt", createId(PARAM_VEL_TO_FILTER), voiceMgmt.getVelocityToFilterUpdater(), 0);

	log("open done");
	isSynth(true);
	programsAreChunks();
}

TranSynth::~TranSynth()
{
}

VstInt32 TranSynth::getChunk(void **data, bool isPreset)
{
	return parameterHolder.serialize((char **)data);
}

VstInt32 TranSynth::setChunk(void *data, VstInt32 byteSize, bool isPreset)
{
	log("setChunk");
	parameterHolder.deserialize((char *)data);
	return 0;
}

float TranSynth::getParameter(VstInt32 index)
{
	log("getParameter");
	PluginParameter *param = parameterHolder.getParameterByIndex(index);
	if (param == nullptr)
	{
		log("param null");
		return 0;
	}
	logf("param value", param->getValue());
	return param->getValue();
}
void TranSynth::setParameter(VstInt32 index, float value)
{
	PluginParameter *param = parameterHolder.getParameterByIndex(index);
	if (param == nullptr)
	{
		return;
	}
	param->setValue(value);
}
void TranSynth::getParameterName(VstInt32 index, char *label)
{
	PluginParameter *param = parameterHolder.getParameterByIndex(index);
	if (param == nullptr)
	{
		return;
	}
	param->getShortName(label);
}
void TranSynth::getParameterDisplay(VstInt32 index, char *text)
{
	PluginParameter *param = parameterHolder.getParameterByIndex(index);
	if (param == nullptr)
	{
		return;
	}
	sprintf_s(text, 8, "%.2f", param->getValue());
}
void TranSynth::getParameterLabel(VstInt32 index, char *label)
{
	strcpy_s(label, 8, "");
}
bool TranSynth::getEffectName(char *name)
{
	strcpy_s(name, 32, "Transition Synth");
	return true;
}
bool TranSynth::getProductString(char *text)
{
	strcpy_s(text, 64, "Transition Synth");
	return true;
}
bool TranSynth::getVendorString(char *text)
{
	strcpy_s(text, 64, "(c) 2017 Joonas Salonpaa");
	return true;
}

VstInt32 TranSynth::processEvents(VstEvents *events)
{
	for (int i = 0; i < events->numEvents; i++)
	{
		if (!(events->events[i]->type & kVstMidiType))
		{
			continue;
		}
		VstMidiEvent *midievent = (VstMidiEvent *)(events->events[i]);
		const char *midiMessage = midievent->midiData;
		if ((midiMessage[0] & 0xF0) == 0b10000000)
		{
			// Note off, key = midievent->midiData[1]
			voiceMgmt.noteOff(midiMessage[1]);
		}
		else if ((midiMessage[0] & 0xF0) == 0b10010000)
		{
			// Note on, key/velocity=midievent->midiData[1]/midievent->midiData[2]
			char key = midiMessage[1];
			char velocity = midiMessage[2];
			voiceMgmt.noteOn(key, velocity);
		}
	}
	return 0;
}

void TranSynth::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
	// Real processing goes here
	for (int i = 0; i < sampleFrames; i++)
	{
		voiceMgmt.calculateNext();
		const float value = voiceMgmt.getValue();
		outputs[0][i] = value;
		/*for (int channel = 0; channel < 2; channel++)
		{
			outputs[channel][i] = value;
		}*/
	}
	memcpy(outputs[1], outputs[0], sampleFrames * sizeof(float));
}
