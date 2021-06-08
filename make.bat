@echo off
g++ -c AdsrEnvelope.cpp
g++ -c ADSRUpdater.cpp
g++ -c BasicOscillator.cpp
g++ -c CallbackUpdatable.cpp
g++ -c EnvelopeStage.cpp
g++ -c MS20Filter.cpp
g++ -c ParameterHolder.cpp
g++ -c PluginParameter.cpp
g++ -c Stdafx.cpp
g++ -c SubSynth.cpp
g++ -c SubSynthParam.cpp
g++ -c SubSynthParams.cpp
g++ -c SubSynthVoiceManagement.cpp
g++ -c VCMgmtEnvUpdater.cpp
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffect.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffect.o
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffectx.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffectx.o
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\vstplugmain.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstplugmain.o
g++ -c TranSynth.cpp -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x"
rem g++ -o TranSynth.dll -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" AdsrEnvelope.o ^
rem ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
rem PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o
dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -luuid -luser32 -mwindows --no-export-all-symbols --def TranSynth.def -o TranSynth.dll