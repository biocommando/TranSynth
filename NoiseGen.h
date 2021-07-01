#pragma once
#include "VAStateVariableFilter.h"
#include "Options.h"
#include <math.h>
#include <stdlib.h>

class NoiseGen
{
    float amount = 0;
    const float randScale = 2.0f / (float)RAND_MAX;
    VAStateVariableFilter filter;

public:
    NoiseGen()
    {
        filter.setFilterType(0);
        filter.setResonance(0);
        filter.setCutoff(getOptions()->getFloatOption("noise_filter_amt", 0.75));
    }

    float get()
    {
        if (amount < 1e-12)
            return 0;
        const auto val = rand() * randScale - 1.0f;
        return filter.calculate(val) * amount;
    }

    void setSamplerate(int sr)
    {
        filter.setSamplerate(sr);
    }

    void setAmount(float amt)
    {
        amount = amt;
    }
};