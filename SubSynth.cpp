#include "SubSynth.h"
SubSynth::SubSynth() : osc1(44100),
					   osc2(44100), lfo(44100), filter(44100)
{
	params.setOnParamsChanged(this);
	value = 0;
	midiNote = 0;
}

void SubSynth::onUpdate()
{
	updated = true;
	//updateParams();
}

void SubSynth::setWavetable(float *wt)
{
	osc1.setWavetable(wt);
	osc2.setWavetable(wt);
}

void SubSynth::setSamplerate(int rate)
{
	osc1.setSamplerate(rate);
	osc2.setSamplerate(rate);
	lfo.setSamplerate(rate);
	filter.setSamplerate(rate);
}

SubSynth::~SubSynth()
{
}
extern void logf(char *, float);
void SubSynth::updateParams()
{
	noteFrequency1 = (float)(pow(2, (midiNote + params.getDetuneAmount()) / 12.0) * 8.1757989156);
	if (params.getDetuneAmount() > 0)
		noteFrequency2 = (float)(pow(2, (midiNote - params.getDetuneAmount()) / 12.0) * 8.1757989156);
	else
		noteFrequency2 = noteFrequency1;

	lfoToPitch = params.getLfoToPitchAmount();
	lfoToCutoff = params.getLfoToCutoffAmount();

	lfo.setFrequency(params.getLfoFrequency() * 5.0f + 0.01f);

	filter.setCutoff(params.getFilterCutoff());
	filter.setResonance(params.getFilterResonance());

	osc1.setWaveTableParams(wtPos, params.getWtWin());
	osc2.setWaveTableParams(wtPos, params.getWtWin());
}

void SubSynth::setWavetablePos(float pos)
{
	if (wtPos != pos)
	{
		wtPos = pos;
		updated = true;
	//updateParams();
	}
}

void SubSynth::setNoteFrequency(float midiNote)
{
	this->midiNote = midiNote;
	filter.reset();
	osc1.randomizePhase();
	osc2.randomizePhase();
	updated = true;
	//updateParams();
}

SubSynthParams *SubSynth::getParams()
{
	return &params;
}

float SubSynth::distort(float value)
{
	const float d = params.getDistortion();
	if (d == 0)
		return value;
	float distorted = value * value * (value > 0 ? 1.0f : -1.0f);
	distorted *= 200 * d * d;
	distorted = fmin(fmax(distorted, -1.0f), 1.0f);
	return distorted + value * (1.0f - d);
}

float SubSynth::getOscValue(enum OscType osc)
{
	if (noteFrequency1 == noteFrequency2)
	{
		return osc1.getValue(osc) * 2;
	}
	return osc1.getValue(osc) + osc2.getValue(osc);
}

void SubSynth::calculateNext()
{
	if (updated)
	{
		updateParams();
		updated = false;
	}
	lfo.calculateNext();
	const float lfoValue = lfo.getValue(OSC_TRIANGLE);

	const float pitchMod = 1.0f - lfoValue * lfoToPitch;
	osc1.setFrequency(noteFrequency1 * pitchMod);
	osc2.setFrequency(noteFrequency2 * pitchMod);
	osc1.calculateNext();
	osc2.calculateNext();

	float oscOutput = 0, oscAmount;
	if ((oscAmount = params.getSawAmount()) > 0)
		oscOutput += getOscValue(OSC_SAW) * oscAmount;
	if ((oscAmount = params.getSqrAmount()) > 0)
		oscOutput += getOscValue(OSC_SQUARE) * oscAmount;
	if ((oscAmount = params.getTriAmount()) > 0)
		oscOutput += getOscValue(OSC_TRIANGLE) * oscAmount;
	if ((oscAmount = params.getWtMix()) > 0)
		oscOutput += getOscValue(OSC_WT) * oscAmount;

	oscOutput *= 0.5f;

	filter.setModulation(lfoToCutoff * lfoValue + externalCutoffModulation);
	const float filterOutput = filter.calculate(oscOutput);

	const float distortionOutput = distort(filterOutput);

	value = distortionOutput * params.getVolume();
}

void SubSynth::setExternalCutoffModulation(float value)
{
	externalCutoffModulation = value;
}

float SubSynth::getValue()
{
	return value;
}
