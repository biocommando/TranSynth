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
        label->setBackColor(cBg);             \
        label->setFrameColor(cBg);            \
        label->setFontColor(cFg);             \
        xframe->addView(label);               \
        aexpr;                                \
    } while (0)

constexpr int tagLinkModeList = 101;
constexpr int tagPresetList = 102;
constexpr int tagPresetActionList = 103;
constexpr int tagGroupedParam = 201;
constexpr int tagGlobalParam = 202;

extern void logf(const char *, float);

extern char workDir[1024];
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
            const char waveTableNames[][16] = {
                "SIN", "TRI", "SQR", "SAW",
                "FM-1", "FM-2", "FM-3", "FM-4",
                "Adt-1", "Adt-2", "Adt-3", "Adt-4",
                "sinsw", "sawsw", "sqrsw", "PWM"};
            label->setText(waveTableNames[v]);
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
            setLinkMode(1, 0, 0, 1);
            break;
        case 10:
            setLinkMode(0, 1, 0, 1);
            break;
        }
    }

    CBitmap *loadBitmap(const char *relativePath)
    {
        resolveWorkDir();
        std::string s = std::string(workDir) + "\\" + relativePath;
        std::wstring ws(s.size(), L'#');
        mbstowcs(&ws[0], s.c_str(), s.size());
        auto bmp = Gdiplus::Bitmap::FromFile(ws.c_str(), false);
        auto cbmp = new CBitmap(bmp);
        delete bmp;
        return cbmp;
    }

    Knob *addKnob(CFrame *xframe, int x, int y, int id, int tag)
    {
        const CColor cBg = kBlackCColor, cFg = kWhiteCColor;
        GRID_RECT(knobRect, x, y, KNOB_SIZE, KNOB_SIZE);
        auto knob = new Knob(knobRect, this, tag, knobBackground, nullptr, id);
        ADD_TEXT("00", x, y + 0.7, KNOB_SIZE, TEXT_H, knob->label = label);
        xframe->addView(knob);
        knob->setValue(synth()->getParameterById(id));
        knobs.push_back(knob);
        return knob;
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
        CColor cBg = kBlackCColor, cFg = kWhiteCColor;
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
        linkModeList->setBackColor(cBg);
        linkModeList->setFrameColor(cFg);
        linkModeList->setFontColor(cFg);

        linkModeList->addEntry(new CMenuItem("Link mode...", 1 << 1));
        linkModeList->addEntry(new CMenuItem("No link"));
        linkModeList->addEntry(new CMenuItem("A-D"));
        linkModeList->addEntry(new CMenuItem("A-D-S"));
        linkModeList->addEntry(new CMenuItem("A-D-S-R"));
        linkModeList->addEntry(new CMenuItem("D-S"));
        linkModeList->addEntry(new CMenuItem("D-S-R"));
        linkModeList->addEntry(new CMenuItem("S-R"));
        linkModeList->addEntry(new CMenuItem("A-S"));
        linkModeList->addEntry(new CMenuItem("A-S-R"));
        linkModeList->addEntry(new CMenuItem("A-R"));
        linkModeList->addEntry(new CMenuItem("D-R"));

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
                /*GRID_RECT(knobRect, 2 + j + xOffset, i + 1 + yOffset, KNOB_SIZE, KNOB_SIZE);
                auto knob = new Knob(knobRect, this, 100 + i * 5 + j, knobBackground, nullptr, id);
                ADD_TEXT("00", 2 + j + xOffset, i + 1.7 + yOffset, KNOB_SIZE, 16, knob->label = label);
                xframe->addView(knob);
                knob->setValue(synth()->getParameterById(id));
                knob->group = j;
                knob->param = i;
                knobs.push_back(knob);*/
            }
        }

        ADD_TEXT("G L O B A L  P A R A M E T E R S", 12, 0.5, GRID_X(6), TEXT_H, );

        for (int i = 0; i < 9; i++)
        {
            int xOffset = i > 6 ? 3 : 0;
            int yOffset = i > 6 ? -7 : 0;
            const auto id = createId(i);
            char buf[32];
            synth()->getParameterLongName(id, buf);
            ADD_TEXT(buf, 12 + xOffset, i + 1.25 + yOffset, GRID_X(2), TEXT_H, label->setHoriAlign(kLeftText));
            auto knob = addKnob(xframe, 14 + xOffset, i + 1 + yOffset, id, tagGlobalParam);
        }

        ADD_TEXT("Preset", 6, 8.5, GRID_SIZE * 1, TEXT_H, label->setHoriAlign(kLeftText));
        GRID_RECT(presetRect, 7, 8.5, 2 * GRID_SIZE, TEXT_H);
        presetList = new COptionMenu(presetRect, this, tagPresetList);
        presetList->setBackColor(cBg);
        presetList->setFrameColor(cFg);
        presetList->setFontColor(cFg);

        presetList->addEntry(new CMenuItem("Select preset", 1 << 1));

        auto presetMgr = synth()->getPresetManager();
        for (int i = 0; i < presetMgr->presetNames.size(); i++)
        {
            presetList->addEntry(new CMenuItem(presetMgr->presetNames[i].c_str()));
        }

        xframe->addView(presetList);

        GRID_RECT(presetActRect, 9.5, 8.5, 2 * GRID_SIZE, TEXT_H);
        presetActionList = new COptionMenu(presetActRect, this, tagPresetActionList);
        presetActionList->setBackColor(cBg);
        presetActionList->setFrameColor(cFg);
        presetActionList->setFontColor(cFg);
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
        //const auto id = synth()->getIdByIndex(index);
        for (int i = knobs.size() - 1; i >= 0; i--)
        {
            if (knobs[i]->paramId == id)
            {
                knobs[i]->setValue(value);
                return;
            }
        }
        //parameterList->setCurrent(0);
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
        /*if (tag == tagParamSelect)
        {
            const auto param = parameterList->getCurrentIndex() - 1;
            for (int i = 0; i < 4; i++)
            {
                dynamicKnobs[i]->setValue(
                    synth()->getParameterById(createId(i, param)));
            }
        }*/
        else if (tag == tagPresetList)
        {
            auto presetMgr = synth()->getPresetManager();
            presetNumber = presetList->getCurrentIndex() - 1;
            presetMgr->readProgram(presetNumber);
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
                presetMgr->saveProgram(presetNumber, presetMgr->presetNames[presetNumber]);
            }
            else // save as new
            {
                const auto name = std::string("User program ") + std::to_string(presetMgr->presetNames.size());
                presetMgr->saveProgram(presetMgr->presetNames.size(), name);
                presetNumber = presetMgr->presetNames.size();
                
                presetList->addEntry(new CMenuItem(name.c_str()));
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
                    if (i != knob->group && linkMode[i])
                    {
                        const auto id = createId(i, knob->param);
                        setParameter(id, knob->getValue());
                        const auto otherIdx = synth()->getIndexById(id);
                        synth()->setParameterAutomated(otherIdx, knob->getValue());
                    }
                }
            }
        }
    }
};