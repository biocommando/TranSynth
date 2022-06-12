rem @echo off
echo #pragma once > build.h
echo #define BUILD_DATE "%date:* =% %time:,=.%" >> build.h
echo #define GIT_COMMIT \>> build.h
git log --format=\""%%h\"" -n 1 >> build.h
set cxx=g++ -static -c

set flags=-DVST_COMPAT -DWIN32 -DWINDOWS -DUNICODE -DWINDOWS_GUI -D__x86_64__

%cxx% AdsrEnvelope.cpp BasicOscillator.cpp EnvelopeStage.cpp MS20Filter.cpp ^
    ParameterHolder.cpp  PluginParameter.cpp SubSynth.cpp SubSynthParam.cpp ^
    SubSynthParams.cpp SubSynthVoiceManagement.cpp HonestEarRapeFilter.cpp ^
    VAStateVariableFilter.cpp -Ofast %flags% || exit /b

rem "-DFST_DEBUG_OUTPUT=""C:/users/public/FstLog.txt"""
%cxx% fst-extension/VSTGui/*.cpp -I"fst-extension/src/" %flags% -trigraphs
%cxx% fst-extension/src/FstAudioEffect.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags%

%cxx% ScriptCaller.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast || exit /b
%cxx% TranSynthGui.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" || exit /b
%cxx% TranSynth.cpp -I"fst-extension/VSTGui/" -I"fst-extension/src/" %flags% -I"D:\code\js\SimpleScriptCompiler" -Ofast || exit /b

g++ -shared ^
AdsrEnvelope.o FstAudioEffect.o BasicOscillator.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o TranSynth.o HonestEarRapeFilter.o VAStateVariableFilter.o ^
aeffguieditor.o TranSynthGui.o vstgui.o vstcontrols.o ScriptCaller.o ^
-L. -lole32 -lkernel32 -lgdi32 -lgdiplus -luuid -luser32 -lshell32 --def TranSynth.def ^
-o TransitionSynth.dll -Ofast || exit /b
xcopy TransitionSynth.dll "C:\Program Files\Steinberg\VstPlugins\TransitionSynth.dll" /Y || exit /b