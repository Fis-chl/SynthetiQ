/*
  ==============================================================================

    GlobalControlsComponent.cpp
    Created: 16 Apr 2022 11:15:03pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "GlobalControlsComponent.h"

GlobalControlsComponent::GlobalControlsComponent(juce::AudioProcessorValueTreeState& apvts)
{
    detuneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, TRANS("GLOBALDETUNE"), detune);
    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, TRANS("GLOBALPITCH"), pitch);
    gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, TRANS("GLOBALGAIN"), gain);

    setSliderParams(detune);
    setSliderParams(pitch);
    setSliderParams(gain);

    setLabelParams(detuneLabel, TRANS("Detune"));
    setLabelParams(pitchLabel, TRANS("Pitch"));
    setLabelParams(gainLabel, TRANS("Gain"));
}

GlobalControlsComponent::~GlobalControlsComponent()
{
}

void GlobalControlsComponent::resized()
{
    detune.setBounds(getLocalBounds().getX() + 10, getLocalBounds().getY() + 10, (getLocalBounds().getWidth() / 3) - 20, getLocalBounds().getHeight() - 20);
    pitch.setBounds(detune.getRight() + 10, getLocalBounds().getY() + 10, (getLocalBounds().getWidth() / 3) - 20, getLocalBounds().getHeight() - 20);
    gain.setBounds(pitch.getRight() + 10, getLocalBounds().getY() + 10, (getLocalBounds().getWidth() / 3) - 20, getLocalBounds().getHeight() - 20);

    detune.setTooltip(TRANS("Global detune"));
    pitch.setTooltip(TRANS("Global pitch"));
    gain.setTooltip(TRANS("Global gain"));

    detuneLabel.setBounds(detune.getX(), detune.getY() - 12, detune.getWidth(), 20);
    pitchLabel.setBounds(pitch.getX(), pitch.getY() - 12, pitch.getWidth(), 20);
    gainLabel.setBounds(gain.getX(), gain.getY() - 12, pitch.getWidth(), 20);
}

void GlobalControlsComponent::paint(juce::Graphics& g)
{
    juce::Colour panelColour = juce::Colour(26, 33, 38);
    juce::Colour lineColour = juce::Colour(44, 59, 69);
    //53, 72, 84
    juce::Colour thickLineColour = juce::Colour(69, 91, 107);
    juce::Colour textColour = juce::Colour(120, 162, 191);

    juce::Colour graphColour = juce::Colours::lightcyan;

    g.setColour(juce::Colour(36, 36, 36));
    juce::Rectangle<float> bounds = { float(getLocalBounds().getX()), float(getLocalBounds().getY()), float(getLocalBounds().getWidth()), float(getLocalBounds().getHeight()) };
    //g.fillRoundedRectangle(bounds, 10.0f);

    g.fillRoundedRectangle(bounds, 10.0f);
}

void GlobalControlsComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 13);
    //slider.setTextValueSuffix("s");
    addAndMakeVisible(slider);
}

void GlobalControlsComponent::setLabelParams(juce::Label& label, juce::String str)
{
    label.setFont(juce::Font(13.0f, juce::Font::plain));
    label.setText(str, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}
