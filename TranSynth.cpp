// This is the main DLL file.

#include "Options.h"
#include "TranSynth.h"
#include "TranSynthGui.h"
#include "Log.h"
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <memory>

std::string workDir;
void resolveWorkDir()
{
    if (workDir != "")
        return;
    // work out the resource directory
    // first we get the DLL path from windows API
    extern void *hInstance;
    wchar_t workDirWc[1024];
    GetModuleFileName((HMODULE)hInstance, workDirWc, 1024);
    char workDirC[1024];
    wcstombs(workDirC, workDirWc, 1024);

    workDir.assign(workDirC);

    // let's get rid of the DLL file name
    auto posBslash = workDir.find_last_of('\\');
    if (posBslash != std::string::npos)
    {
        workDir = workDir.substr(0, posBslash);
    }

    // Let's find out the actual directory we want to work in
    auto workDirSpec = workDir + "\\TranSynthWorkDir.txt";
    std::ifstream f;
    f.open(workDirSpec);
    std::getline(f, workDirSpec);
    f.close();
    if (workDirSpec != "")
        workDir = workDirSpec;
}

AudioEffect *createEffectInstance(audioMasterCallback audioMaster)
{
    return new TranSynth(audioMaster);
}

int createId(int group, int param)
{
    if (group == -1)
    {
        return 10000 + param;
    }
    return 200 * group + param;
}

int createId(int param)
{
    return createId(-1, param);
}

Options *getOptions()
{
    static bool initDone = false;
    static Options options;
    if (!initDone)
    {
        resolveWorkDir();
        options.read(workDir + "\\options.ini");
        initDone = true;
    }
    return &options;
}

std::ofstream *getLogger()
{
    static bool initDone = false;
    static std::ofstream file;
    if (!initDone)
    {
        file.open(workDir + "\\log.txt");
        initDone = true;
    }
    return &file;
}

TranSynth::TranSynth(audioMasterCallback audioMaster) : AudioEffectX(audioMaster, 0, NUM_PARAMS),
                                                        presetManager(parameterHolder), downsamplingFilterLeft(0, 0),
                                                        downsamplingFilterRight(0, 0), scriptCaller("scripts.txt")
{
    setNumInputs(2);         // stereo in
    setNumOutputs(2);        // stereo out
    setUniqueID(-809164751); // identify
    isSynth(true);
    programsAreChunks();

    voiceMgmt.setVoiceLimit(getOptions()->getIntOption("voice_limit", 16));
    oversampling = getOptions()->getIntOption("oversampling", 1);

    srand((int)time(NULL));
    const float volumes[] = {1, 1, 0.5, 0};

    for (int group = 0; group < 4; group++)
    {
        std::string groupSuffix = '@' + std::to_string(group + 1);
        auto params = voiceMgmt.getParams(group);

        auto name = "TRI Amount";
        auto shortName = "Tri" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_TRI), params->setTriAmount());

        name = "SAW Amount";
        shortName = "Saw" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_SAW), params->setSawAmount(), 1);

        name = "SQR Amount";
        shortName = "Sqr" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_SQR), params->setSqrAmount());

        name = "Wavetable Amount";
        shortName = "Wt" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_WT_MIX), params->setWtMix());

        name = "Wavetable Window";
        shortName = "WtW" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_WT_WIN), params->setWtWin(), 0.0882);

        name = "Detune";
        shortName = "Dtn" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_DETUNE), params->setDetuneAmount());

        name = "Filter Cutoff";
        shortName = "Cut" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_CUTOFF), params->setFilterCutoff(), 1);

        name = "Filter Resonance";
        shortName = "Res" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_RESONANCE), params->setFilterResonance());

        name = "Distortion";
        shortName = "Dist" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_DISTORTION), params->setDistortion());

        name = "LFO Frequency";
        shortName = "LFO_f" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_LFO_FREQ), params->setLfoFrequency(), 0.2f);

        name = "LFO to Pitch";
        shortName = "ModPt" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_LFO_TO_PITCH), params->setLfoToPitchAmount());

        name = "LFO to Filter";
        shortName = "ModFl" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_LFO_TO_CUTOFF), params->setLfoToCutoffAmount());

        name = "Volume";
        shortName = "Vol" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_VOLUME), params->setVolume(), volumes[group]);

        name = "Noise amount";
        shortName = "Noise" + groupSuffix;
        addParameter(name, shortName, createId(group, PARAM_NOISE_AMOUNT), params->setNoiseAmount());
    }

    addParameter("Env. Attack (1->2)", "Attack", createId(PARAM_ATTACK), voiceMgmt.getAttackUpdater(), 0.05f);
    addParameter("Env. Decay (2->3)", "Decay", createId(PARAM_DECAY), voiceMgmt.getDecayUpdater(), 0.2f);
    addParameter("Env. Release (3->4)", "Release", createId(PARAM_RELEASE), voiceMgmt.getReleaseUpdater(), 0.2f);

    addParameter("Velocity to volume", "Vel>Vol", createId(PARAM_VEL_TO_VOLUME), voiceMgmt.getVelocityToVolumeUpdater(), 1);
    addParameter("Velocity to filter", "Vel>Flt", createId(PARAM_VEL_TO_FILTER), voiceMgmt.getVelocityToFilterUpdater(), 0);

    addParameter("Stereo detune", "Stereo", createId(PARAM_STEREO_UNISON_DETUNE), voiceMgmt.getStereoEffectUpdater(), 0);

    addParameter("Wavetable type", "Wavetbl", createId(PARAM_WT_TYPE), voiceMgmt.getWtTypeUpdater());
    addParameter("Wavetable position", "WtblPos", createId(PARAM_WT_POS), voiceMgmt.getWtPosUpdater());

    addParameter("LFO maximum rate", "LFO-max", createId(PARAM_LFO_MAX_RATE), voiceMgmt.getLfoMaxRateUpdater(), 4.0f / 49);
    addParameter("Envelope max length", "EnvLen", createId(PARAM_ENVELOPE_SPEED), voiceMgmt.getEnvelopeSpeedUpdater(), 3.0f / 29);

    addParameter("Filter type", "FltType", createId(PARAM_FILTER_TYPE), voiceMgmt.getFilterTypeUpdater());

    addParameter("Cycle envelope", "CyclEnv", createId(PARAM_CYCLE_ENVELOPE), voiceMgmt.getCycleEnvelopeUpdater());

    addParameter("Volume", "Volume", createId(PARAM_PATCH_VOLUME), voiceMgmt.getVolumeUpdater(), 1.0f);
}

void TranSynth::addParameter(const std::string &name, const std::string &shortName, int id,
                             CallbackUpdatable *cu, float defaultValue)
{
    parameterHolder.addParameter(PluginParameter(name, shortName, id, cu, defaultValue));
}

void TranSynth::open()
{
    downsamplingFilterLeft = MultistageLowpassFilter(sampleRate * oversampling, 4);
    downsamplingFilterRight = MultistageLowpassFilter(sampleRate * oversampling, 4);
    downsamplingFilterLeft.update(sampleRate * 0.5f);
    downsamplingFilterRight.update(sampleRate * 0.5f);
    voiceMgmt.setSampleRate((int)sampleRate * oversampling);
    setEditor(new TranSynthGui(this));
}

TranSynth::~TranSynth()
{
}

VstInt32 TranSynth::getChunk(void **data, bool isPreset)
{
    if (chunk)
    {
        free(chunk);
    }
    const auto pgmName = getPresetManager()->getProgramName();
    const short nameHdrSize = 2 + pgmName.size();
    short hdrSize = 1 + sizeof(short) + 1 + 2 + nameHdrSize;
    auto header = std::make_unique<char[]>(hdrSize);
    header[0] = 'H';
    memcpy(header.get() + 1, &hdrSize, sizeof(short));
    header[3] = 'V';
    header[4] = MAJOR_VERSION;
    header[5] = MINOR_VERSION;

    header[6] = 'N';
    const unsigned char pgmNameLen = pgmName.size();
    memcpy(header.get() + 7, &pgmNameLen, 1);
    memcpy(header.get() + 8, pgmName.c_str(), pgmNameLen);

    auto ret = parameterHolder.serialize((char **)data, header.get(), hdrSize);
    chunk = *(char **)data;
    return ret;
}

VstInt32 TranSynth::setChunk(void *data, VstInt32 byteSize, bool isPreset)
{
    const auto buf = (char *)data;
    if (buf[0] == 'H') // Header present
    {
        short hdrSize;
        memcpy(&hdrSize, buf + 1, sizeof(short));
        const auto header = buf + 1 + sizeof(short);
        int versionMajor = -1; // unknown version
        int versionMinor = -1;
        if (header[0] == 'V') // Version
        {
            versionMajor = header[1];
            versionMinor = header[2];
            LOG_DEBUG("program load", "version maj", versionMajor);
            LOG_DEBUG("program load", "version min", versionMinor);
        }
        if (hdrSize > 6 && header[3] == 'N')
        {
            unsigned char pgmNameLen;
            memcpy(&pgmNameLen, header + 4, 1);
            char pgmName[256];
            memcpy(pgmName, header + 5, pgmNameLen);
            std::string strPgmName(pgmName, pgmNameLen);
            getPresetManager()->setProgramName(strPgmName);
            LOG_DEBUG("setChunk", "Program name: %s", strPgmName.c_str());
            if (editor)
            {
                ((TranSynthGui *)editor)->notifyCurrentPresetNameChanged();
            }
        }
        parameterHolder.deserialize(buf + hdrSize);
    }
    else
    {
        parameterHolder.deserialize(buf);
    }
    return 0;
}

float TranSynth::getParameter(VstInt32 index)
{
    PluginParameter *param = parameterHolder.getParameterByIndex(index);
    if (param == nullptr)
    {
        return 0;
    }
    return param->getValue();
}
void TranSynth::setParameter(VstInt32 index, float value)
{
    PluginParameter *param = parameterHolder.getParameterByIndex(index);
    if (param == nullptr)
    {
        return;
    }
    param->setValue(value);
    if (editor)
    {
        ((TranSynthGui *)editor)->setParameter(param->getId(), value);
    }
}

float TranSynth::getParameterById(int id)
{
    auto p = parameterHolder.getParameterById(id);
    return p ? p->getValue() : 0;
}
void TranSynth::setParameterById(int id, float value)
{
    auto p = parameterHolder.getParameterById(id);
    if (p)
        p->setValue(value);
}
int TranSynth::getIdByIndex(int index)
{
    auto p = parameterHolder.getParameterByIndex(index);
    return p ? p->getId() : -1;
}
int TranSynth::getIndexById(int id)
{
    auto p = parameterHolder.getParameterById(id);
    for (int i = 0; i < NUM_PARAMS; i++)
    {
        if (p == parameterHolder.getParameterByIndex(i))
        {
            return i;
        }
    }
    return -1;
}

void TranSynth::getParameterName(VstInt32 index, char *label)
{
    PluginParameter *param = parameterHolder.getParameterByIndex(index);
    if (param == nullptr)
    {
        return;
    }
    param->getShortName(label);
}

void TranSynth::getParameterLongName(int id, char *label)
{
    auto p = parameterHolder.getParameterById(id);
    if (p)
        p->getName(label, 32);
}

void TranSynth::getParameterDisplay(VstInt32 index, char *text)
{
    PluginParameter *param = parameterHolder.getParameterByIndex(index);
    if (param == nullptr)
    {
        return;
    }
    sprintf_s(text, 8, "%.2f", param->getValue());
}
void TranSynth::getParameterLabel(VstInt32 index, char *label)
{
    strcpy_s(label, 8, "");
}
bool TranSynth::getEffectName(char *name)
{
    strcpy_s(name, 32, "Transition Synth");
    return true;
}
bool TranSynth::getProductString(char *text)
{
    strcpy_s(text, 64, "Transition Synth");
    return true;
}
bool TranSynth::getVendorString(char *text)
{
    strcpy_s(text, 64, "(c) 2017 Joonas Salonpaa");
    return true;
}

VstInt32 TranSynth::processEvents(VstEvents *events)
{
    for (int i = 0; i < events->numEvents; i++)
    {
        if (!(events->events[i]->type & kVstMidiType))
        {
            continue;
        }
        VstMidiEvent *midievent = (VstMidiEvent *)(events->events[i]);
        const char *midiMessage = midievent->midiData;
        if ((midiMessage[0] & 0xF0) == 0b10000000)
        {
            // Note off, key = midievent->midiData[1]
            voiceMgmt.noteOff(midiMessage[1]);
        }
        else if ((midiMessage[0] & 0xF0) == 0b10010000)
        {
            // Note on, key/velocity=midievent->midiData[1]/midievent->midiData[2]
            char key = midiMessage[1];
            char velocity = midiMessage[2];
            voiceMgmt.noteOn(key, velocity);
        }
    }
    return 0;
}

void TranSynth::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
    const bool stereo = voiceMgmt.isStereoEnabled();
    float ch1, ch2;
    for (int i = 0; i < sampleFrames; i++)
    {
        for (int j = 0; j < oversampling; j++)
        {
            voiceMgmt.calculateNext();
            voiceMgmt.getValue(&ch1, &ch2);
            ch1 = downsamplingFilterLeft.process(ch1);
            if (stereo)
                ch2 = downsamplingFilterRight.process(ch2);
        }
        outputs[0][i] = ch1;
        if (stereo)
            outputs[1][i] = ch2;
    }
    if (!stereo)
        memcpy(outputs[1], outputs[0], sampleFrames * sizeof(float));
}

bool PresetManager::readProgram(int number, std::string &name, bool readNameOnly, FILE *copyToTmp)
{
    fseek(f, 0, SEEK_SET);
    char buf[256];
    bool pgFound = false;
    name.assign("");
    bool doLoadProgram = !readNameOnly && !copyToTmp;
    while (!feof(f))
    {
        fgets(buf, sizeof(buf), f);
        char cmd = 0;
        int id = 0;
        float value = 0;
        sscanf(buf, "%c %d %f", &cmd, &id, &value);
        if (cmd == '{' && id == number)
        {
            pgFound = true;
            // Set all parameters to initial value
            if (doLoadProgram && value == 0)
            {
                for (int i = 0; i < NUM_PARAMS; i++)
                {
                    parameterHolder.getParameterByIndex(i)->setInitialValue();
                }
            }
        }
        if (!pgFound)
            continue;

        if (copyToTmp)
            fprintf(copyToTmp, "%s", buf);

        if (cmd == '+' && doLoadProgram)
        {
            if (auto p = parameterHolder.getParameterById(id))
                p->setValue(value);
        }
        if (cmd == '$')
        {
            buf[strlen(buf) - 1] = 0;
            name.assign(&buf[2]);
            if (readNameOnly)
                break;
            if (doLoadProgram)
                curProgramName = name;
        }

        if (cmd == '}')
            break;
    }
    return pgFound;
}

void PresetManager::init()
{
    resolveWorkDir();
    presetNames.clear();
    openFile(0);
    bool pgFound = true;
    for (int i = 0; pgFound; i++)
    {
        std::string name;
        pgFound = readProgram(i, name, true);
        if (pgFound)
            presetNames.push_back(name);
    }
    closeFile();
}
void PresetManager::openFile(int rw)
{
    closeFile();
    std::string presetFileName = workDir + "\\" + fileName;
    f = fopen(presetFileName.c_str(), rw ? "w" : "r");
}

void PresetManager::closeFile()
{
    if (f)
    {
        fclose(f);
        f = nullptr;
    }
}
std::string PresetManager::readProgram(int number)
{
    std::string s;
    openFile(0);
    readProgram(number, s, false);
    closeFile();
    return s;
}

void PresetManager::saveProgram(int number, const std::string &name)
{
    curProgramName = name;
    std::string presetTmpFileName = std::string(workDir) + "\\" + "TranSynPresets.tmp";
    FILE *tmp = fopen(presetTmpFileName.c_str(), "w");

    openFile(0);

    for (int i = 0; i < presetNames.size(); i++)
    {
        if (i != number)
        {
            std::string s;
            readProgram(i, s, false, tmp);
        }
    }

    fprintf(tmp, "{ %d\n$ %s\n", number, name.c_str());
    for (int i = 0; i < NUM_PARAMS; i++)
    {
        auto p = parameterHolder.getParameterByIndex(i);
        fprintf(tmp, "+ %d %f\n", p->getId(), p->getValue());
    }
    fprintf(tmp, "}\n");
    fclose(tmp);
    closeFile();

    openFile(1);
    tmp = fopen(presetTmpFileName.c_str(), "r");
    char ch;
    while ((ch = fgetc(tmp)) != EOF)
        fputc(ch, f);
    fclose(tmp);
    closeFile();
    init();
}