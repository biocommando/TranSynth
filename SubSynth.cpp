#include "SubSynth.h"
SubSynth::SubSynth() : osc1(44100),
                       osc2(44100), lfo(44100), ms20Filter(44100),
                       moogFilter(44100), herFilter(44100)
{
    params.setOnParamsChanged(this);
    value = 0;
    midiNote = 0;
    filter = &ms20Filter;
}

void SubSynth::onUpdate()
{
    updated = true;
    //updateParams();
}

void SubSynth::setWavetable(float *wt)
{
    osc1.setWavetable(wt);
    osc2.setWavetable(wt);
}

void SubSynth::setSamplerate(int rate)
{
    osc1.setSamplerate(rate);
    osc2.setSamplerate(rate);
    lfo.setSamplerate(rate);
    //filter->setSamplerate(rate);
    ms20Filter.setSamplerate(rate);
    moogFilter.setSamplerate(rate);
    herFilter.setSamplerate(rate);
    svFilter.setSamplerate(rate);
    noise.setSamplerate(rate);
}

void SubSynth::setFilterType(int type)
{
    updated = true;
    switch (type)
    {
    case 0:
        filter = &ms20Filter;
        break;
    case 1:
        filter = &moogFilter;
        break;
    case 2:
        filter = &herFilter;
        break;
    default:
        svFilter.setFilterType(type - 3);
        filter = &svFilter;
    break;
    }
}

SubSynth::~SubSynth()
{
}

void SubSynth::updateParams()
{
    noteFrequency1 = (float)(pow(2, (midiNote + params.getDetuneAmount()) / 12.0) * 8.1757989156);
    if (params.getDetuneAmount() > 0)
        noteFrequency2 = (float)(pow(2, (midiNote - params.getDetuneAmount()) / 12.0) * 8.1757989156);
    else
        noteFrequency2 = noteFrequency1;

    lfoToPitch = params.getLfoToPitchAmount();
    lfoToCutoff = params.getLfoToCutoffAmount();

    lfo.setFrequency(params.getLfoFrequency() * lfoRange + 0.01f);

    filter->setCutoff(params.getFilterCutoff());
    filter->setResonance(params.getFilterResonance());

    osc1.setWaveTableParams(wtPos, params.getWtWin());
    osc2.setWaveTableParams(wtPos, params.getWtWin());

    noise.setAmount(params.getNoiseAmount());
}

void SubSynth::setWavetablePos(float pos)
{
    if (wtPos != pos)
    {
        wtPos = pos;
        updated = true;
    }
}

void SubSynth::setLfoRange(float maxHz)
{
    if (lfoRange != maxHz)
    {
        lfoRange = maxHz;
        updated = true;
    }
}

void SubSynth::setNoteFrequency(float midiNote)
{
    this->midiNote = midiNote;
    filter->reset();
    osc1.randomizePhase();
    osc2.randomizePhase();
    updated = true;
    released = false;
    osc2MixLevel = 1;
}

SubSynthParams *SubSynth::getParams()
{
    return &params;
}

float SubSynth::distort(float value)
{
    const float d = params.getDistortion();
    if (d == 0)
        return value;
    float distorted = value * value * (value > 0 ? 1.0f : -1.0f);
    distorted *= 200 * d * d;
    distorted = fmin(fmax(distorted, -1.0f), 1.0f);
    return distorted + value * (1.0f - d);
}

float SubSynth::getOscValue(enum OscType osc)
{
    if (noteFrequency1 == noteFrequency2)
    {
        if (osc2MixLevel > 1e-5)
        {
            osc2MixLevel *= .9996;
            return osc1.getValue(osc) + osc2.getValue(osc) * osc2MixLevel;
        }
        return osc1.getValue(osc);
    }
    if (osc2MixLevel < 1 && released)
        osc2MixLevel *= 1.001;
    else
        osc2MixLevel = 1;
    return osc1.getValue(osc) + osc2.getValue(osc) * osc2MixLevel;
}

void SubSynth::calculateNext()
{
    if (updated)
    {
        updateParams();
        updated = false;
    }
    lfo.calculateNext();
    const float lfoValue = lfo.getValue(OSC_TRIANGLE);

    const float pitchMod = 1.0f - lfoValue * lfoToPitch;
    osc1.setFrequency(noteFrequency1 * pitchMod);
    osc2.setFrequency(noteFrequency2 * pitchMod);
    osc1.calculateNext();
    osc2.calculateNext();

    float oscOutput = 0, oscAmount;
    if ((oscAmount = params.getSawAmount()) > 0)
        oscOutput += getOscValue(OSC_SAW) * oscAmount;
    if ((oscAmount = params.getSqrAmount()) > 0)
        oscOutput += getOscValue(OSC_SQUARE) * oscAmount;
    if ((oscAmount = params.getTriAmount()) > 0)
        oscOutput += getOscValue(OSC_TRIANGLE) * oscAmount;
    if ((oscAmount = params.getWtMix()) > 0)
        oscOutput += getOscValue(OSC_WT) * oscAmount;

    oscOutput *= 0.5f;

    oscOutput += noise.get();

    filter->setModulation(lfoToCutoff * lfoValue + externalCutoffModulation);
    const float filterOutput = filter->calculate(oscOutput);

    const float distortionOutput = distort(filterOutput);

    value = distortionOutput * params.getVolume();
}

void SubSynth::setExternalCutoffModulation(float value)
{
    externalCutoffModulation = value;
}

float SubSynth::getValue()
{
    return value;
}
