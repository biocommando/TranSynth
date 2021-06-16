#pragma once

#include "FilterBase.h"

#define MACRO_CLIP(input) (input) > 1.0 ? 1.0 : ((input) < -1.0 ? -1.0 : (input))

class HonestEarRapeFilter : public FilterBase
{
private:
    const float FILTER_CONST = 0.3183099f;
    float filterMemory[7];
    float cutoff;
    float resonance;
    float cutoffModulation;
    float sampleRate;

    float cutoffCoeffLA;
    float cutoffCoeffHA;
    float cutoffCoeffLB;
    int cutCalcCounter;

    float hardClip(float input);
    float lpCoeff(float cutoff);
    float hpCoeff(float cutoff);

public:
    void setCutoff(float value);
    void setModulation(float value);
    void setResonance(float value);
    void reset();
    float calculate(float input);
    void setSamplerate(int sampleRate) { this->sampleRate = sampleRate; }
    HonestEarRapeFilter(float sampleRate);
    ~HonestEarRapeFilter();
};
