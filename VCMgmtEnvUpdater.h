#pragma once
#include "CallbackUpdatable.h"
#include "ADSRUpdater.h"
class VCMgmtEnvUpdater : public CallbackUpdatable
{
    ADSRUpdater *parent;
    int type;

public:
    VCMgmtEnvUpdater();

    void init(ADSRUpdater *parent, int type);

    void onUpdateWithValue(float value);

    ~VCMgmtEnvUpdater();
};
