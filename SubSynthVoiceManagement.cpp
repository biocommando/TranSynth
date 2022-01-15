#include "Options.h"
#include "SubSynthVoiceManagement.h"
#include <vector>
#include <stdio.h>
#include <math.h>

constexpr int wtUpdaterId = 1;
constexpr int lfoMaxRateId = 2;
constexpr int envelopeSpeedId = 3;
constexpr int attackUpdaterId = 4;
constexpr int decayUpdaterId = 5;
constexpr int releaseUpdaterId = 6;
constexpr int filterTypeUpdaterId = 7;
constexpr int cycleEnvelopeUpdaterId = 8;

#define FOR_ALL_VOICES(expr)                             \
    for (int vci = 0; vci < SS_VOICEMNGMT_VOICES; vci++) \
        do                                               \
        {                                                \
            auto voice = &voices[vci];                   \
            expr;                                        \
    } while (0)

SubSynthVoiceManagement::SubSynthVoiceManagement() : counter(0)
{
    wtUpdater.listener = this;
    wtUpdater.id = wtUpdaterId;
    wtUpdater.onUpdateWithValue(0);

    lfoMaxRate.listener = this;
    lfoMaxRate.id = lfoMaxRateId;

    envelopeSpeed.listener = this;
    envelopeSpeed.id = envelopeSpeedId;

    attackUpdater.listener = this;
    attackUpdater.id = attackUpdaterId;

    decayUpdater.listener = this;
    decayUpdater.id = decayUpdaterId;

    releaseUpdater.listener = this;
    releaseUpdater.id = releaseUpdaterId;

    filterType.listener = this;
    filterType.id = filterTypeUpdaterId;

    cycleEnvelope.listener = this;
    cycleEnvelope.id = cycleEnvelopeUpdaterId;

    FOR_ALL_VOICES(voice->synth.setWavetable(wt));
}

void generateBasicWavetable(float *wt, enum OscType t, float vol)
{
    BasicOscillator gen;
    gen.setFrequency(50);
    gen.randomizePhase(0);
    for (int i = 0; i < wtSize; i++)
    {
        gen.calculateNext();
        wt[i] = gen.getValue(t) * vol;
    }
}

void generateFmWavetable(float *wt, int n)
{
    std::string keyBase = "fm_wt" + std::to_string(n) + "_p";
    float ratioOp2 = getOptions()->getFloatOption(keyBase + "1");
    float mod2To1Amt = getOptions()->getFloatOption(keyBase + "2");
    float ratioOp3 = getOptions()->getFloatOption(keyBase + "3");
    float mod3To2Amt = getOptions()->getFloatOption(keyBase + "4");
    float p0 = 0, p1 = 0, p2 = 0;
    const float f0 = 50, f1 = ratioOp2 * 50, f2 = ratioOp3 * 50;
    const float incrementBase = asin(1) * 4 / 44100;

    const float vol = sqrt(2);

    float avg = 0;
    for (int i = 0; i < wtSize; i++)
    {
        p0 += incrementBase * f0;
        p1 += incrementBase * f1;
        p2 += incrementBase * f2;
        const float v2 = sin(p2);
        p1 += v2 * mod3To2Amt;
        const float v1 = sin(p1);
        p0 += v1 * mod2To1Amt;
        const float v0 = sin(p0);

        wt[i] = v0 * vol;
        avg += wt[i];
    }

    avg /= wtSize;

    // Remove DC offset
    if (fabs(avg) > 1e-6)
    {
        for (int i = 0; i < wtSize; i++)
        {
            wt[i] -= avg;
        }
    }
}

void generateAdditWavetable(float *wt, int n)
{
    std::string keyBase = "ad_wt" + std::to_string(n) + "_";
    auto opts = getOptions();

    const float freqs[] = {opts->getFloatOption(keyBase + "f1"),
                           opts->getFloatOption(keyBase + "f2"),
                           opts->getFloatOption(keyBase + "f3"),
                           opts->getFloatOption(keyBase + "f4"), -1};
    const float amplitudes[] = {opts->getFloatOption(keyBase + "a1"),
                                opts->getFloatOption(keyBase + "a2"),
                                opts->getFloatOption(keyBase + "a3"),
                                opts->getFloatOption(keyBase + "a4")};

    std::vector<float> phases;
    float volmult = 0;
    for (int i = 0; freqs[i] > 0; i++)
    {
        volmult += amplitudes[i];
        phases.push_back(0);
    }
    const float vol = sqrt(2) / volmult;
    const float incrementBase = asin(1) * 4 / 44100;
    for (int i = 0; i < wtSize; i++)
    {
        float val = 0;
        for (int j = 0; j < phases.size(); j++)
        {
            val += sin(phases[j]) * amplitudes[j];
            phases[j] += freqs[j] * incrementBase;
        }
        wt[i] = val * vol;
    }
}

void generateSweepWavetable(float *wt, enum OscType t)
{
    const float vol = sqrt(2);

    BasicOscillator gen;
    gen.randomizePhase(0);
    float freq = 50;
    const float speed = 50.0 / wtSize;
    for (int i = 0; i < wtSize; i++)
    {
        gen.setFrequency(freq);
        freq += speed;
        gen.calculateNext();
        wt[i] = gen.getValue(t) * vol;
    }
}

void generatePwmWavetable(float *wt)
{
    int phase = 0;
    const int phaseMax = 44100 / 50;
    const int pwInc = phaseMax / (2 * wtSize / phaseMax);
    int pw = 10;
    for (int i = 0; i < wtSize; i++)
    {
        wt[i] = phase < pw ? 1 : -1;
        if (++phase == phaseMax)
        {
            phase = 0;
            pw += pwInc;
        }
    }
}

void SubSynthVoiceManagement::generateWavetable(int id)
{
    if (id == 0)
        generateBasicWavetable(wt, OSC_SINE, sqrt(2));
    else if (id == 1)
        generateBasicWavetable(wt, OSC_TRIANGLE, sqrt(3));
    else if (id == 2)
        generateBasicWavetable(wt, OSC_SQUARE, 1);
    else if (id == 3)
        generateBasicWavetable(wt, OSC_SAW, sqrt(3));
    else if (id >= 4 && id <= 7)
        generateFmWavetable(wt, id - 3);
    else if (id >= 8 && id <= 11)
        generateAdditWavetable(wt, id - 7);
    else if (id == 12)
        generateSweepWavetable(wt, OSC_SINE);
    else if (id == 13)
        generateSweepWavetable(wt, OSC_SAW);
    else if (id == 14)
        generateSweepWavetable(wt, OSC_SQUARE);
    else if (id == 15)
        generatePwmWavetable(wt);
}

void SubSynthVoiceManagement::onParamUpdated(int id, float value)
{
    switch (id)
    {
    case wtUpdaterId:
    {
        if (lock_wt_to_plugin_generated)
            return;
        const int numValues = 16;
        int newVal = value * numValues;
        if (newVal >= numValues)
            newVal = numValues - 1;
        if (newVal != currentWt || forceRegenerateWavetable)
        {
            forceRegenerateWavetable = false;
            currentWt = newVal;
            generateWavetable(newVal);
        }
    }
    break;
    case lfoMaxRateId:
    {
        FOR_ALL_VOICES(voice->synth.setLfoRange(1 + value * 49));
    }
    break;
    case envelopeSpeedId:
    {
        envelopeScaleFactor = 1 + 29 * value;
        const float s = envelopeScaleFactor * sampleRate;
        FOR_ALL_VOICES(voice->envelope.setAttack((int)(s * attackUpdater.value)));
        FOR_ALL_VOICES(voice->envelope.setDecay((int)(s * decayUpdater.value)));
        FOR_ALL_VOICES(voice->envelope.setRelease((int)(s * releaseUpdater.value)));
    }
    break;
    case attackUpdaterId:
    {
        FOR_ALL_VOICES(voice->envelope.setAttack((int)(envelopeScaleFactor * sampleRate * value)));
    }
    break;
    case decayUpdaterId:
    {
        FOR_ALL_VOICES(voice->envelope.setDecay((int)(envelopeScaleFactor * sampleRate * value)));
    }
    break;
    case releaseUpdaterId:
    {
        FOR_ALL_VOICES(voice->envelope.setRelease((int)(envelopeScaleFactor * sampleRate * value)));
    }
    break;
    case filterTypeUpdaterId:
    {
        const int numValues = 10;
        int newVal = value * numValues;
        if (newVal >= numValues)
            newVal = numValues - 1;
        if (newVal != currentFilterType)
        {
            currentFilterType = newVal;
            FOR_ALL_VOICES(voice->synth.setFilterType(newVal));
        }
    }
    break;
    case cycleEnvelopeUpdaterId:
    {
        FOR_ALL_VOICES(voice->envelope.setCycleOnOff(value > 0.5));
    }
    break;
    }
}

SubSynthVoiceManagement::~SubSynthVoiceManagement()
{
}

void SubSynthVoiceManagement::setSampleRate(int rate)
{
    sampleRate = rate;

    FOR_ALL_VOICES(voice->synth.setSamplerate(rate));
}

SubSynthParams *SubSynthVoiceManagement::getParams(int paramSet)
{
    if (paramSet >= 0 && paramSet < SS_VOICEMNGMT_PARAMS)
    {
        return &params[paramSet];
    }
    return nullptr;
}

void SubSynthVoiceManagement::onUpdate()
{
    updateSynthParams();
}

void SubSynthVoiceManagement::updateSynthParams()
{
    for (int i = 0; i < voiceLimit; i++)
    {
        auto voice = &voices[i];
        if (voice->active)
        {
            auto envelope = &voice->envelope;
            const auto envStage = envelope->getStage();
            auto envRatio = envelope->getRatio();
            switch (envStage)
            {
            case 0:
                interpolated.paramValuesFromInterpolatedParams(params[0], params[1], envRatio);
                break;
            case 1:
                interpolated.paramValuesFromInterpolatedParams(params[1], params[2], envRatio);
                break;
            case 2:
                interpolated.fromParams(params[2]);
                break;
            case 3:
                const auto releaseStage = envelope->getReleaseStage();
                if (releaseStage < 2)
                {
                    // First calculate the state where the voice was left just before release
                    interpolated.paramValuesFromInterpolatedParams(params[releaseStage], params[releaseStage + 1], envelope->getRatio(releaseStage));
                    // Then interpolate the parameters between that state and the final stage
                    interpolated.paramValuesFromInterpolatedParams(interpolated, params[3], envRatio);
                }
                else
                {
                    interpolated.paramValuesFromInterpolatedParams(params[releaseStage], params[3], envRatio);
                }
                break;
            }
            voices[i].synth.getParams()->fromParams(interpolated);
            voices[i].synth.setWavetablePos(wtPos.value);
        }
    }
}

void SubSynthVoiceManagement::setVoiceLimit(int limit)
{
    if (limit > 0 && limit < SS_VOICEMNGMT_VOICES)
        voiceLimit = limit;
}

bool SubSynthVoiceManagement::isStereoEnabled()
{
    return stereoEffect.value > 1e-6;
}

void SubSynthVoiceManagement::calculateNext()
{
    updateSynthParams();
    for (int i = 0; i < voiceLimit; i++)
    {
        if (voices[i].active)
        {
            voices[i].envelope.calculateNext();
            if (voices[i].envelope.ended())
            {
                voices[i].active = false;
                voices[i].midiNote = -1;
            }
            else
            {
                voices[i].synth.calculateNext();
            }
        }
    }
}

void SubSynthVoiceManagement::getValue(float *ch1, float *ch2)
{
    const auto patchVolume = volume.value;
    *ch1 = *ch2 = 0;
    for (int i = 0; i < voiceLimit; i++)
    {
        if (voices[i].active)
        {
            const auto velToVol = velocityToVolume.value;
            const auto vol = (1 - velToVol) + velToVol * voices[i].velocity;
            const auto value = voices[i].synth.getValue() * vol * patchVolume;

            if (voices[i].channel == 0)
            {
                *ch1 += value;
            }
            else
            {
                *ch2 += value;
            }
        }
    }
}

void SubSynthVoiceManagement::noteOn(int midiNote, int velocity)
{
    if (isStereoEnabled())
    {
        noteOn((float)midiNote + stereoEffect.value * 0.5, midiNote, velocity, 0);
        noteOn((float)midiNote - stereoEffect.value * 0.5, midiNote, velocity, 1);
    }
    else
    {
        noteOn(midiNote, midiNote, velocity, 0);
    }
}

void SubSynthVoiceManagement::noteOn(float midiNote, int noteNumber, int velocity, int channel)
{
    int lowestCounterIndex = 0;
    for (int i = 0; i < voiceLimit; i++)
    {
        if (!voices[i].active)
        {
            activateVoice(i, midiNote, noteNumber, velocity, channel);
            return;
        }
        else
        {
            lowestCounterIndex = voices[i].counter < voices[lowestCounterIndex].counter ? i : lowestCounterIndex;
        }
    }
    // steal the voice
    activateVoice(lowestCounterIndex, midiNote, noteNumber, velocity, channel);
}

void SubSynthVoiceManagement::activateVoice(int voiceIndex, float midiNote, int noteNumber, int velocity, int channel)
{
    const float vel1 = velocity / 127.0f;
    auto voice = &voices[voiceIndex];
    voice->midiNote = noteNumber;
    voice->active = true;
    voice->synth.setNoteFrequency(midiNote);
    voice->synth.setExternalCutoffModulation(vel1 * velocityToFilter.value);
    voice->envelope.trigger();
    voice->velocity = vel1;
    voice->counter = nextCounter();
    voice->channel = channel;
    updateSynthParams();
}

void SubSynthVoiceManagement::noteOff(int midiNote)
{
    for (int i = 0; i < voiceLimit; i++)
    {
        if (voices[i].midiNote == midiNote)
        {
            voices[i].envelope.release();
        }
    }
}

CallbackUpdatable *SubSynthVoiceManagement::getVelocityToFilterUpdater()
{
    return &velocityToFilter;
}
CallbackUpdatable *SubSynthVoiceManagement::getVelocityToVolumeUpdater()
{
    return &velocityToVolume;
}
CallbackUpdatable *SubSynthVoiceManagement::getStereoEffectUpdater()
{
    return &stereoEffect;
}
CallbackUpdatable *SubSynthVoiceManagement::getWtPosUpdater()
{
    return &wtPos;
}
CallbackUpdatable *SubSynthVoiceManagement::getWtTypeUpdater()
{
    return &wtUpdater;
}
CallbackUpdatable *SubSynthVoiceManagement::getVolumeUpdater()
{
    return &volume;
}

long SubSynthVoiceManagement::nextCounter()
{
    return ++counter;
}