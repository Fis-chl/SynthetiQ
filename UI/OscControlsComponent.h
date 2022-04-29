/*
  ==============================================================================

    OscControlsComponent.h
    Created: 12 Apr 2022 12:58:34am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscControlsComponent : public juce::Component
{
public:
    OscControlsComponent(juce::AudioProcessorValueTreeState& apvts, juce::String oscId);
    ~OscControlsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    void setSliderParams(juce::Slider& slider);
    void setLabelParams(juce::Label& label, juce::String str);
    juce::Slider oscWaveSelector;
    juce::Label oscWaveLabel;
    juce::Slider oscGain;
    juce::Label oscGainLabel;
    juce::Slider oscDetune;
    juce::Label oscDetuneLabel;
    juce::Slider oscPitch;
    juce::Label oscPitchLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscWaveSelectorAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscDetuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscPitchAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(OscControlsComponent)
};