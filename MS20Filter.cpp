#include "MS20Filter.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

MS20Filter::MS20Filter(int samplfreq)
{
	T = 1.0f / (float)samplfreq;
	pC1 = 0.0000000033f;
	pR1 = 20000;
	xm2 = 0;
	xm1 = 0;
	ym1 = 0;
	ym2 = 0;
	sampleIdx = 0;
	integratedCutoff = 1;
	setCutoff(1);
	setResonance(0);
	setModulation(0);
	updateConstants();
	update();
}

void MS20Filter::setSamplerate(int rate)
{
	T = 1.0f / (float)rate;
	updateConstants();
	update();
}

MS20Filter::~MS20Filter()
{
}

float MS20Filter::calculate(float input)
{
	integratedCutoff = integratedCutoff + (cutoff - integratedCutoff) * 0.01;
	if (++sampleIdx == 15)
	{
		sampleIdx = 0;
		update();
	}
	float x = (float)input;
	float y = (Ax * xm2 + Bx * xm1 + Cx * x - Ay * ym2 - By * ym1) * iCy;
	// saturation
	if (x < -1)
		x = -1;
	else if (x > 1)
		x = 1;
	xm2 = xm1;
	xm1 = x;
	ym2 = ym1;
	ym1 = y;
	return y;
}

void MS20Filter::update()
{
	k2 = 2.2f * resonance;
	float cmtemp = cutmod + 1;
	cmtemp = 10.0f - (integratedCutoff * cmtemp * 1.4f);
	R1 = (float)pow(2, cmtemp * 0.7382) * pR1;
	R2 = 3 * R1;
	Cx = Cx0 / R1;
	Bx = 2 * Cx;
	Ax = Cx;
	const float tempTerm2 = (2 * k2 * T) / C2;		// dependent on resonance
	const float tempTerm3 = (2 * R2 * T) / (C1 * R1); // dependent on cutoff
	Cy = 4 * R2 + tempTerm1 - tempTerm2 + tempTerm3 + Cx;
	By = ((-8) * R2 + Bx);
	Ay = 4 * R2 - tempTerm1 + tempTerm2 - tempTerm3 + Ax;
	iCy = 1 / Cy;
}

void MS20Filter::updateConstants()
{
	C1 = pC1;
	C2 = C1 * 0.30303f;
	Cx0 = (T * T) / (C1 * C2);
	tempTerm1 = (2 * T) / C1 + (2 * T) / C2;
}

void MS20Filter::setCutoff(float v)
{
	cutoff = 10 * v;
}
void MS20Filter::setResonance(float v)
{
	resonance = v;
}
void MS20Filter::setModulation(float v)
{
	cutmod = v;
}

void MS20Filter::setComponentValues(float c, float r)
{
	pC1 = c;
	pR1 = r;
	updateConstants();
}

void MS20Filter::reset()
{
	integratedCutoff = cutoff;
	xm2 = 0;
	xm1 = 0;
	ym1 = 0;
	ym2 = 0;
	sampleIdx = 0;
	updateConstants();
	update();
}
