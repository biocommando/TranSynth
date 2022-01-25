#pragma once
#include "FilterBase.h"

constexpr int delayBufSize = 4096;

class DelayFilter : public FilterBase
{
    float buffer[delayBufSize];
    float phase = 0;
    float phaseInc = 1;
    float feedback = 0;
    float sampleRate = 1;
    float phaseMod = 0;
    float bufferMs = 10;
public:
    FilterBase *additionalFilter = nullptr;
    DelayFilter()
    {
        reset();
    }

    float calculate(float input)
    {
        if (additionalFilter)
            input = additionalFilter->calculate(input);
        auto newPhase = phase + phaseInc + phaseMod;
        if (newPhase < 0)
            newPhase = 0;

        float bufval;
        for (int i = (int)phase; i < (int)newPhase; i++)
        {
            bufval = buffer[i % delayBufSize];
            buffer[i % delayBufSize] = feedback * bufval - input;
        }
        phase = newPhase;
        if (phase >= delayBufSize)
            phase -= delayBufSize;
        return input + bufval;
    }

    void setCutoff(float v)
    {
        phaseInc = delayBufSize / sampleRate / (v * bufferMs / 1000);
        if (additionalFilter)
            additionalFilter->setCutoff(v);
    }

    void setResonance(float v)
    {
        feedback = v * 0.99999;
        if (additionalFilter)
            additionalFilter->setResonance(v);
    }

    void setModulation(float v)
    {
        phaseMod = v;
        if (additionalFilter)
            additionalFilter->setModulation(v);
    }

    void setSamplerate(int rate)
    {
        sampleRate = rate;
        if (additionalFilter)
            additionalFilter->setSamplerate(rate);
    }

    void reset()
    {
        for (int i = 0; i < delayBufSize; i++)
            buffer[i] = 0;
    }
};