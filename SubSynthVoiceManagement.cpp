#include "SubSynthVoiceManagement.h"
#include <stdio.h>
#include <windows.h>

constexpr int wtSize = 10000;
float wt[wtSize];
extern void logf(const char *, float);

/*char workDir[1024] = {0};
void getWorkDir()
{
	if (workDir[0])
		return;
	// work out the resource directory
	// first we get the DLL path from windows API
	extern void *hInstance;
	GetModuleFileName((HMODULE)hInstance, workDir, 1024);
	// let's get rid of the DLL file name
	for (int i = strlen(workDir) - 1; i >= 0; i--)
		if (workDir[i] == '\\')
		{
			workDir[i] = 0;
			break;
		}
	// add the resource directory to the path
	// strcat(workDir, "\\wavesynth_resources\\");
}*/

/*FILE *openFile(const char *name, const char *mode)
{
	getWorkDir();
	char path[1024];
	sprintf(path, "%s\\%s", workDir, name);

	logf("path", 0);
	logf(path, 0);
	return fopen(path, mode);
}*/

SubSynthVoiceManagement::SubSynthVoiceManagement() : counter(0)
{
	for (int i = 0; i < 3; i++)
	{
		envelopeUpdaters[i].init(this, i);
	}
	wtUpdater.init(this);
	for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
	{
		voices[i].synth.setWavetable(wt);
	}
}

void SubSynthVoiceManagement::generateWavetable(int id)
{
	BasicOscillator gen(44100);
	BasicOscillator gen2(44100);
	BasicOscillator gen3(44100);
	gen.setFrequency(50);
	gen.randomizePhase(0);
	gen2.randomizePhase(0);
	gen3.randomizePhase(0);

	bool useFm = false;
	
	enum OscType t;
	float vol = 1, gen2Freq;
	if (id == 0 || id >= 4)
	{
		t = OSC_SINE;
		vol = 1.0f / sqrt(2);

		gen2Freq = 50 * id;
		gen3.setFrequency(25 * (id + 3));
	}
	if (id == 1)
	{
		t = OSC_TRIANGLE;
		vol = 1.0f / sqrt(3);
	}
	if (id == 2)
	{
		t = OSC_SQUARE;
		vol = 0.5;
	}
	if (id == 3)
	{
		t = OSC_SAW;
		vol = 1.0f / sqrt(3);
	}
	useFm = id >= 4;
	for (int i = 0; i < wtSize; i++)
	{
		gen.calculateNext();
		wt[i] = gen.getValue(t) * vol;
		if (useFm)
		{
			gen3.calculateNext();
			gen2.setFrequency(gen2Freq + gen2Freq * gen3.getValue(OSC_SINE));
			gen2.calculateNext();
			gen.setFrequency(50 + 50 * gen2.getValue(OSC_SINE));
			//wt[i] = wt[i] * gen2.getValue(OSC_TRIANGLE);
		}
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
CallbackUpdatable *SubSynthVoiceManagement::getWtTypeUpdater()
{
	return &wtUpdater;
}

long SubSynthVoiceManagement::nextCounter()
{
	return ++counter;
}