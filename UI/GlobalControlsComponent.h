/*
  ==============================================================================

    GlobalControlsComponent.h
    Created: 16 Apr 2022 11:15:03pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class GlobalControlsComponent : public juce::Component
{
public:
    GlobalControlsComponent(juce::AudioProcessorValueTreeState&);
    ~GlobalControlsComponent();

    void resized();
    void paint(juce::Graphics&);

private:

    void setSliderParams(juce::Slider&);
    void setLabelParams(juce::Label&, juce::String);

    juce::Slider detune;
    juce::Slider pitch;
    juce::Slider gain;

    juce::Label detuneLabel;
    juce::Label pitchLabel;
    juce::Label gainLabel;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> detuneAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
};