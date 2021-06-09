#pragma once
enum OscType
{
	OSC_SINE,
	OSC_TRIANGLE,
	OSC_SAW,
	OSC_SQUARE,
	OSC_WT
};

class BasicOscillator
{
private:
	float phase, frequency, hzToF;
	int wtPos, wtWindow;
	float *wt = nullptr;
public:
	BasicOscillator(int sampleRate);
	~BasicOscillator();
	void calculateNext();
	void setWavetable(float *wt);
	float getValue(enum OscType oscType);
	void setFrequency(float f_Hz);
	void setWaveTableParams(float pos, float window);
	void setSamplerate(int rate);
	void randomizePhase();
};
