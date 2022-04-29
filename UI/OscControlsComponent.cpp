/*
  ==============================================================================

    OscControlsComponent.cpp
    Created: 12 Apr 2022 12:58:34am
    Author:  lukeb_000

  ==============================================================================
*/

#include "OscControlsComponent.h"

OscControlsComponent::OscControlsComponent(juce::AudioProcessorValueTreeState& apvts, juce::String oscId)
{
    setSliderParams(oscWaveSelector);
    setSliderParams(oscGain);
    setSliderParams(oscDetune);
    setSliderParams(oscPitch);
    setLabelParams(oscWaveLabel, "Wave");
    setLabelParams(oscGainLabel, "Level");
    setLabelParams(oscDetuneLabel, "Detune");
    setLabelParams(oscPitchLabel, "Pitch");

    oscWaveSelector.setTooltip(TRANS("Oscillator waveform"));
    oscGain.setTooltip(TRANS("Oscillator gain"));
    oscDetune.setTooltip(TRANS("Oscillator detune"));
    oscPitch.setTooltip(TRANS("Oscillator pitch"));

    oscWaveSelectorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, juce::String((oscId)+"WAVETYPE"), oscWaveSelector);
    oscGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, juce::String((oscId)+"GAIN"), oscGain);
    oscDetuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, juce::String((oscId)+"DETUNE"), oscDetune);
    oscPitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, juce::String((oscId)+"PITCH"), oscPitch);
}

OscControlsComponent::~OscControlsComponent()
{
}

void OscControlsComponent::paint(juce::Graphics& g)
{
    float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
    juce::Colour panelColour = juce::Colour(36, 36, 36);
    g.setColour(panelColour);
    g.fillRoundedRectangle(x, y, width, height, 10.0f);

    g.setColour(juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
}

void OscControlsComponent::resized()
{
    const int size = 70;
    int vertPos = (getHeight() / 2) - 45;
    oscGain.setBounds(getWidth() - 70, vertPos, size, size);
    oscGainLabel.setBounds(oscGain.getX(), oscGain.getY() + 62, size, 30);

    oscWaveSelector.setBounds(10, vertPos, size, size);
    oscWaveLabel.setBounds(oscWaveSelector.getX(), oscWaveSelector.getY() + 62, size, 30);

    oscDetune.setBounds((getWidth() / 3) - 30, vertPos, size, size);
    oscDetuneLabel.setBounds(oscDetune.getX(), oscDetune.getY() + 62, size, 30);

    oscPitch.setBounds((getWidth() * 2 / 3) - 30, vertPos, size, size);
    oscPitchLabel.setBounds(oscPitch.getX(), oscPitch.getY() + 62, size, 30);
}

void OscControlsComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    slider.setColour(juce::Slider::trackColourId, juce::Colours::aliceblue);
    addAndMakeVisible(slider);
}

void OscControlsComponent::setLabelParams(juce::Label& label, juce::String str)
{
    label.setFont(juce::Font(13.0f, juce::Font::plain));
    label.setText(str, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}
