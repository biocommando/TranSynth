#pragma once
#include "aeffguieditor.h"
#include "TranSynth.h"
#include <vector>

constexpr int TOP_MARGIN = 30;
constexpr int LEFT_MARGIN = 10;

constexpr int GRID_SIZE = 60;

constexpr int KNOB_SIZE = 40;

constexpr int TEXT_H = 16;

#define GRID_X(x) LEFT_MARGIN + GRID_SIZE *(x)

#define GRID_Y(y) TOP_MARGIN + GRID_SIZE *(y)

#define GRID_RECT(name, x, y, w, h) CRect name(GRID_X(x), GRID_Y(y), GRID_X(x) + (w), GRID_Y(y) + (h))

#define ADD_TEXT(text, x, y, w, h, aexpr)     \
    do                                        \
    {                                         \
        GRID_RECT(r, x, y, w, h);             \
        auto label = new CTextLabel(r, text); \
        setColors(label, false);              \
        xframe->addView(label);               \
        aexpr;                                \
    } while (0)

constexpr int tagLinkModeList = 101;
constexpr int tagPresetList = 102;
constexpr int tagPresetActionList = 103;
constexpr int tagGroupedParam = 201;
constexpr int tagGlobalParam = 202;
constexpr int tagPresetNameEdit = 301;

extern void logf(const char *, float);

extern CColor gold, bggray;

extern std::string workDir;
extern void resolveWorkDir();

class Knob : public CKnob
{
public:
    int paramId;
    CTextLabel *label;
    int group, param;

    Knob(const CRect &size, CControlListener *listener, long tag, CBitmap *background,
         CBitmap *handle, int id) : CKnob(size, listener, tag, background, handle), paramId(id)
    {
    }

    void setValue(float value)
    {
        CKnob::setValue(value);
        setLabel(value);
    }

    void setLabel(float value)
    {
        if (paramId == createId(PARAM_WT_TYPE))
        {
            int v = value * 16;
            if (v >= 16)
                v = 15;
            const char waveTableNames[][8] = {
                "SIN", "TRI", "SQR", "SAW",
                "FM-1", "FM-2", "FM-3", "FM-4",
                "Adt-1", "Adt-2", "Adt-3", "Adt-4",
                "sinsw", "sawsw", "sqrsw", "PWM"};
            label->setText(waveTableNames[v]);
            return;
        }
        else if (paramId == createId(PARAM_FILTER_TYPE))
        {
            int v = value * 10;
            if (v >= 10)
                v = 9;
            const char FilterTypeNames[][8] = {
                "MS20", "Moog", "Aggro",
                "LoPass", "HiPass", "BPass",
                "BShelf", "Notch", "APass",
                "Peak"
            };
            label->setText(FilterTypeNames[v]);
            return;
        }
        char text[5];
        int vint = value * 99;
        if (vint < 10)
            sprintf(text, "0%d", vint);
        else
            sprintf(text, "%d", vint);
        label->setText(text);
    }
};

class TranSynthGui : public AEffGUIEditor, public CControlListener
{

    COptionMenu *linkModeList = nullptr;
    COptionMenu *presetList = nullptr;
    COptionMenu *presetActionList = nullptr;
    CBitmap *knobBackground = nullptr;
    CTextEdit *currentPresetNameEdit = nullptr;

    std::string currentPresetName = "<Preset name>";

    std::vector<Knob *> knobs;
    int linkMode[4];
    int presetNumber = -1;

    TranSynth *synth()
    {
        return (TranSynth *)effect;
    }

    void setLinkMode(int a, int d, int s, int r)
    {
        linkMode[0] = a;
        linkMode[1] = d;
        linkMode[2] = s;
        linkMode[3] = r;
    }

    void setLinkModeFromListIdx()
    {
        const auto i = linkModeList->getCurrentIndex() - 1;
        switch (i)
        {
        case 0:
            setLinkMode(0, 0, 0, 0);
            break;
        case 1:
            setLinkMode(1, 1, 0, 0);
            break;
        case 2:
            setLinkMode(1, 1, 1, 0);
            break;
        case 3:
            setLinkMode(1, 1, 1, 1);
            break;
        case 4:
            setLinkMode(0, 1, 1, 0);
            break;
        case 5:
            setLinkMode(0, 1, 1, 1);
            break;
        case 6:
            setLinkMode(0, 0, 1, 1);
            break;
        case 7:
            setLinkMode(1, 0, 1, 0);
            break;
        case 8:
            setLinkMode(1, 1, 0, 1);
            break;
        case 9:
            setLinkMode(1, 0, 1, 1);
            break;
        case 10:
            setLinkMode(1, 0, 0, 1);
            break;
        case 11:
            setLinkMode(0, 1, 0, 1);
            break;
        case 12:
            setLinkMode(1, 1, 2, 2); // A-D, S-R
            break;
        case 13:
            setLinkMode(2, 1, 1, 2); // A-R, D-S
            break;
        case 14:
            setLinkMode(1, 2, 1, 2); // A-S, D-R
            break;
        }
    }

    CBitmap *loadBitmap(const char *relativePath)
    {
        resolveWorkDir();
        std::string s = workDir + "\\" + relativePath;
        std::wstring ws(s.size(), L'#');
        mbstowcs(&ws[0], s.c_str(), s.size());
        auto bmp = Gdiplus::Bitmap::FromFile(ws.c_str(), false);
        auto cbmp = new CBitmap(bmp);
        delete bmp;
        return cbmp;
    }

    Knob *addKnob(CFrame *xframe, int x, int y, int id, int tag)
    {
        const CColor cBg = kBlackCColor, cFg = gold;
        GRID_RECT(knobRect, x, y, KNOB_SIZE, KNOB_SIZE);
        auto knob = new Knob(knobRect, this, tag, knobBackground, nullptr, id);
        ADD_TEXT("00", x, y + 0.7, KNOB_SIZE, TEXT_H, knob->label = label);
        xframe->addView(knob);
        knob->setValue(synth()->getParameterById(id));
        knobs.push_back(knob);
        return knob;
    }

    void setColors(CParamDisplay *ctrl, bool setFrameColor = true)
    {
        const CColor cBg = kBlackCColor, cFg = gold;
        ctrl->setBackColor(cBg);
        if (setFrameColor)
            ctrl->setFrameColor(cFg);
        ctrl->setFontColor(cFg);
    }

public:
    TranSynthGui(void *ptr) : AEffGUIEditor(ptr)
    {
        setLinkMode(0, 0, 0, 0);
    }
    ~TranSynthGui()
    {
    }

    bool open(void *ptr)
    {
        CRect frameSize(0, 0, GRID_X(18) + LEFT_MARGIN, GRID_Y(9));
        CColor cBg = kBlackCColor, cFg = gold;
        ERect *wSize;
        getRect(&wSize);

        wSize->top = wSize->left = 0;
        wSize->bottom = (VstInt16)frameSize.bottom;
        wSize->right = (VstInt16)frameSize.right;

        auto xframe = new CFrame(frameSize, ptr, this);

        knobBackground = loadBitmap("TranSynKnob.bmp");

        xframe->setBackgroundColor(cBg);

        ADD_TEXT("Stage parameter link mode", 0, 8.5, GRID_SIZE * 3, TEXT_H, label->setHoriAlign(kLeftText));
        GRID_RECT(optionRect, 3, 8.5, 2 * GRID_SIZE, TEXT_H);

        linkModeList = new COptionMenu(optionRect, this, tagLinkModeList);
        setColors(linkModeList);

        linkModeList->addEntry(new CMenuItem("Link mode...", 1 << 1));
        linkModeList->addEntry(new CMenuItem("No link"));
        linkModeList->addEntry(new CMenuItem("A-D"));
        linkModeList->addEntry(new CMenuItem("A-D-S"));
        linkModeList->addEntry(new CMenuItem("A-D-S-R"));
        linkModeList->addEntry(new CMenuItem("D-S"));
        linkModeList->addEntry(new CMenuItem("D-S-R"));
        linkModeList->addEntry(new CMenuItem("S-R"));
        linkModeList->addEntry(new CMenuItem("A-S"));
        linkModeList->addEntry(new CMenuItem("A-D-R"));
        linkModeList->addEntry(new CMenuItem("A-S-R"));
        linkModeList->addEntry(new CMenuItem("A-R"));
        linkModeList->addEntry(new CMenuItem("D-R"));
        linkModeList->addEntry(new CMenuItem("A-D, S-R"));
        linkModeList->addEntry(new CMenuItem("A-R, D-S"));
        linkModeList->addEntry(new CMenuItem("A-S, D-R"));

        xframe->addView(linkModeList);

        const int groupedParams[] = {
            PARAM_TRI, PARAM_SAW, PARAM_SQR, PARAM_WT_MIX, PARAM_WT_WIN, PARAM_DETUNE,
            PARAM_DISTORTION, PARAM_CUTOFF, PARAM_RESONANCE, PARAM_LFO_FREQ, PARAM_LFO_TO_CUTOFF,
            PARAM_LFO_TO_PITCH, PARAM_VOLUME, -1};
        const char stageNames[][20] = {
            "Attack", "Decay", "Sustain", "Release"};
        knobs.clear();

        for (int i = 0; groupedParams[i] >= 0; i++)
        {
            int xOffset = i > 6 ? 6 : 0;
            int yOffset = i > 6 ? -7 : 0;
            char buf[32];
            synth()->getParameterLongName(createId(0, groupedParams[i]), buf);
            ADD_TEXT(buf, xOffset, i + 1.25 + yOffset, GRID_SIZE * 2, TEXT_H, label->setHoriAlign(kLeftText));
            for (int j = 0; j < 4; j++)
            {
                if (i == 0)
                {
                    ADD_TEXT(stageNames[j], 2 + j - 0.1, 0.5, GRID_SIZE, TEXT_H, );
                    ADD_TEXT(stageNames[j], 8 + j - 0.1, 0.5, GRID_SIZE, TEXT_H, );
                }

                auto id = createId(j, groupedParams[i]);
                auto knob = addKnob(xframe, 2 + j + xOffset, i + 1 + yOffset, id, tagGroupedParam);
                knob->group = j;
                knob->param = groupedParams[i];
            }
        }

        const int globalParams[] = {
            PARAM_ATTACK, PARAM_DECAY, PARAM_RELEASE, PARAM_ENVELOPE_SPEED,
            PARAM_STEREO_UNISON_DETUNE, PARAM_LFO_MAX_RATE, PARAM_FILTER_TYPE,
            PARAM_WT_POS, PARAM_WT_TYPE, PARAM_VEL_TO_FILTER, PARAM_VEL_TO_VOLUME,
            PARAM_PATCH_VOLUME, -1};

        ADD_TEXT("G L O B A L  P A R A M E T E R S", 12, 0.5, GRID_X(6), TEXT_H, );

        for (int i = 0; globalParams[i] >= 0; i++)
        {
            int xOffset = i > 6 ? 3 : 0;
            int yOffset = i > 6 ? -7 : 0;
            const auto id = createId(globalParams[i]);
            char buf[32];
            synth()->getParameterLongName(id, buf);
            ADD_TEXT(buf, 12 + xOffset, i + 1.25 + yOffset, GRID_X(2), TEXT_H, label->setHoriAlign(kLeftText));
            auto knob = addKnob(xframe, 14 + xOffset, i + 1 + yOffset, id, tagGlobalParam);
        }

        GRID_RECT(presetNameEditRect, 6, 8, GRID_SIZE * 6, TEXT_H * 1.5);
        currentPresetNameEdit = new CTextEdit(presetNameEditRect, this, tagPresetNameEdit, currentPresetName.c_str());
        setColors(currentPresetNameEdit);
        currentPresetNameEdit->setBackColor(bggray);

        xframe->addView(currentPresetNameEdit);

        ADD_TEXT("Preset", 6, 8.5, GRID_SIZE * 1, TEXT_H, label->setHoriAlign(kLeftText));
        GRID_RECT(presetRect, 7, 8.5, 2 * GRID_SIZE, TEXT_H);
        presetList = new COptionMenu(presetRect, this, tagPresetList);
        setColors(presetList);

        presetList->addEntry(new CMenuItem("Select preset", 1 << 1));

        auto presetMgr = synth()->getPresetManager();
        for (int i = 0; i < presetMgr->presetNames.size(); i++)
        {
            presetList->addEntry(new CMenuItem(presetMgr->presetNames[i].c_str()));
        }

        xframe->addView(presetList);

        GRID_RECT(presetActRect, 9.5, 8.5, 2 * GRID_SIZE, TEXT_H);
        presetActionList = new COptionMenu(presetActRect, this, tagPresetActionList);
        setColors(presetActionList);
        presetActionList->addEntry(new CMenuItem("Preset actions...", 1 << 1));
        presetActionList->addEntry(new CMenuItem("Save preset"));
        presetActionList->addEntry(new CMenuItem("Save preset as new"));
        xframe->addView(presetActionList);

        ADD_TEXT("(c) 2017-2021 Joonas Salonpaa", 14, 8.5, 4 * GRID_SIZE, TEXT_H, label->setHoriAlign(kRightText));
        ADD_TEXT("github.com/biocommando/TranSynth", 14, 8.75, 4 * GRID_SIZE, TEXT_H, label->setHoriAlign(kRightText));

        auto logoBmp = loadBitmap("TranSynLogo.bmp");

        CRect logoRect(wSize->right / 2 - 234, 5, wSize->right + 234, 50);
        auto logo = new CView(logoRect);
        logo->setBackground(logoBmp);
        xframe->addView(logo);

        knobBackground->forget();
        logoBmp->forget();

        frame = xframe;

        return true;
    }
    void close()
    {
        auto xframe = frame;
        frame = 0;
        delete xframe;
    }

    void setParameter(int id, float value)
    {
        if (!frame)
            return;
        for (int i = knobs.size() - 1; i >= 0; i--)
        {
            if (knobs[i]->paramId == id)
            {
                knobs[i]->setValue(value);
                return;
            }
        }
    }

    void valueChanged(CControl *control)
    {
        if (!frame)
            return;
        auto tag = control->getTag();

        if (tag == tagLinkModeList)
        {
            setLinkModeFromListIdx();
        }
        else if (tag == tagPresetList)
        {
            auto presetMgr = synth()->getPresetManager();
            presetNumber = presetList->getCurrentIndex() - 1;
            currentPresetName = presetMgr->readProgram(presetNumber);
            currentPresetNameEdit->setText(currentPresetName.c_str());
            presetList->setCurrent(0);
            for (int i = 0; i < knobs.size(); i++)
            {
                knobs[i]->setValue(synth()->getParameterById(knobs[i]->paramId));
            }
        }
        else if (tag == tagPresetActionList)
        {
            auto presetMgr = synth()->getPresetManager();
            const auto action = presetActionList->getCurrentIndex();
            presetActionList->setCurrent(0);
            if (action == 1 && presetNumber >= 0) // save / replace
            {
                presetMgr->saveProgram(presetNumber, currentPresetName);
            }
            else // save as new
            {
                presetMgr->refresh();
                presetMgr->saveProgram(presetMgr->presetNames.size(), currentPresetName);
                presetNumber = presetMgr->presetNames.size();

                presetList->addEntry(new CMenuItem(currentPresetName.c_str()));
            }
        }
        else if (tag == tagGroupedParam || tag == tagGlobalParam)
        {
            auto knob = (Knob *)control;
            //const auto param = parameterList->getCurrentIndex() - 1;
            const auto idx = synth()->getIndexById(knob->paramId);
            synth()->setParameterAutomated(idx, knob->getValue());
            knob->setLabel(knob->getValue());
            if (tag == tagGroupedParam && linkMode[knob->group])
            {
                for (int i = 0; i < 4; i++)
                {
                    if (i != knob->group && linkMode[i] == linkMode[knob->group])
                    {
                        const auto id = createId(i, knob->param);
                        setParameter(id, knob->getValue());
                        const auto otherIdx = synth()->getIndexById(id);
                        synth()->setParameterAutomated(otherIdx, knob->getValue());
                    }
                }
            }
        }
        else if (tag == tagPresetNameEdit)
        {
            char name[256];
            currentPresetNameEdit->getText(name);
            currentPresetName.assign(name);
        }
    }
};
