#pragma once
enum OscType
{
	OSC_SINE,
	OSC_TRIANGLE,
	OSC_SAW,
	OSC_SQUARE
};

class BasicOscillator
{
private:
	float phase, frequency, hzToF;

public:
	BasicOscillator(int sampleRate);
	~BasicOscillator();
	void calculateNext();
	float getValue(enum OscType oscType);
	void setFrequency(float f_Hz);
	void setSamplerate(int rate);
	void randomizePhase();
};
