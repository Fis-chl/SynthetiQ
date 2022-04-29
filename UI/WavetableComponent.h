/*
  ==============================================================================

    WavetableComponent.h
    Created: 26 Feb 2022 1:04:28am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include <math.h>

//==============================================================================
/*
*/
class WavetableComponent : public juce::Component, public juce::Timer
{
public:
    WavetableComponent(int& tableIndex);
    ~WavetableComponent() override;

    void setFramePerSecond(int framePerSecond)
    {
        jassert(framePerSecond > 0 && framePerSecond < 1000);
        startTimerHz(framePerSecond);
    }

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void drawGraph(juce::Graphics&, juce::Rectangle<int> drawArea);
    void plot(juce::Rectangle<int>&, juce::Graphics&, juce::Colour);
    float sin(float x);
    float saw(float x);
    float sqr(float x);
    int& tableIndex;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (WavetableComponent)
};
