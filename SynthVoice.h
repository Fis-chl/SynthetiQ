/*
  ==============================================================================

    SynthVoice.h
    Created: 25 Feb 2022 5:01:16pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthSound.h"
#include "Data/AdsrData.h"
#include "Data/OscData.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels);
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    void update(const float attack, const float decay, const float sustain, const float release);
    void setDetune(const float amount);
    void setGain(const float gain);
    OscData& getOsc(int index);
    void setOscEnabled(int index, bool enabled);

private:
    AdsrData adsr;
    juce::AudioBuffer<float> synthBuffer;

    OscData osc1;
    OscData osc2;
    bool osc1enabled = true;
    bool osc2enabled = true;
    //juce::dsp::Gain<float> gain;
    bool isPrepared{ false };
    float detune = 0.0f;


};
