/*
  ==============================================================================

    EqualizerComponent.h
    Created: 14 Apr 2022 12:53:55am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "EqualizerGraphComponent.h"

class EqualizerComponent : public juce::Component, public juce::Timer
{
public:
    EqualizerComponent(juce::AudioProcessorValueTreeState& state, New_synthAudioProcessor& p);
    ~EqualizerComponent() override;

    //==============================================================================

    void paint(juce::Graphics&) override;
    void resized() override;

private:

    class BandEditor : public juce::Component
    {

    public:
        BandEditor(size_t i, juce::AudioProcessorValueTreeState& state);

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BandEditor)

            juce::AudioProcessorValueTreeState& apvts;

        size_t index;

        void resized() override;

        void updateControls(New_synthAudioProcessor::FilterType type);

        void setFrequency(float frequency);

        void setGain(float gain);

        void setType(int type);

        juce::ComboBox            filterType;
        juce::Slider              frequency{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
        juce::Slider              quality{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
        juce::Slider              gain{ juce::Slider::Rotary, juce::Slider::TextBoxBelow };
        juce::TextButton          activate{ TRANS("A") };
        juce::OwnedArray<juce::AudioProcessorValueTreeState::ComboBoxAttachment> boxAttachments;
        juce::OwnedArray<juce::AudioProcessorValueTreeState::SliderAttachment> attachments;
        juce::OwnedArray<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachments;
    };

    juce::OwnedArray<BandEditor> bandEditors;

    New_synthAudioProcessor& processor;
    EqualizerGraphComponent graphComponent;

    void timerCallback() override;

    juce::Rectangle<int> plotFrame;

    const int   clickRadius = 4;
    const float maxDB = 24.0f;

    float getPositionForFrequency(float freq)
    {
        return (std::log(freq / 20.0f) / std::log(2.0f)) / 10.0f;
    }

    float getFrequencyForPosition(float pos)
    {
        return 20.0f * std::pow(2.0f, pos * 10.0f);
    }

    float getPositionForGain(float gain, float top, float bottom)
    {
        return juce::jmap(juce::Decibels::gainToDecibels(gain, -maxDB), -maxDB, maxDB, bottom, top);
    }

    float getGainForPosition(float pos, float top, float bottom)
    {
        return juce::Decibels::decibelsToGain(juce::jmap(pos, bottom, top, -maxDB, maxDB), -maxDB);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EqualizerComponent)
};
