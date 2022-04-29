/*
  ==============================================================================

    AdsrControlsComponent.h
    Created: 12 Apr 2022 3:05:31am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AdsrControlsComponent : public juce::Component
{
public:
    AdsrControlsComponent(juce::AudioProcessorValueTreeState& apvts);
    ~AdsrControlsComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void setSliderParams(juce::Slider& slider);
    void setLabelParams(juce::Label& label, juce::String str);

    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;

    // Labels
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;

    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    // unique_ptr frees up memory when the plug-in UI is closed
    std::unique_ptr<SliderAttachment> attackAttachment;
    std::unique_ptr<SliderAttachment> decayAttachment;
    std::unique_ptr<SliderAttachment> sustainAttachment;
    std::unique_ptr<SliderAttachment> releaseAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrControlsComponent)
};
