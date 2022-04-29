/*
  ==============================================================================

    AdsrGraphComponent.h
    Created: 12 Apr 2022 3:05:39am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class AdsrGraphComponent : public juce::Component, public juce::Timer
{
public:
    AdsrGraphComponent(float& attack, float& decay, float& sustain, float& release);
    ~AdsrGraphComponent() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawGraph(juce::Graphics&, juce::Rectangle<int> drawArea);
    void plot(juce::Rectangle<int>&, juce::Graphics&);
    void setFramePerSecond(int framePerSecond)
    {
        jassert(framePerSecond > 0 && framePerSecond < 1000);
        startTimerHz(framePerSecond);
    }

    float& attack;
    float& decay;
    float& sustain;
    float& release;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdsrGraphComponent)
};