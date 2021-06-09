@echo off
g++ -c AdsrEnvelope.cpp -Ofast
g++ -c ADSRUpdater.cpp -Ofast
g++ -c BasicOscillator.cpp -Ofast
g++ -c CallbackUpdatable.cpp -Ofast
g++ -c EnvelopeStage.cpp -Ofast
g++ -c MS20Filter.cpp -Ofast
g++ -c ParameterHolder.cpp -Ofast
g++ -c PluginParameter.cpp -Ofast
g++ -c Stdafx.cpp -Ofast
g++ -c SubSynth.cpp -Ofast
g++ -c SubSynthParam.cpp -Ofast
g++ -c SubSynthParams.cpp -Ofast
g++ -c SubSynthVoiceManagement.cpp -Ofast
g++ -c VCMgmtEnvUpdater.cpp -Ofast
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffect.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffect.o -Ofast
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffectx.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffectx.o -Ofast
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\vstplugmain.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstplugmain.o -Ofast
g++ -c TranSynth.cpp -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -Ofast
rem g++ -o TranSynth.dll -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" AdsrEnvelope.o ^
rem ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
rem PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o
dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -luuid -luser32 -mwindows --no-export-all-symbols --def TranSynth.def ^
-o TransitionSynth.dll -Ofast

xcopy TransitionSynth.dll "C:\Program Files (x86)\VstPlugins\TransitionSynth.dll" /Y