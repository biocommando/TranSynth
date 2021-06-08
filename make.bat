@echo off
g++ -c AdsrEnvelope.cpp -O3
g++ -c ADSRUpdater.cpp -O3
g++ -c BasicOscillator.cpp -O3
g++ -c CallbackUpdatable.cpp -O3
g++ -c EnvelopeStage.cpp -O3
g++ -c MS20Filter.cpp -O3
g++ -c ParameterHolder.cpp -O3
g++ -c PluginParameter.cpp -O3
g++ -c Stdafx.cpp -O3
g++ -c SubSynth.cpp -O3
g++ -c SubSynthParam.cpp -O3
g++ -c SubSynthParams.cpp -O3
g++ -c SubSynthVoiceManagement.cpp -O3
g++ -c VCMgmtEnvUpdater.cpp -O3
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffect.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffect.o -O3
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffectx.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffectx.o -O3
g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\vstplugmain.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstplugmain.o -O3
g++ -c TranSynth.cpp -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -O3
rem g++ -o TranSynth.dll -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" AdsrEnvelope.o ^
rem ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
rem PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o
dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o ADSRUpdater.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o CallbackUpdatable.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o Stdafx.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o VCMgmtEnvUpdater.o TranSynth.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -luuid -luser32 -mwindows --no-export-all-symbols --def TranSynth.def ^
-o TransitionSynth.dll

xcopy TransitionSynth.dll "C:\Program Files (x86)\VstPlugins\TransitionSynth.dll" /Y