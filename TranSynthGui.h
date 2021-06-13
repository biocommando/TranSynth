#pragma once
#include "aeffguieditor.h"
#include "TranSynth.h"

constexpr int tagParamSelect = 1;

extern void logf(const char *, float);

extern char workDir[1024];
extern void resolveWorkDir();

class TranSynthGui : public AEffGUIEditor, public CControlListener
{
    COptionMenu *parameterList = nullptr;
    CKnob *dynamicKnobs[4];

    TranSynth *synth()
    {
        return (TranSynth *)effect;
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

public:
    TranSynthGui(void *ptr) : AEffGUIEditor(ptr) { logf("create gui", 0); }
    ~TranSynthGui()
    {
    }

    bool open(void *ptr)
    {
        logf("open gui", 0);
        CRect frameSize(0, 0, 600, 600);
        CColor cBg = kBlackCColor, cFg = kWhiteCColor;
        ERect *wSize;
        getRect(&wSize);

        wSize->top = wSize->left = 0;
        wSize->bottom = (VstInt16)frameSize.bottom;
        wSize->right = (VstInt16)frameSize.right;

        auto xframe = new CFrame(frameSize, ptr, this);

        auto knobBackground = loadBitmap("TranSynKnob.bmp");

        xframe->setBackgroundColor(cBg);

        CRect optionRect(10, 10, 200, 40);
        parameterList = new COptionMenu(optionRect, this, tagParamSelect);
        parameterList->setBackColor(cBg);
        parameterList->setFrameColor(cFg);
        parameterList->setFontColor(cFg);

        parameterList->addEntry(new CMenuItem("Select parameter", 1 << 1));
        parameterList->addEntry(new CMenuItem("TRI Amount"));
        parameterList->addEntry(new CMenuItem("SAW Amount"));
        parameterList->addEntry(new CMenuItem("SQR Amount"));
        parameterList->addEntry(new CMenuItem("Detune"));
        parameterList->addEntry(new CMenuItem("Filter Cutoff"));
        parameterList->addEntry(new CMenuItem("Filter Resonance"));
        parameterList->addEntry(new CMenuItem("LFO Rate"));
        parameterList->addEntry(new CMenuItem("LFO to Pitch"));
        parameterList->addEntry(new CMenuItem("LFO to Filter"));
        parameterList->addEntry(new CMenuItem("Volume"));
        parameterList->addEntry(new CMenuItem("Distortion"));
        parameterList->addEntry(new CMenuItem("Wavetable Amount"));
        parameterList->addEntry(new CMenuItem("Wavetable Window"));

        xframe->addView(parameterList);

        for (int i = 0; i < 4; i++)
        {
            CRect knobRect(250 + i * 50, 10, 250 + i * 50 + 40, 50);
            dynamicKnobs[i] = new CKnob(knobRect, this, 100 + i, knobBackground, nullptr, CPoint(0, 0));
            xframe->addView(dynamicKnobs[i]);
        }

        knobBackground->forget();

        frame = xframe;

        return true;
    }
    void close()
    {
        auto xframe = frame;
        frame = 0;
        delete xframe;
    }

    void setParameter(VstInt32 index, float value)
    {
        if (!frame)
            return;
        parameterList->setCurrent(0);
    }

    void valueChanged(CControl *pControl)
    {
        if (!frame)
            return;
        auto tag = pControl->getTag();
        if (tag == tagParamSelect)
        {
            const auto param = parameterList->getCurrentIndex() - 1;
            for (int i = 0; i < 4; i++)
            {
                dynamicKnobs[i]->setValue(
                    synth()->getParameterById(createId(i, param)));
            }
        }
        if (tag >= 100 && tag <= 103)
        {
            const auto param = parameterList->getCurrentIndex() - 1;
            const auto idx = synth()->getIndexById(createId(tag - 100, param));
            synth()->setParameterAutomated(idx,
                                           dynamicKnobs[tag - 100]->getValue());
        }
    }

    void updateAllControls()
    {
    }
};
