/*
  ==============================================================================

    LfoComponent.h
    Created: 16 Apr 2022 1:31:22pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LfoComponent : public juce::Component
{
public:
    LfoComponent(juce::AudioProcessorValueTreeState&, int& tableIndex, juce::String lfoId);
    ~LfoComponent();

    void paint(juce::Graphics& g);
    void resized();

    class LfoGraphComponent : public juce::Component, public juce::Timer
    {
    public:
        LfoGraphComponent(int& tableIndex);
        ~LfoGraphComponent();

        void paint(juce::Graphics& g);
        void resized();

    private:
        void timerCallback() override;
        void drawGraph(juce::Graphics&);
        void plot(juce::Rectangle<int>&, juce::Graphics&, juce::Colour);
        void setFramePerSecond(int framePerSecond)
        {
            jassert(framePerSecond > 0 && framePerSecond < 1000);
            startTimerHz(framePerSecond);
        }

        float sin(float x) {
            // Sin wave
            return std::sin(x);
        }

        float saw(float x) {
            x = x + 0.5f;
            float cotx = std::sin(juce::MathConstants<float>::pi * x) / std::cos(juce::MathConstants<float>::pi * x);
            float val = (-2 / juce::MathConstants<float>::pi) * std::atan(cotx);
            return val;
        }

        float sqr(float x) {
            if (std::sin(x) < 0) { return -1.0f; }
            return 1.0f;
        }
       
        float tri(float x)
        {
            float sinx = std::sin(juce::MathConstants<float>::pi * x / 2);
            float arcsinx = std::asin(sinx);
            return (2 / juce::MathConstants<float>::pi) * arcsinx;
        }

        int& tableIndex;
    };

    void setSliderParams(juce::Slider&);

    LfoGraphComponent graphComponent;

    juce::Slider waveSelector;
    juce::TextButton onButton { TRANS("A") };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> waveAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> onAttachment;

    juce::ComboBox parameterSelect;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> paramAttachment;
    juce::Slider paramAmount;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment;
    juce::Slider frequency;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> frequencyAttachment;

    int& tableIndex;
    juce::String lfoId;

private:

};
