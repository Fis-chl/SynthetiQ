/*
  ==============================================================================

    AdsrComponent.cpp
    Created: 25 Feb 2022 9:03:27pm
    Author:  lukeb_000

  ==============================================================================
*/

#include <JuceHeader.h>
#include "AdsrComponent.h"

//==============================================================================
AdsrComponent::AdsrComponent(juce::AudioProcessorValueTreeState& apvts, float& attack, float& decay, float& sustain, float& release) :
    graphComponent(attack, decay, sustain, release), controlsComponent(apvts)
{
    addAndMakeVisible(graphComponent);
    addAndMakeVisible(controlsComponent);
}

AdsrComponent::~AdsrComponent()
{
}

void AdsrComponent::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);

    g.setColour(juce::Colours::white);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
}

void AdsrComponent::resized()
{
    const auto bounds = getLocalBounds().reduced(10);
    const auto padding = 10;
    const auto sliderWidth = bounds.getWidth() / 4 - padding;
    const auto sliderHeight = bounds.getHeight() - padding;
    const auto sliderStartX = 10;
    const auto sliderStartY = 10;

    graphComponent.setBounds(0, 0, getWidth(), getHeight() - 90);
    controlsComponent.setBounds(0, graphComponent.getY() + graphComponent.getHeight() + 10, getWidth(), getHeight() - graphComponent.getHeight() - 10);
}
