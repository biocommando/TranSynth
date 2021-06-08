#pragma once
#define OSC_SINE 0
#define OSC_TRIANGLE 1
#define OSC_SAW 2
#define OSC_SQUARE 3
class BasicOscillator
{
private:
	float phase, frequency, Hz_to_F;
	float sin1(float phase);
	float tri1(float phase);
	float saw1(float phase);
	float sqr1(float phase);
public:
	BasicOscillator(int sampleRate);
	~BasicOscillator();
	void calculateNext();
	float getValue(int oscType);
	void setFrequency(float f_Hz);
};

