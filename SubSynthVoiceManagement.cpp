#include "SubSynthVoiceManagement.h"
#include <stdio.h>

constexpr int wtSize = 10000;
float wt[wtSize];

SubSynthVoiceManagement::SubSynthVoiceManagement() : counter(0)
{
	for (int i = 0; i < 3; i++)
	{
		envelopeUpdaters[i].init(this, i);
	}
	FILE *f = fopen("TranSynWtConf.txt", "r");
	int id = fgetc(f);
	fclose(f);
	loadWavetable(id);
}
int SubSynthVoiceManagement::getWavetableId()
{
	return wtId;
}

void SubSynthVoiceManagement::loadWavetable(int wtId)
{
	this->wtId = wtId;
	FILE *f;
	char fname[] = "X-TranSyn.raw";
	fname[0] = wtId;
	f = fopen(fname, "rb");
	short buf[wtSize];
	fread(buf, 2, wtSize, f);
	for (int i = 0; i < wtSize; i++)
	{
		wt[i] = buf[i] / 32767.0f;
	}
	fclose(f);

	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].synth.setWavetable(wt);
	}
}

SubSynthVoiceManagement::~SubSynthVoiceManagement()
{
}

void SubSynthVoiceManagement::setSampleRate(int rate)
{
	sampleRate = rate;

	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].synth.setSamplerate(rate);
	}
}

SubSynthParams *SubSynthVoiceManagement::getParams(int paramSet)
{
	if (paramSet >= 0 && paramSet < SS_VOICEMNGMT_PARAMS)
	{
		return &params[paramSet];
	}
	return nullptr;
}

void SubSynthVoiceManagement::setAttack(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope.setAttack((int)(4 * sampleRate * value));
	}
}

void SubSynthVoiceManagement::setDecay(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope.setDecay((int)(4 * sampleRate * value));
	}
}

void SubSynthVoiceManagement::setRelease(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope.setRelease((int)(4 * sampleRate * value));
	}
}

void SubSynthVoiceManagement::onUpdate()
{
	updateSynthParams();
}

void SubSynthVoiceManagement::updateSynthParams()
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].active)
		{
			const int envStage = voices[i].envelope.getStage();
			const float envRatio = voices[i].envelope.getRatio();
			switch (envStage)
			{
			case 0:
				interpolated.paramValuesFromInterpolatedParams(params[0], params[1], envRatio);
				break;
			case 1:
				interpolated.paramValuesFromInterpolatedParams(params[1], params[2], envRatio);
				break;
			case 2:
				interpolated.fromParams(params[2]);
				break;
			case 3:
				interpolated.paramValuesFromInterpolatedParams(params[2], params[3], envRatio);
				break;
			}
			voices[i].synth.getParams()->fromParams(interpolated);
			voices[i].synth.setWavetablePos(wtPos.value);
		}
	}
}

bool SubSynthVoiceManagement::isStereoEnabled()
{
	return stereoEffect.value > 1e-6;
}

void SubSynthVoiceManagement::calculateNext()
{
	updateSynthParams();
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].active)
		{
			voices[i].envelope.calculateNext();
			if (voices[i].envelope.ended())
			{
				voices[i].active = false;
				voices[i].midiNote = -1;
			}
			else
			{
				voices[i].synth.calculateNext();
			}
		}
	}
}

void SubSynthVoiceManagement::getValue(float *ch1, float *ch2)
{
	*ch1 = *ch2 = 0;
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].active)
		{
			const auto velToVol = velocityToVolume.value;
			const auto vol = (1 - velToVol) + velToVol * voices[i].velocity;
			const auto value = voices[i].synth.getValue() * vol;

			if (voices[i].channel == 0)
			{
				*ch1 += value;
			}
			else
			{
				*ch2 += value;
			}
		}
	}
}

void SubSynthVoiceManagement::noteOn(int midiNote, int velocity)
{
	if (isStereoEnabled())
	{
		noteOn((float)midiNote + stereoEffect.value * 0.5, midiNote, velocity, 0);
		noteOn((float)midiNote - stereoEffect.value * 0.5, midiNote, velocity, 1);
	}
	else
	{
		noteOn(midiNote, midiNote, velocity, 0);
	}
}

void SubSynthVoiceManagement::noteOn(float midiNote, int noteNumber, int velocity, int channel)
{
	int lowestCounterIndex = 0;
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (!voices[i].active)
		{
			activateVoice(i, midiNote, noteNumber, velocity, channel);
			return;
		}
		else
		{
			lowestCounterIndex = voices[i].counter < voices[lowestCounterIndex].counter ? i : lowestCounterIndex;
		}
	}
	// steal the voice
	activateVoice(lowestCounterIndex, midiNote, noteNumber, velocity, channel);
}

void SubSynthVoiceManagement::activateVoice(int voiceIndex, float midiNote, int noteNumber, int velocity, int channel)
{
	const float vel1 = velocity / 127.0f;
	voices[voiceIndex].midiNote = noteNumber;
	voices[voiceIndex].active = true;
	voices[voiceIndex].synth.setNoteFrequency(midiNote);
	voices[voiceIndex].synth.setExternalCutoffModulation(vel1 * velocityToFilter.value);
	voices[voiceIndex].envelope.trigger();
	voices[voiceIndex].velocity = vel1;
	voices[voiceIndex].counter = nextCounter();
	voices[voiceIndex].channel = channel;
	updateSynthParams();
}

void SubSynthVoiceManagement::noteOff(int midiNote)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].midiNote == midiNote)
		{
			voices[i].envelope.release();
		}
	}
}

CallbackUpdatable *SubSynthVoiceManagement::getAttackUpdater()
{
	return &envelopeUpdaters[0];
}

CallbackUpdatable *SubSynthVoiceManagement::getDecayUpdater()
{
	return &envelopeUpdaters[1];
}

CallbackUpdatable *SubSynthVoiceManagement::getReleaseUpdater()
{
	return &envelopeUpdaters[2];
}

CallbackUpdatable *SubSynthVoiceManagement::getVelocityToFilterUpdater()
{
	return &velocityToFilter;
}
CallbackUpdatable *SubSynthVoiceManagement::getVelocityToVolumeUpdater()
{
	return &velocityToVolume;
}
CallbackUpdatable *SubSynthVoiceManagement::getStereoEffectUpdater()
{
	return &stereoEffect;
}
CallbackUpdatable *SubSynthVoiceManagement::getWtPosUpdater()
{
	return &wtPos;
}

long SubSynthVoiceManagement::nextCounter()
{
	return ++counter;
}