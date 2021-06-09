#pragma once
#include "EnvelopeStage.h"
class AdsrEnvelope
{
    EnvelopeStage *stages[4];
    int stage;
    bool endReached;
    float sustain, releaseLevel;
    float envelope;
    void triggerStage(int stage);

public:
    void setAttack(int samples);
    void setDecay(int samples);
    void setSustain(float level);
    void setRelease(int samples);

    void calculateNext();
    void trigger();
    void release();

    int getStage();
    float getRatio();
    float getEnvelope();
    bool ended();

    AdsrEnvelope();
    ~AdsrEnvelope();
};
