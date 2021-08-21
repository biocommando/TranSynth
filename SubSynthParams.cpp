#include "SubSynthParams.h"

SubSynthParams::SubSynthParams() : onParamsChangedUpdatable(nullptr)
{
    //onParamsChangedUpdatable = NULL;
    for (int i = 0; i < NUM_SUB_SYNTH_PARAMS; i++)
    {
        params[i].init(this);
    }
}

SubSynthParams::~SubSynthParams()
{
}

float SubSynthParams::getTriAmount()
{
    return params[0].getValue();
}
float SubSynthParams::getSawAmount()
{
    return params[1].getValue();
}
float SubSynthParams::getSqrAmount()
{
    return params[2].getValue();
}
float SubSynthParams::getDetuneAmount()
{
    return params[3].getValue();
}
float SubSynthParams::getFilterCutoff()
{
    return params[4].getValue();
}
float SubSynthParams::getFilterResonance()
{
    return params[5].getValue();
}
float SubSynthParams::getLfoFrequency()
{
    return params[6].getValue();
}
float SubSynthParams::getLfoToPitchAmount()
{
    return params[7].getValue();
}
float SubSynthParams::getLfoToCutoffAmount()
{
    return params[8].getValue();
}
float SubSynthParams::getVolume()
{
    return params[9].getValue();
}
float SubSynthParams::getDistortion()
{
    return params[10].getValue();
}
float SubSynthParams::getWtMix()
{
    return params[11].getValue();
}
float SubSynthParams::getWtWin()
{
    return params[12].getValue();
}
float SubSynthParams::getNoiseAmount()
{
    return params[13].getValue();
}

CallbackUpdatable *SubSynthParams::setTriAmount()
{
    return &params[0];
}
CallbackUpdatable *SubSynthParams::setSawAmount()
{
    return &params[1];
}
CallbackUpdatable *SubSynthParams::setSqrAmount()
{
    return &params[2];
}
CallbackUpdatable *SubSynthParams::setDetuneAmount()
{
    return &params[3];
}
CallbackUpdatable *SubSynthParams::setFilterCutoff()
{
    return &params[4];
}
CallbackUpdatable *SubSynthParams::setFilterResonance()
{
    return &params[5];
}
CallbackUpdatable *SubSynthParams::setLfoFrequency()
{
    return &params[6];
}
CallbackUpdatable *SubSynthParams::setLfoToPitchAmount()
{
    return &params[7];
}
CallbackUpdatable *SubSynthParams::setLfoToCutoffAmount()
{
    return &params[8];
}
CallbackUpdatable *SubSynthParams::setVolume()
{
    return &params[9];
}
CallbackUpdatable *SubSynthParams::setDistortion()
{
    return &params[10];
}
CallbackUpdatable *SubSynthParams::setWtMix()
{
    return &params[11];
}
CallbackUpdatable *SubSynthParams::setWtWin()
{
    return &params[12];
}
CallbackUpdatable *SubSynthParams::setNoiseAmount()
{
    return &params[13];
}

void SubSynthParams::setOnParamsChanged(CallbackUpdatable *callbackUpdatable)
{
    this->onParamsChangedUpdatable = callbackUpdatable;
}

void SubSynthParams::onUpdate()
{
    paramsChanged();
}

void SubSynthParams::paramsChanged()
{
    if (onParamsChangedUpdatable != nullptr)
    {
        onParamsChangedUpdatable->onUpdate();
    }
}

void SubSynthParams::paramValuesFromInterpolatedParams(SubSynthParams &p1, SubSynthParams &p2, float ratio)
{
    bool paramsDidChange = false;
    for (int i = 0; i < NUM_SUB_SYNTH_PARAMS; i++)
    {
        const float v1 = p1.params[i].getValue();
        const float v2 = p2.params[i].getValue();
        if (v1 != v2)
        {
            const float interPolatedValue = v1 + (v2 - v1) * ratio;
            if (fabs(interPolatedValue - params[i].getValue()) > 1e-6)
            {
                paramsDidChange = params[i].setValueBypassingCallback(interPolatedValue) || paramsDidChange;
            }
        }
        else
        {
            paramsDidChange = params[i].setValueBypassingCallback(v1);
        } 
    }
    if (paramsDidChange)
        paramsChanged();
}

void SubSynthParams::fromParams(SubSynthParams &p)
{
    bool updated = false;
    for (int i = 0; i < NUM_SUB_SYNTH_PARAMS; i++)
    {
        updated = params[i].setValueBypassingCallback(p.params[i].getValue()) || updated;
    }
    if (updated)
        paramsChanged();
}
