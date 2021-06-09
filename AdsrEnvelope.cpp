#include "AdsrEnvelope.h"

AdsrEnvelope::AdsrEnvelope()
{
    endReached = true;
    sustain = 0;
    stage = 0;
    stages[0] = new EnvelopeStage(true);
    stages[1] = new EnvelopeStage(true);
    stages[2] = new EnvelopeStage(false);
    stages[3] = new EnvelopeStage(true);
}

AdsrEnvelope::~AdsrEnvelope()
{
    for (int i = 0; i < 4; i++)
    {
        delete stages[i];
    }
}

void AdsrEnvelope::setAttack(int samples)
{
    stages[0]->setLength(samples);
}

void AdsrEnvelope::setDecay(int samples)
{
    stages[1]->setLength(samples);
}

void AdsrEnvelope::setSustain(float level)
{
    sustain = level;
}

void AdsrEnvelope::setRelease(int samples)
{
    stages[3]->setLength(samples);
}

void AdsrEnvelope::calculateNext()
{
    if (endReached)
    {
        return;
    }
    EnvelopeStage *current = stages[stage];
    if (current->hasNext())
    {
        current->calcuateNext();
        float ratio = current->getRatio();
        switch (stage)
        {
        case 0:
            envelope = ratio;
            break;
        case 1:
            envelope = 1 - (1 - sustain) * ratio;
            break;
        case 2:
            envelope = sustain;
            break;
        case 3:
            envelope = releaseLevel * (1 - ratio);
            break;
        default:
            break;
        }
    }
    else if (stage < 3)
    {
        triggerStage(stage + 1);
        calculateNext();
    }
    else
    {
        // release ended
        envelope = 0;
        endReached = true;
    }
}

bool AdsrEnvelope::ended()
{
    return endReached;
}

void AdsrEnvelope::trigger()
{
    endReached = false;
    triggerStage(0);
}
void AdsrEnvelope::triggerStage(int stage)
{
    this->stage = stage;
    stages[stage]->reset();
}

void AdsrEnvelope::release()
{
    releaseLevel = envelope;
    triggerStage(3);
}

int AdsrEnvelope::getStage()
{
    return stage;
}

float AdsrEnvelope::getRatio()
{
    return stages[stage]->getRatio();
}

float AdsrEnvelope::getEnvelope()
{
    return envelope;
}
