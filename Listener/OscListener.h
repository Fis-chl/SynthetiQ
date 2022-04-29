/*
  ==============================================================================

    OscListener.h
    Created: 8 Mar 2022 4:13:31am
    Author:  lukeb_000

  ==============================================================================
*/

#include <JuceHeader.h>

#pragma once

class OscListener : juce::AudioProcessorValueTreeState::Listener {
public:
    OscListener();

private:
    void parameterChanged(const juce::String& parameterID, float newValue) override;
};
