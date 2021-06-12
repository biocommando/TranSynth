#include "SubSynthVoiceManagement.h"
#include <vector>
#include <stdio.h>
#include <math.h>

constexpr int wtSize = 10000;
float wt[wtSize];
extern void logf(const char *, float);

/*char workDir[1024] = {0};
void getWorkDir()
{
    if (workDir[0])
        return;
    // work out the resource directory
    // first we get the DLL path from windows API
    extern void *hInstance;
    GetModuleFileName((HMODULE)hInstance, workDir, 1024);
    // let's get rid of the DLL file name
    for (int i = strlen(workDir) - 1; i >= 0; i--)
        if (workDir[i] == '\\')
        {
            workDir[i] = 0;
            break;
        }
    // add the resource directory to the path
    // strcat(workDir, "\\wavesynth_resources\\");
}*/

/*FILE *openFile(const char *name, const char *mode)
{
    getWorkDir();
    char path[1024];
    sprintf(path, "%s\\%s", workDir, name);

    logf("path", 0);
    logf(path, 0);
    return fopen(path, mode);
}*/

SubSynthVoiceManagement::SubSynthVoiceManagement() : counter(0)
{
    for (int i = 0; i < 3; i++)
    {
        envelopeUpdaters[i].init(this, i);
    }
    wtUpdater.init(this);
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        voices[i].synth.setWavetable(wt);
    }
}

void generateBasicWavetable(enum OscType t, float vol)
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

void generateFmWavetable(float ratioOp2, float mod2To1Amt, float ratioOp3, float mod3To2Amt)
{
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

void generateAdditWavetable(const float *freqs, const float *amplitudes)
{
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

void generateSweepWavetable(enum OscType t)
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

void generatePwmWavetable()
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
    switch (id)
    {
    case 0:
        generateBasicWavetable(OSC_SINE, sqrt(2));
        break;
    case 1:
        generateBasicWavetable(OSC_TRIANGLE, sqrt(3));
        break;
    case 2:
        generateBasicWavetable(OSC_SQUARE, 1);
        break;
    case 3:
        generateBasicWavetable(OSC_SAW, sqrt(3));
        break;
    case 4:
        generateFmWavetable(4, 0.02, 1, 0.05);
        break;
    case 5:
        generateFmWavetable(6, 0.02, 8, 0.1);
        break;
    case 6:
        generateFmWavetable(3, 0.03, 7, 0.1);
        break;
    case 7:
        generateFmWavetable(sqrt(3), 0.05, 0.5, 0.1);
        break;
    case 8:
    {
        const float freqs[] = {50, 100, 150, 200, -1};
        const float amplitudes[] = {1, 0.5, 0.5, 0.25};
        generateAdditWavetable(freqs, amplitudes);
    }
    break;
    case 9:
    {
        const float freqs[] = {50, 75, 150, 250, -1};
        const float amplitudes[] = {1, 0.5, 1, 0.7};
        generateAdditWavetable(freqs, amplitudes);
    }
    break;
    case 10:
    {
        const float freqs[] = {50, 200, 400, 1200, -1};
        const float amplitudes[] = {1, 0.8, 0.6, 0.4};
        generateAdditWavetable(freqs, amplitudes);
    }
    break;
    case 11:
    {
        const float freqs[] = {50, 52, -1};
        const float amplitudes[] = {1, 1};
        generateAdditWavetable(freqs, amplitudes);
    }
    break;
    case 12:
        generateSweepWavetable(OSC_SINE);
        break;
    case 13:
        generateSweepWavetable(OSC_SAW);
        break;
    case 14:
        generateSweepWavetable(OSC_SQUARE);
        break;
    case 15:
        generatePwmWavetable();
        break;
    }
    /*BasicOscillator gen(44100);
    BasicOscillator gen2(44100);
    BasicOscillator gen3(44100);
    gen.setFrequency(50);

    bool useFm = false;

    enum OscType t;
    float vol = 1, gen2Freq;
    if (id == 0 || id >= 4)
    {
        t = OSC_SINE;
        vol = sqrt(2);

        gen2Freq = 50 * id + (id - 4) * 2;
        gen3.setFrequency(25 * (id - 2));
    }
    if (id == 1)
    {
        t = OSC_TRIANGLE;
        vol = sqrt(3);
    }
    if (id == 2)
    {
        t = OSC_SQUARE;
    }
    if (id == 3)
    {
        t = OSC_SAW;
        vol = sqrt(3);
    }
    useFm = id >= 4;
    for (int i = 0; i < wtSize; i++)
    {
        gen.calculateNext();
        wt[i] = gen.getValue(t) * vol;
        if (useFm)
        {
            gen3.calculateNext();
            gen2.setFrequency(gen2Freq + gen2Freq * gen3.getValue(OSC_SINE));
            gen2.calculateNext();
            gen.setFrequency(50 + 50 * gen2.getValue(OSC_SINE));
        }
    }*/
}

SubSynthVoiceManagement::~SubSynthVoiceManagement()
{
}

void SubSynthVoiceManagement::setSampleRate(int rate)
{
    sampleRate = rate;

    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        voices[i].synth.setSamplerate(rate);
    }
}

SubSynthParams *SubSynthVoiceManagement::getParams(int paramSet)
{
    if (paramSet >= 0 && paramSet < SS_VOICEMNGMT_PARAMS)
    {
        return &params[paramSet];
    }
    return nullptr;
}

void SubSynthVoiceManagement::setAttack(float value)
{
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        voices[i].envelope.setAttack((int)(4 * sampleRate * value));
    }
}

void SubSynthVoiceManagement::setDecay(float value)
{
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        voices[i].envelope.setDecay((int)(4 * sampleRate * value));
    }
}

void SubSynthVoiceManagement::setRelease(float value)
{
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        voices[i].envelope.setRelease((int)(4 * sampleRate * value));
    }
}

void SubSynthVoiceManagement::onUpdate()
{
    updateSynthParams();
}

void SubSynthVoiceManagement::updateSynthParams()
{
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        if (voices[i].active)
        {
            const int envStage = voices[i].envelope.getStage();
            const float envRatio = voices[i].envelope.getRatio();
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
                interpolated.paramValuesFromInterpolatedParams(params[2], params[3], envRatio);
                break;
            }
            voices[i].synth.getParams()->fromParams(interpolated);
            voices[i].synth.setWavetablePos(wtPos.value);
        }
    }
}

bool SubSynthVoiceManagement::isStereoEnabled()
{
    return stereoEffect.value > 1e-6;
}

void SubSynthVoiceManagement::calculateNext()
{
    updateSynthParams();
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
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
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
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
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
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
    voices[voiceIndex].midiNote = noteNumber;
    voices[voiceIndex].active = true;
    voices[voiceIndex].synth.setNoteFrequency(midiNote);
    voices[voiceIndex].synth.setExternalCutoffModulation(vel1 * velocityToFilter.value);
    voices[voiceIndex].envelope.trigger();
    voices[voiceIndex].velocity = vel1;
    voices[voiceIndex].counter = nextCounter();
    voices[voiceIndex].channel = channel;
    updateSynthParams();
}

void SubSynthVoiceManagement::noteOff(int midiNote)
{
    for (int i = 0; i < SS_VOICEMNGMT_VOICES; i++)
    {
        if (voices[i].midiNote == midiNote)
        {
            voices[i].envelope.release();
        }
    }
}

CallbackUpdatable *SubSynthVoiceManagement::getAttackUpdater()
{
    return &envelopeUpdaters[0];
}

CallbackUpdatable *SubSynthVoiceManagement::getDecayUpdater()
{
    return &envelopeUpdaters[1];
}

CallbackUpdatable *SubSynthVoiceManagement::getReleaseUpdater()
{
    return &envelopeUpdaters[2];
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