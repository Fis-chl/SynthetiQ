/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "UI/AdsrComponent.h"
#include "UI/OscComponent.h"
#include "UI/WavetableComponent.h"
#include "Visualizer/ScopeComponent.h"
#include "EqualizerComponent.h"
#include "UI/LfoComponent.h"
#include "UI/GlobalControlsComponent.h"

//==============================================================================
/**
*/
class New_synthAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    New_synthAudioProcessorEditor (New_synthAudioProcessor&);
    ~New_synthAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void setLabelProperties(juce::Label& label, juce::String str);
    New_synthAudioProcessor& audioProcessor;
    OscComponent osc1;
    OscComponent osc2;
    AdsrComponent adsr;
    juce::Label osc1Label;
    juce::Label osc2Label;
    juce::Label env1Label;

    // Master gain
    //juce::Slider gainSlider;
    //juce::Label gainLabel;
    //std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;

    ScopeComponent<float> scopeComponent;
    EqualizerComponent equalizer;
    LfoComponent lfo1Component;
    LfoComponent lfo2Component;

    GlobalControlsComponent globalControls;

    juce::OpenGLContext openGLContext;

    juce::SharedResourcePointer<juce::TooltipWindow> tooltipWindow;



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (New_synthAudioProcessorEditor)

};
