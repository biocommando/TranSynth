rem @echo off
echo #pragma once > build.h
echo #define BUILD_DATE "%date:* =% %time:,=.%" >> build.h
g++ -c AdsrEnvelope.cpp -Ofast
g++ -c BasicOscillator.cpp -Ofast
g++ -c EnvelopeStage.cpp -Ofast
g++ -c MS20Filter.cpp -Ofast
g++ -c ParameterHolder.cpp -Ofast
g++ -c PluginParameter.cpp -Ofast
g++ -c SubSynth.cpp -Ofast
g++ -c SubSynthParam.cpp -Ofast
g++ -c SubSynthParams.cpp -Ofast
g++ -c SubSynthVoiceManagement.cpp -Ofast
g++ -c HonestEarRapeFilter.cpp -Ofast
g++ -c VAStateVariableFilter.cpp -Ofast

set flags=-DVST_COMPAT -DWIN32 -DWINDOWS -DUNICODE -DWINDOWS_GUI
g++ -c fst-extension/VSTGui/*.cpp -I"fst-extension/src/" %flags% -trigraphs
g++ -c fst-extension/src/FstAudioEffect.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags%

g++ -c ScriptCaller.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast
g++ -c TranSynthGui.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler"
g++ -c TranSynth.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast

dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o FstAudioEffect.o BasicOscillator.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o TranSynth.o HonestEarRapeFilter.o VAStateVariableFilter.o ^
aeffguieditor.o TranSynthGui.o vstgui.o vstcontrols.o ScriptCaller.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -lgdiplus -luuid -luser32 -lshell32 -mwindows --no-export-all-symbols --def TranSynth.def ^
-o TransitionSynth.dll -Ofast

xcopy TransitionSynth.dll "C:\Program Files (x86)\VstPlugins\TransitionSynth.dll" /Y