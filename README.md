# Transition Synth

![screenshot here](https://raw.githubusercontent.com/biocommando/TranSynth/master/screenshot.gif "Screenshot")

This is a VST 2.4 project with the idea that all the synth parameters are interpolated between 4 stages that follow an ADSR envelope. The heart of the synth is a simple subtractive synthesizer
with parameters that can be easily morphed between different stages. This means that e.g. instead
of waveform selection the synth has a waveform mixer. The synth has also a simple wavetable
oscillator for creating outlandish soundscapes.

![voice architecture](https://raw.githubusercontent.com/biocommando/TranSynth/master/voice-architecture.gif "Voice architecture")

The wavetable oscillator is implemented so that a short wave file (10k samples) is programmatically
generated into the memory. A global parameter for wavetable position and a stage-specific window
parameter are given. The waveform of the oscillator will consist of *window* samples starting from the
wavetable position.

# License

All code in this repository is licensed with MIT license terms. VST SDK code is BSD-3-clause but the code isn't included in this repository.

# Building

It is possible to build the project (with MinGW-W64) using the make.bat file, though it would require
editing of the include paths and uncommenting some lines. Maybe a proper build tool could be taken into
use at some point.