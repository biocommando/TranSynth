#pragma once
#include "CallbackUpdatable.h"
class SubSynthParam :
    public CallbackUpdatable
{
    float value;
    CallbackUpdatable *onUpdate;
    //int id;
public:
    void onUpdateWithValue(float value);
    float getValue();
    bool setValueBypassingCallback(float value);

    SubSynthParam();
    ~SubSynthParam();

    void init(CallbackUpdatable *onUpdate);
};

