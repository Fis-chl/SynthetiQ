/*
  ==============================================================================

    SynthVoice.cpp
    Created: 25 Feb 2022 5:01:16pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "SynthVoice.h"

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<juce::SynthesiserSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    if (osc1enabled) { osc1.setWaveFrequency(midiNoteNumber + detune); }
    if (osc2enabled) { osc2.setWaveFrequency(midiNoteNumber + detune); }
    if (osc1enabled || osc2enabled) { adsr.noteOn(); }
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    adsr.noteOff();

    if (!allowTailOff || !adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{

}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{

}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    adsr.setSampleRate(sampleRate);

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = outputChannels;

    if (osc1enabled) { osc1.prepareToPlay(spec); }
    if (osc2enabled) { osc2.prepareToPlay(spec); }

    isPrepared = true;
}

void SynthVoice::update(const float attack, const float decay, const float sustain, const float release)
{
    adsr.updateADSR(attack, decay, sustain, release);
}

void SynthVoice::setDetune(const float amount)
{
    detune = amount;
}

void SynthVoice::setGain(const float _gain)
{
    //gain.setGainLinear(_gain);
}

OscData& SynthVoice::getOsc(int index)
{
    switch (index)
    {
    case 0:
        return osc1;
        break;
    case 1:
        return osc2;
        break;
    default:
        return osc1;
        break;
    }
}

void SynthVoice::setOscEnabled(int index, bool enabled)
{
    switch (index)
    {
    case 0:
        osc1enabled = enabled;
        break;
    case 1:
        osc2enabled = enabled;
        break;
    default:
        osc1enabled = enabled;
        break;
    }
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    jassert(isPrepared);

    if (!isVoiceActive())
        return;

    synthBuffer.setSize(outputBuffer.getNumChannels(), numSamples, false, false, true);
    synthBuffer.clear();

    juce::dsp::AudioBlock<float> audioBlock{ synthBuffer };
    juce::dsp::AudioBlock<float> secondBlock{ synthBuffer };
    if (osc1enabled) { osc1.getNextAudioBlock(audioBlock); }
    if (osc2enabled) { osc2.getNextAudioBlock(audioBlock); }

    if (osc1enabled || osc2enabled)
    {
        adsr.applyEnvelopeToBuffer(synthBuffer, 0, synthBuffer.getNumSamples());

        for (int channel = 0; channel < outputBuffer.getNumChannels(); channel++)
        {
            outputBuffer.addFrom(channel, startSample, synthBuffer, channel, 0, numSamples);

            if (!adsr.isActive())
                clearCurrentNote();
        }
    }
}
 