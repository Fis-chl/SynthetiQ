/*
  ==============================================================================

    OscData.h
    Created: 25 Feb 2022 9:48:07pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class OscData : public juce::dsp::Oscillator<float>
{
public:
    void prepareToPlay(juce::dsp::ProcessSpec& spec);
    void setWaveType(const int choice);
    void setWaveFrequency(const int midiNoteNumber);
    void setGain(float gain);
    void getNextAudioBlock(juce::dsp::AudioBlock<float>& block);
    void update(int, float, float, int);
    void setMidiNoteNumber(int num) { currentMidiNoteNumber = num; }

private:
    juce::dsp::Gain<float> oscGain;
    float detune = 0.0f;
    int pitch = 0;
    int currentMidiNoteNumber;
};
