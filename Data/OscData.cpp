/*
  ==============================================================================

    OscData.cpp
    Created: 25 Feb 2022 9:48:07pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "OscData.h"

void OscData::prepareToPlay(juce::dsp::ProcessSpec& spec)
{
    prepare(spec);
    oscGain.prepare(spec);
}

void OscData::update(const int waveType, const float gain, const float nDetune, const int nPitch)
{
    setWaveType(waveType);
    setGain(gain);
    detune = nDetune;
    pitch = nPitch;
    setWaveFrequency(currentMidiNoteNumber);
}

void OscData::setWaveType(const int choice)
{
    switch (choice)
    {
    case 0:
        // Sin wave
        initialise([](float x) { return std::sin(x); }, 128);
        break;

    case 1:
        // Saw wave
        initialise([](float x) { return x / juce::MathConstants<float>::pi; }, 128);
        break;

    case 2:
        // Square wave
        initialise([](float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
        break;

    default:
        jassertfalse;  // Shouldn't be here
        break;
    }
}

void OscData::setWaveFrequency(const int midiNoteNumber)
{
    currentMidiNoteNumber = midiNoteNumber;
    float noteHz = juce::MidiMessage::getMidiNoteInHertz(currentMidiNoteNumber + pitch);
    // Maximum detune is 1 note above/below
    float tDetune = 0.0f;
    if (detune > 0.0f)
        tDetune = juce::MidiMessage::getMidiNoteInHertz(currentMidiNoteNumber + 1) - noteHz;
    else
        tDetune = noteHz - juce::MidiMessage::getMidiNoteInHertz(currentMidiNoteNumber - 1);
    tDetune = tDetune * detune;
    setFrequency(noteHz + tDetune);
}

void OscData::setGain(float gain)
{
    oscGain.setGainLinear(gain);
}

void OscData::getNextAudioBlock(juce::dsp::AudioBlock<float>& block)
{
    process(juce::dsp::ProcessContextReplacing<float>(block));
    oscGain.process(juce::dsp::ProcessContextReplacing<float>(block));
}
