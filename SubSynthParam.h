#pragma once
#include "CallbackUpdatable.h"
class SubSynthParam : public CallbackUpdatable
{
    float value;
    CallbackUpdatable *onUpdate;
public:
    void onUpdateWithValue(float value);

    float getValue() { return value; }

    bool setValueBypassingCallback(float value)
    {
        if (this->value != value)
        {
            this->value = value;
            return true;
        }
        return false;
    }

    SubSynthParam();
    ~SubSynthParam();

    void init(CallbackUpdatable *onUpdate);
};
