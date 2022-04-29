/*
  ==============================================================================

    SynthSound.h
    Created: 25 Feb 2022 5:01:31pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class SynthSound : public juce::SynthesiserSound
{
public:
    bool appliesToNote(int midiNoteNumber) override { return true; }
    bool appliesToChannel(int midiChannel) override { return true; }

};