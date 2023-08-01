[![](https://github.com/cwoodall/wahmbulance/workflows/Pamplejuce/badge.svg)](https://github.com/cwoodall/wahmbulance/actions)

Wahmbulance is an **autowah** plugin based on the [Pamplejuce template]() and JUCE 7.x

It was developed with:

1. C++20
2. JUCE 7.x as a submodule tracking develop
3. CMP as a submodule. This is used for drawing interactive plots using JUCE 
4. CMake 3.21 for building cross-platform
5. [Catch2](https://github.com/catchorg/Catch2) v3.1.0 as the test framework and runner
6. [pluginval](http://github.com/tracktion/pluginval) 1.x for plugin validation  
7. GitHub Actions config for building binaries, running Catch2 tests and pluginval,artifact building on the Windows, Linux and macOS platforms, including code signing and notarization on macOS and Windows EV/OV code signing via Azure Key Vault

This was adapted from a python implementation of the effect that I worked on [https://github.com/cwoodall/py-audio-effects-autowah/]
## Inspiration

Originally the Wahmbulance plug-in was based 

## Implementation

### Core algorithm

### UI/UX Design

