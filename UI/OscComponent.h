/*
  ==============================================================================

    OscComponent.h
    Created: 25 Feb 2022 10:10:57pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "WavetableComponent.h"
#include "OscControlsComponent.h"

//==============================================================================
/*
*/
class OscComponent  : public juce::Component
{
public:
    OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::String oscId, int& tableIndex);
    ~OscComponent() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& slider);
    void setLabelParams(juce::Label& label, juce::String str);
    WavetableComponent wavetable;
    //juce::Slider oscWaveSelector;
    //juce::Label oscWaveLabel;
    //juce::Slider oscGain;
    //juce::Label oscGainLabel;
    juce::TextButton oscToggle { TRANS("A") };
    juce::Label oscToggleLabel;
    OscControlsComponent oscControls;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscWaveSelectorAttachment;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> oscToggleAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OscComponent)
};
