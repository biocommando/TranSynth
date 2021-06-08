#include "SubSynthVoiceManagement.h"


SubSynthVoiceManagement::SubSynthVoiceManagement() : counter(0)
{
	for (int i = 0; i < 3; i++)
	{
		envelopeUpdaters[i].init(this, i);
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
		}
	}
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

float SubSynthVoiceManagement::getValue()
{
	float value = 0;
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].active)
		{
			const auto velToVol = velocityToVolume.value;
			const auto vol = (1 - velToVol) + velToVol * voices[i].velocity;
			value += voices[i].synth.getValue() * vol;
		}
	}
	return value;
}

void SubSynthVoiceManagement::noteOn(int midiNote, int velocity)
{
	int lowestCounterIndex = 0;
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (!voices[i].active)
		{
			activateVoice(i, midiNote, velocity);
			return;
		}
		else
		{
			lowestCounterIndex = voices[i].counter < voices[lowestCounterIndex].counter ? i : lowestCounterIndex;
		}
	}
	// steal the voice
	activateVoice(lowestCounterIndex, midiNote, velocity);
}

void SubSynthVoiceManagement::activateVoice(int voiceIndex, int midiNote, int velocity)
{
	const float vel1 = velocity / 127.0f;
	voices[voiceIndex].midiNote = midiNote;
	voices[voiceIndex].active = true;
	voices[voiceIndex].synth.setNoteFrequency(midiNote);
	voices[voiceIndex].synth.setExternalCutoffModulation(vel1 * velocityToFilter.value);
	voices[voiceIndex].envelope.trigger();
	voices[voiceIndex].velocity = vel1;
	voices[voiceIndex].counter = nextCounter();
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

long SubSynthVoiceManagement::nextCounter()
{
	return ++counter;
}