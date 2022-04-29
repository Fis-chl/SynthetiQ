/*
  ==============================================================================

    EqualizerComponent.cpp
    Created: 14 Apr 2022 12:53:55am
    Author:  lukeb_000

  ==============================================================================
*/

#include "EqualizerComponent.h"

EqualizerComponent::EqualizerComponent(juce::AudioProcessorValueTreeState& state, New_synthAudioProcessor& p)
    : processor(p), graphComponent(p)
{
    for (size_t i = 0; i < 5; ++i) {
        auto* bandEditor = bandEditors.add(new BandEditor(i, state));
        addAndMakeVisible(bandEditor);
    }

    addAndMakeVisible(graphComponent);

    //startTimerHz(60);
}

EqualizerComponent::~EqualizerComponent()
{
}


void EqualizerComponent::paint(juce::Graphics& g)
{
    int panelNameHeight = 42;
    int localMargin = 2;
    juce::Font panelNameFont = juce::Font(24.0f, juce::Font::plain).withTypefaceStyle("Italic");

    {
        float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
        juce::Colour panelColour = juce::Colour(36, 36, 36);
        g.setColour(panelColour);
        g.fillRoundedRectangle(x, y, width, height, 10.0f);
    }

    {
        juce::Rectangle<int> bounds = getLocalBounds();
        juce::String text("Parametric EQ");
        juce::Colour fillColour = juce::Colours::white;
        g.setColour(fillColour);
        g.setFont(panelNameFont);
        g.drawText(text, bounds.removeFromTop(panelNameHeight).reduced(localMargin), juce::Justification::centred, true);
    }
}

void EqualizerComponent::resized()
{
    auto bandSpace = getLocalBounds().removeFromBottom(getHeight() / 2);
    auto width = juce::roundToInt(bandSpace.getWidth()) / (bandEditors.size());
    for (auto* bandEditor : bandEditors)
        bandEditor->setBounds(bandSpace.removeFromLeft(width));
    auto graphSpace = getLocalBounds().removeFromTop(getHeight() / 2);
    graphSpace.reduce(10, 10);
    graphSpace.removeFromTop(42);
    graphComponent.setBounds(graphSpace);
}

EqualizerComponent::BandEditor::BandEditor(size_t i, juce::AudioProcessorValueTreeState& state)
    : index(i),
    apvts(state)
{
    if (auto* choiceParameter = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(TRANS(juce::String(index) + "TYPE"))))
        filterType.addItemList(choiceParameter->choices, 1);

    addAndMakeVisible(filterType);
    boxAttachments.add(new juce::AudioProcessorValueTreeState::ComboBoxAttachment(apvts, TRANS(juce::String(index) + "TYPE"), filterType));

    addAndMakeVisible(frequency);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, TRANS(juce::String(index) + "FREQUENCY"), frequency));
    frequency.setTooltip(TRANS("Filter's frequency"));

    addAndMakeVisible(quality);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, TRANS(juce::String(index) + "RESONANCE"), quality));
    quality.setTooltip(TRANS("Filter's resonance"));

    addAndMakeVisible(gain);
    attachments.add(new juce::AudioProcessorValueTreeState::SliderAttachment(apvts, TRANS(juce::String(index) + "GAIN"), gain));
    gain.setTooltip(TRANS("Filter's gain"));

    activate.setClickingTogglesState(true);
    activate.setColour(juce::TextButton::buttonOnColourId, juce::Colours::cyan);
    activate.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    buttonAttachments.add(new juce::AudioProcessorValueTreeState::ButtonAttachment(apvts, TRANS(juce::String(index) + "ACTIVE"), activate));
    addAndMakeVisible(activate);
    activate.setTooltip(TRANS("Activate or deactivate this filter"));
}

void EqualizerComponent::BandEditor::resized()
{
    auto bounds = getLocalBounds();

    bounds.reduce(10, 20);

    filterType.setBounds(bounds.removeFromTop(20));

    auto freqBounds = bounds.removeFromBottom(bounds.getHeight() * 2 / 3);
    frequency.setBounds(freqBounds.withTop(freqBounds.getY() + 10));

    auto buttons = freqBounds.reduced(5).withHeight(20);
    activate.setBounds(buttons.removeFromRight(20));

    quality.setBounds(bounds.removeFromLeft(bounds.getWidth() / 2));
    gain.setBounds(bounds);
}

void EqualizerComponent::BandEditor::updateControls(New_synthAudioProcessor::FilterType type)
{
    switch (type) {
    case New_synthAudioProcessor::LowPass:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(false);
        break;
    case New_synthAudioProcessor::LowShelf:
        frequency.setEnabled(true); quality.setEnabled(false); gain.setEnabled(true);
        break;
    case New_synthAudioProcessor::BandPass:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(false);
        break;
    case New_synthAudioProcessor::Notch:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(false);
        break;
    case New_synthAudioProcessor::Peak:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(true);
        break;
    case New_synthAudioProcessor::HighShelf:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(true);
        break;
    case New_synthAudioProcessor::HighPass:
        frequency.setEnabled(true); quality.setEnabled(true); gain.setEnabled(false);
        break;
    case New_synthAudioProcessor::NoFilter:
    default:
        frequency.setEnabled(true);
        quality.setEnabled(true);
        gain.setEnabled(true);
        break;
    }
}

void EqualizerComponent::BandEditor::setFrequency(float freq)
{
    frequency.setValue(freq, juce::sendNotification);
}

void EqualizerComponent::BandEditor::setGain(float gainToUse)
{
    gain.setValue(gainToUse, juce::sendNotification);
}

void EqualizerComponent::BandEditor::setType(int type)
{
    filterType.setSelectedId(type + 1, juce::sendNotification);
}

void EqualizerComponent::timerCallback()
{
    //repaint(plotFrame);
}