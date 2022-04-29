/*
  ==============================================================================

    OscComponent.cpp
    Created: 25 Feb 2022 10:10:57pm
    Author:  lukeb_000

  ==============================================================================
*/

#include <JuceHeader.h>
#include "OscComponent.h"

//==============================================================================
OscComponent::OscComponent(juce::AudioProcessorValueTreeState& apvts, juce::String oscId, int& tableIndex)
    : wavetable(tableIndex), oscControls(apvts, oscId)
{
    setLabelParams(oscToggleLabel, "On/Off");
    addAndMakeVisible(wavetable);
    addAndMakeVisible(oscControls);

    oscToggle.setClickingTogglesState(true);
    oscToggle.setColour(juce::TextButton::buttonOnColourId, juce::Colours::cyan);
    oscToggle.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    oscToggleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, TRANS(juce::String(oscId) + "ENABLED"), oscToggle);
    addAndMakeVisible(oscToggle);
    oscToggle.setTooltip(TRANS("Activate or deactivate this oscillator"));
}

OscComponent::~OscComponent()
{
}

void OscComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    //g.setColour (juce::Colours::white);
    //g.drawRoundedRectangle (0.0f, 0.0f, float(getWidth()), float(getHeight()), 10.0f, 1.0f);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
}

void OscComponent::resized()
{
    wavetable.setBounds(0, 0, getWidth(), getHeight() - 100);
    oscControls.setBounds(0, wavetable.getY() + wavetable.getHeight() + 10, getWidth(), getHeight() - wavetable.getHeight() - 10);

    // Oscillator toggle
    oscToggle.setBounds(wavetable.getX() + 5, wavetable.getY() + 5, 20, 20);
    //oscToggleLabel.setBounds(oscToggle.getX(), oscToggle.getY() + 40, 60, 30);
}

void OscComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, true, 80, 30);
    slider.setColour(juce::Slider::trackColourId, juce::Colours::aliceblue);
    addAndMakeVisible(slider);
}

void OscComponent::setLabelParams(juce::Label& label, juce::String str)
{
    label.setFont(juce::Font(16.0f, juce::Font::bold));
    label.setText(str, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}
