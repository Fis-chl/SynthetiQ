/*
  ==============================================================================

    AdsrControlsComponent.cpp
    Created: 12 Apr 2022 3:05:31am
    Author:  lukeb_000

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdsrControlsComponent.h"

AdsrControlsComponent::AdsrControlsComponent(juce::AudioProcessorValueTreeState& apvts)
{
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;

    attackAttachment = std::make_unique<SliderAttachment>(apvts, "ATTACK", attackSlider);
    decayAttachment = std::make_unique<SliderAttachment>(apvts, "DECAY", decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment>(apvts, "SUSTAIN", sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(apvts, "RELEASE", releaseSlider);

    setSliderParams(attackSlider);
    setSliderParams(decaySlider);
    setSliderParams(releaseSlider);

    sustainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    sustainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 13);
    sustainSlider.setTextValueSuffix("%");
    addAndMakeVisible(sustainSlider);

    setLabelParams(attackLabel, "Attack");
    setLabelParams(decayLabel, "Decay");
    setLabelParams(sustainLabel, "Sustain");
    setLabelParams(releaseLabel, "Release");

    attackSlider.setTooltip(TRANS("Envelope attack"));
    decaySlider.setTooltip(TRANS("Envelope decay"));
    sustainSlider.setTooltip(TRANS("Envelope sustain"));
    releaseSlider.setTooltip(TRANS("Envelope release"));
}

AdsrControlsComponent::~AdsrControlsComponent()
{
}

void AdsrControlsComponent::paint(juce::Graphics& g)
{
    float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
    juce::Colour panelColour = juce::Colour(36, 36, 36);
    g.setColour(panelColour);
    g.fillRoundedRectangle(x, y, width, height, 10.0f);

    g.setColour(juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
}

void AdsrControlsComponent::resized()
{
    float paddedSize = getWidth() - 20;
    int vertPos = (getHeight() / 2) - 35;
    attackSlider.setBounds(10, vertPos, 60, 60);
    attackLabel.setBounds(attackSlider.getX(), attackSlider.getY() + 52, 60, 30);
    decaySlider.setBounds((paddedSize / 3) - 10, vertPos, 60, 60);
    decayLabel.setBounds(decaySlider.getX(), decaySlider.getY() + 52, 60, 30);
    sustainSlider.setBounds(((paddedSize * 2) / 3) - 30, vertPos, 60, 60);
    sustainLabel.setBounds(sustainSlider.getX(), sustainSlider.getY() + 52, 60, 30);
    releaseSlider.setBounds(getWidth() - 70, vertPos, 60, 60);
    releaseLabel.setBounds(releaseSlider.getX(), releaseSlider.getY() + 52, 60, 30);

    //attackSlider.setBounds(sliderStartX, sliderStartY, sliderWidth, sliderHeight);
    //decaySlider.setBounds(attackSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);
    //sustainSlider.setBounds(decaySlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);
    //releaseSlider.setBounds(sustainSlider.getRight() + padding, sliderStartY, sliderWidth, sliderHeight);

    //attackLabel.setBounds(sliderStartX, sliderStartY + sliderHeight, sliderWidth, 20);
    //decayLabel.setBounds(attackSlider.getRight() + padding, sliderStartY + sliderHeight, sliderWidth, 20);
    //sustainLabel.setBounds(decaySlider.getRight() + padding, sliderStartY + sliderHeight, sliderWidth, 20);
    //releaseLabel.setBounds(sustainSlider.getRight() + padding, sliderStartY + sliderHeight, sliderWidth, 20);
}

void AdsrControlsComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 13);
    slider.setTextValueSuffix("s");
    addAndMakeVisible(slider);
}

void AdsrControlsComponent::setLabelParams(juce::Label& label, juce::String str)
{
    label.setFont(juce::Font(13.0f, juce::Font::plain));
    label.setText(str, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}