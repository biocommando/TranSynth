rem @echo off
echo #pragma once > build.h
echo #define BUILD_DATE "%date:* =% %time:,=.%" >> build.h
g++ -c AdsrEnvelope.cpp BasicOscillator.cpp EnvelopeStage.cpp MS20Filter.cpp ^
    ParameterHolder.cpp  PluginParameter.cpp SubSynth.cpp SubSynthParam.cpp ^
    SubSynthParams.cpp SubSynthVoiceManagement.cpp HonestEarRapeFilter.cpp ^
    VAStateVariableFilter.cpp -Ofast || exit /b

set flags=-DVST_COMPAT -DWIN32 -DWINDOWS -DUNICODE -DWINDOWS_GUI
g++ -c fst-extension/VSTGui/*.cpp -I"fst-extension/src/" %flags% -trigraphs
g++ -c fst-extension/src/FstAudioEffect.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags%

g++ -c ScriptCaller.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast || exit /b
g++ -c TranSynthGui.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" || exit /b
g++ -c TranSynth.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast || exit /b

dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o FstAudioEffect.o BasicOscillator.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o TranSynth.o HonestEarRapeFilter.o VAStateVariableFilter.o ^
aeffguieditor.o TranSynthGui.o vstgui.o vstcontrols.o ScriptCaller.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -lgdiplus -luuid -luser32 -lshell32 -mwindows --no-export-all-symbols --def TranSynth.def ^
-o TransitionSynth.dll -Ofast || exit /b

xcopy TransitionSynth.dll "C:\Program Files (x86)\VstPlugins\TransitionSynth.dll" /Y || exit /b