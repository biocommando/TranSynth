#include "SubSynthVoiceManagement.h"


SubSynthVoiceManagement::SubSynthVoiceManagement(int sampleRate)
{
	counter = 0;
	this->sampleRate = sampleRate;
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].synth = new SubSynth(sampleRate);
		voices[i].envelope = new AdsrEnvelope();
		voices[i].active = false;
		voices[i].midiNote = -1;
	}
	for (int i = 0; i < SS_VOICEMNGMT_PARAMS; i++)
	{
		params[i] = new SubSynthParams();
		// synth params are recalculated on every frame anyway
		// params[i]->setOnParamsChanged(this);
	}

	for (int i = 0; i < 3; i++)
	{
		envelopeUpdaters[i] = new VCMgmtEnvUpdater(this, i);
	}
}


SubSynthVoiceManagement::~SubSynthVoiceManagement()
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].clear();
	}
	for (int i = 0; i < SS_VOICEMNGMT_PARAMS; i++)
	{
		delete params[i];
	}
	for (int i = 0; i < 3; i++)
	{
		delete envelopeUpdaters[i];
	}
}

SubSynthParams *SubSynthVoiceManagement::getParams(int paramSet)
{
	if (paramSet >= 0 && paramSet < SS_VOICEMNGMT_PARAMS)
	{
		return params[paramSet];
	}
	return NULL;
}

void SubSynthVoiceManagement::setAttack(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope->setAttack((int)(4 * sampleRate * value));
	}
}

void SubSynthVoiceManagement::setDecay(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope->setDecay((int)(4 * sampleRate * value));
	}
}

void SubSynthVoiceManagement::setRelease(float value)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].envelope->setRelease((int)(4 * sampleRate * value));
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
			int envStage = voices[i].envelope->getStage();
			float envRatio = voices[i].envelope->getRatio();
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
			voices[i].synth->getParams()->fromParams(&interpolated);
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
			voices[i].envelope->calculateNext();
			if (voices[i].envelope->ended())
			{
				voices[i].active = false;
				voices[i].midiNote = -1;
			}
			else
			{
				voices[i].synth->calculateNext();
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
			value += voices[i].synth->getValue() * voices[i].velocity;
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
	voices[voiceIndex].midiNote = midiNote;
	voices[voiceIndex].active = true;
	voices[voiceIndex].synth->setNoteFrequency(midiNote);
	voices[voiceIndex].envelope->trigger();
	voices[voiceIndex].velocity = velocity / 127.0f;
	voices[voiceIndex].counter = nextCounter();
	updateSynthParams();
}

void SubSynthVoiceManagement::noteOff(int midiNote)
{
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		if (voices[i].midiNote == midiNote)
		{
			voices[i].envelope->release();
		}
	}
}


CallbackUpdatable *SubSynthVoiceManagement::getAttackUpdater()
{
	return envelopeUpdaters[0];
}

CallbackUpdatable *SubSynthVoiceManagement::getDecayUpdater()
{
	return envelopeUpdaters[1];
}

CallbackUpdatable *SubSynthVoiceManagement::getReleaseUpdater()
{
	return envelopeUpdaters[2];
}

long SubSynthVoiceManagement::nextCounter()
{
	return ++counter;
}