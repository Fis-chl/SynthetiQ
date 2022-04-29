/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
New_synthAudioProcessorEditor::New_synthAudioProcessorEditor(New_synthAudioProcessor& p)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , osc1(audioProcessor.apvts, "OSC1", p.getTable1Index())
    , osc2(audioProcessor.apvts, "OSC2", p.getTable2Index())
    , adsr(audioProcessor.apvts, p.getAttack(), p.getDecay(), p.getSustain(), p.getRelease())
    , scopeComponent(p.getAudioBufferQueue())
    , equalizer(audioProcessor.apvts, audioProcessor)
    , lfo1Component(audioProcessor.apvts, audioProcessor.lfo1.waveType, "LFO1")
    , lfo2Component(audioProcessor.apvts, audioProcessor.lfo2.waveType, "LFO2")
    , globalControls(audioProcessor.apvts)
{
    setSize (1000, 800);
    addAndMakeVisible(osc1);
    addAndMakeVisible(osc2);
    addAndMakeVisible(adsr);
    addAndMakeVisible(equalizer);
    addAndMakeVisible(scopeComponent);

    addAndMakeVisible(lfo1Component);
    addAndMakeVisible(lfo2Component);

    addAndMakeVisible(globalControls);


    // Gain slider

    //gainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "MASTERGAIN", gainSlider);
    //gainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    //gainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 80, 30);
    //gainSlider.setColour(juce::Slider::trackColourId, juce::Colours::aliceblue);
    //addAndMakeVisible(gainSlider);
    
    //setLabelProperties(osc1Label, "Oscillator 1");
    //setLabelProperties(osc2Label, "Oscillator 2");
    //setLabelProperties(env1Label, "Envelope 1");
    //setLabelProperties(gainLabel, "Gain");

#ifdef JUCE_OPENGL
    openGLContext.attachTo(*getTopLevelComponent());
#endif
}

New_synthAudioProcessorEditor::~New_synthAudioProcessorEditor()
{
}

//==============================================================================
void New_synthAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);
}

void New_synthAudioProcessorEditor::resized()
{
    // Oscillator Positions
    osc1.setBounds(10, 10, getWidth() / 2 - 15, getHeight() / 3 - 70);
    osc2.setBounds(getWidth() / 2 + 5, 10, osc1.getWidth(), osc1.getHeight());

    // Oscilloscope Position
    scopeComponent.setBounds(10, getHeight() - 90, (2 * getWidth() / 3) - 40, 80);

    // Equalizer Position
    equalizer.setBounds(10, osc1.getY() + osc1.getHeight() + 10, (2 * getWidth() / 3) - 40, getHeight() - osc1.getHeight() - scopeComponent.getHeight() - 40);
    DBG(equalizer.getWidth());

    // Envelope Position
    adsr.setBounds(equalizer.getRight() + 10, equalizer.getY(), getWidth() - 30 - equalizer.getWidth(), (equalizer.getHeight() / 2) - 40);

    lfo1Component.setBounds(equalizer.getRight() + 10, adsr.getBottom() + 10, getWidth() - 30 - equalizer.getWidth(), (equalizer.getHeight() / 4) + 10);
    lfo2Component.setBounds(equalizer.getRight() + 10, lfo1Component.getBottom() + 10, lfo1Component.getWidth(), lfo1Component.getHeight());

    globalControls.setBounds(lfo1Component.getX(), scopeComponent.getY(), lfo1Component.getWidth(), scopeComponent.getHeight());

}

void New_synthAudioProcessorEditor::setLabelProperties(juce::Label& label, juce::String str)
{
    label.setFont(juce::Font(20.0f, juce::Font::bold));
    label.setText(str, juce::dontSendNotification);
    label.setColour(juce::Label::textColourId, juce::Colours::white);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}
