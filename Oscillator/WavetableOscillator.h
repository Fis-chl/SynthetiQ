/*
  ==============================================================================

    WavetableOscillator.h
    Created: 12 Apr 2022 3:42:34pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class WavetableOscillator : public juce::dsp::Oscillator<float>
{
public:
    void initialise();
    void setFrequency();
    void prepare();
    void reset();
    void process();
};
