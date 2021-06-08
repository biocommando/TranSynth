#include "BasicOscillator.h"
#include <stdlib.h>
#include <math.h>

BasicOscillator::BasicOscillator(int sampleRate) : phase(0), hzToF(1.0f / (float)sampleRate),
												   frequency(0)
{
	randomizePhase();
}

BasicOscillator::~BasicOscillator()
{
}

void BasicOscillator::setSamplerate(int rate)
{
	hzToF = 1.0f / (float)rate;
}

void BasicOscillator::randomizePhase()
{
	phase = (float)(rand() % 100000) * 0.00001f;
}

inline float sin1(float phase)
{
	const float phase_rad = phase * 6.283185307179586476925286766559f;
	return sin(phase_rad);
}
inline float tri1(float phase)
{
	if (phase < 0.5)
		return 4 * phase - 1;
	else
		return -4 * phase + 3;
}
inline float saw1(float phase)
{
	return 2 * phase - 1;
}
inline float sqr1(float phase)
{
	return phase < 0.5 ? 1.0f : -1.0f;
}

void BasicOscillator::calculateNext()
{
	phase = phase + frequency;
	if (phase >= 1.0)
		phase = phase - 1.0f;
}
void BasicOscillator::setFrequency(float f_Hz)
{
	frequency = f_Hz * hzToF;
}
float BasicOscillator::getValue(enum OscType oscType)
{
	switch (oscType)
	{
	case OSC_SINE:
		return sin1(phase);
	case OSC_TRIANGLE:
		return tri1(phase);
	case OSC_SAW:
		return saw1(phase);
	case OSC_SQUARE:
		return sqr1(phase);
	default:
		return 0;
	}
}