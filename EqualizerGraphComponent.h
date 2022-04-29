/*
  ==============================================================================

    EqualizerGraphComponent.h
    Created: 14 Apr 2022 5:58:12pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class EqualizerGraphComponent : public juce::Component, public juce::Timer
{
public:
    EqualizerGraphComponent(New_synthAudioProcessor&);
    ~EqualizerGraphComponent();

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void drawGraphLog(juce::Graphics&, int, juce::Rectangle<float>, bool, juce::Colour, juce::Colour);
    void drawFrequencyMagnitudes(juce::Graphics&, juce::Colour, int, float);
    void drawEqBands(juce::Graphics&, int, float, float);

    void timerCallback() override;

    New_synthAudioProcessor& processor;
};