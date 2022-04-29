/*
  ==============================================================================

    AdsrComponent.h
    Created: 25 Feb 2022 9:03:27pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AdsrGraphComponent.h"
#include "AdsrControlsComponent.h"

//==============================================================================
/*
*/
class AdsrComponent  : public juce::Component
{
public:
    AdsrComponent(juce::AudioProcessorValueTreeState& apvts, float& attack, float& decay, float& sustain, float& release);
    ~AdsrComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    AdsrGraphComponent graphComponent;
    AdsrControlsComponent controlsComponent;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdsrComponent)
};
