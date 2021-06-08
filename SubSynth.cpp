#include "SubSynth.h"
SubSynth::SubSynth(int samplerate)
{
	osc1 = new BasicOscillator(samplerate);
	osc2 = new BasicOscillator(samplerate);
	lfo = new BasicOscillator(samplerate);
	filter = new MS20Filter(samplerate);
	params = new SubSynthParams();
	params->setOnParamsChanged(this);
	value = 0;
	midiNote = 0;
}

void SubSynth::onUpdate()
{
	updateParams();
}

SubSynth::~SubSynth()
{
	delete osc1;
	delete osc2;
	delete lfo;
	delete filter;
	delete params;
}
extern void logf(char*, float);
void SubSynth::updateParams()
{
	noteFrequency1 = (float)(pow(2, (midiNote + params->getDetuneAmount()) / 12.0) * 8.1757989156);
	noteFrequency2 = (float)(pow(2, (midiNote - params->getDetuneAmount()) / 12.0) * 8.1757989156);

	lfoToPitch = params->getLfoToPitchAmount();
	lfoToCutoff = params->getLfoToCutoffAmount();

	lfo->setFrequency(params->getLfoFrequency() * 5.0f + 0.01f);

	filter->setCutoff(params->getFilterCutoff());
	filter->setResonance(params->getFilterResonance());
}

void SubSynth::setNoteFrequency(int midiNote)
{
	this->midiNote = midiNote;
	this->filter->reset();
	updateParams();
}

SubSynthParams *SubSynth::getParams()
{
	return params;
}

float SubSynth::distort(float value)
{
	float d = params->getDistortion();
	float distorted = value * value * (value > 0 ? 1.0f : -1.0f);
	distorted *= 200 * d * d;
	distorted = fmin(fmax(distorted, -1.0f), 1.0f);
	return distorted + value * (1.0f - d);
}

void SubSynth::calculateNext()
{
	lfo->calculateNext();
	float lfoValue = lfo->getValue(OSC_TRIANGLE);
	
	float fm = 1.0f - lfoValue * lfoToPitch;
	osc1->setFrequency(noteFrequency1 * fm);
	osc2->setFrequency(noteFrequency2 * fm);
	osc1->calculateNext();
	osc2->calculateNext();

	float oscOutput = (osc1->getValue(OSC_SAW) + osc2->getValue(OSC_SAW)) * params->getSawAmount();
	oscOutput += (osc1->getValue(OSC_SQUARE) + osc2->getValue(OSC_SQUARE)) * params->getSqrAmount();
	oscOutput += (osc1->getValue(OSC_TRIANGLE) + osc2->getValue(OSC_TRIANGLE)) * params->getTriAmount();
	oscOutput *= 0.5f;

	filter->setModulation(lfoToCutoff * lfoValue);
	float filterOutput = filter->calculate(oscOutput);

	float distortionOutput = distort(filterOutput);
	
	value = distortionOutput * params->getVolume();
}

float SubSynth::getValue()
{
	return value;
}
