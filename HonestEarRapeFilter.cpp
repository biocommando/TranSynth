#include "HonestEarRapeFilter.h"

HonestEarRapeFilter::HonestEarRapeFilter(float sampleRate) : sampleRate(sampleRate)
{
    reset();
}

HonestEarRapeFilter::~HonestEarRapeFilter()
{
}

void HonestEarRapeFilter::reset()
{
    for (int i = 0; i < 7; i++)
    {
        filterMemory[i] = 0;
    }
    cutoff = 1;
    resonance = 0;
    cutCalcCounter = 0;
    cutoffCoeffHA = cutoffCoeffLA = cutoffCoeffLB = 0;
}

inline float HonestEarRapeFilter::hpCoeff(float cutoff)
{
    const auto cutFreqHz = cutoff * 0.5f * 44100;
    return 1.0f / (2.0f * 3.14159265f * 1.0f / sampleRate * cutFreqHz + 1.0f);
}

inline float HonestEarRapeFilter::lpCoeff(float cutoff)
{
    const auto cutFreqHz = cutoff * 0.5f * 44100;
    return 1.0f / (1.0f / (2.0f * 3.14159265f * 1.0f / sampleRate * cutFreqHz) + 1.0f);
}

static inline float calculateLpFilter(float *filter, float input, float cutoff)
{
    const float output = *filter + cutoff * (input - *filter);
    *filter = output;
    return output;
}

static inline float calculateHpFilter(float *filter, float input, float cutoff)
{
    const float output = cutoff * (input + filter[0] - filter[1]);
    filter[0] = output;
    filter[1] = input;
    return output;
}
float HonestEarRapeFilter::hardClip(float input)
{
    if (input > 1)
        return 1;
    if (input < -1)
        return -1;
    return input;
}

void HonestEarRapeFilter::updateCutoff()
{
    float modulatedCutoff = cutoff + cutoffModulation;
    modulatedCutoff = modulatedCutoff < 0 ? 0 : modulatedCutoff;
    const float cutoffLAHA = modulatedCutoff * 0.5f + 0.002f;
    const float cutoffLB = cutoffLAHA * 0.5f;

    cutoffCoeffLA = lpCoeff(cutoffLAHA);
    cutoffCoeffLB = lpCoeff(cutoffLB);
    cutoffCoeffHA = hpCoeff(cutoffLAHA);
}

float HonestEarRapeFilter::calculate(float input)
{
    const int LA0 = 0, LA1 = 1, LB0 = 2, LB1 = 3, HA = 4, TOTAL = 6;

    if (++cutCalcCounter == 16)
    {
        updateCutoff();

        cutCalcCounter = 0;
    }

    const float LA0Input = filterMemory[TOTAL] + input;
    const float LA0Output = calculateLpFilter(&filterMemory[LA0], LA0Input, cutoffCoeffLA);
    const float LA1Input = LA0Output + filterMemory[TOTAL];
    const float LA1Output = calculateLpFilter(&filterMemory[LA1], LA1Input, cutoffCoeffLA);

    const float LB0Input = LA0Input;
    const float LB0Output = calculateLpFilter(&filterMemory[LB0], LB0Input, cutoffCoeffLB);
    const float LB1Input = LB0Output;
    const float LB1Output = calculateLpFilter(&filterMemory[LB1], LB1Input, cutoffCoeffLB);

    const float SAInput = LA1Output + LB1Output;
    const float SAOutput = MACRO_CLIP(SAInput);

    const float HAInput = input + SAOutput;
    const float HAOutput = calculateHpFilter(&filterMemory[HA], HAInput, cutoffCoeffHA);

    const float SBInput = HAOutput;
    const float SBOutput = MACRO_CLIP(SBInput);

    const float SCInput = SBOutput * resonance * 2;
    const float SCOutput = MACRO_CLIP(SCInput);

    filterMemory[TOTAL] = SCOutput;

    return (HAOutput + SAOutput + LA1Output) * 0.33f;
}

void HonestEarRapeFilter::setCutoff(float value)
{
    cutoff = value;
    if (cutoff < 0)
        cutoff = 0;
}

void HonestEarRapeFilter::setModulation(float value)
{
    cutoffModulation = MACRO_CLIP(value);
}

void HonestEarRapeFilter::setResonance(float value)
{
    resonance = value;
}
