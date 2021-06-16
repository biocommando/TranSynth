rem @echo off
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
rem g++ -c VCMgmtEnvUpdater.cpp -Ofast
rem g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffect.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffect.o -Ofast
rem g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\audioeffectx.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o audioeffectx.o -Ofast
rem g++ -c "D:\VST3 SDK\public.sdk\source\vst2.x\vstplugmain.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstplugmain.o -Ofast
rem g++ -c "D:\VST3 SDK\vstgui.sf\vstgui\aeffguieditor.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o aeffguieditor.o
rem g++ -c "D:\VST3 SDK\vstgui.sf\vstgui\vstgui.cpp" -DWIN32 -trigraphs -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstgui.o
rem g++ -c "D:\VST3 SDK\vstgui.sf\vstgui\vstcontrols.cpp" -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -o vstcontrols.o
g++ -c TranSynthGui.cpp -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\vstgui.sf\vstgui" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x"
g++ -c TranSynth.cpp -DWIN32 -I"D:\VST3 SDK" -I"D:\VST3 SDK\vstgui.sf\vstgui" -I"D:\VST3 SDK\plugininterfaces\source\vst2.x" -I"D:\VST3 SDK\public.sdk\source\vst2.x" -Ofast

dllwrap  --output-def libTranSynth.def  --driver-name c++ ^
AdsrEnvelope.o audioeffect.o audioeffectx.o vstplugmain.o BasicOscillator.o EnvelopeStage.o MS20Filter.o ParameterHolder.o ^
PluginParameter.o SubSynth.o SubSynthParam.o SubSynthParams.o SubSynthVoiceManagement.o TranSynth.o HonestEarRapeFilter.o VAStateVariableFilter.o ^
aeffguieditor.o TranSynthGui.o vstgui.o vstcontrols.o ^
-L. --add-stdcall-alias -lole32 -lkernel32 -lgdi32 -lgdiplus -luuid -luser32 -lshell32 -mwindows --no-export-all-symbols --def TranSynth.def ^
-o TransitionSynth.dll -Ofast

xcopy TransitionSynth.dll "C:\Program Files (x86)\VstPlugins\TransitionSynth.dll" /Y