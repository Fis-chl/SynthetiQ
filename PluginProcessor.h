/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"
#include "SynthSound.h"
#include "Visualizer/ScopeComponent.h"
#include "Data/LfoData.h"

//==============================================================================
/**
*/
class New_synthAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    New_synthAudioProcessor();
    ~New_synthAudioProcessor() override;

    enum FilterType
    {
        NoFilter = 0,
        HighPass,
        LowPass,
        AllPass,
        BandPass,
        Notch,
        Peak,
        LowShelf,
        HighShelf,
        HighPassFirstOrder,
        LowPassFirstOrder,
        AllPassFirstOrder
    };

    struct Band
    {
        Band(juce::String bandName, FilterType bandFilterType, juce::Colour bandColour, float bandFrequency,
            float bandResonance, float bandGain, bool bandActive = true) :
            name(bandName),
            filterType(bandFilterType),
            colour(bandColour),
            frequency(bandFrequency),
            resonance(bandResonance),
            gain(bandGain),
            active(bandActive)

        {}

        juce::String name;
        FilterType filterType;
        juce::Colour colour;
        float frequency = 1000.0f;
        float resonance = 1.0f;
        float gain = 1.0f;
        bool active = true;
        std::vector<double> magnitudes;
        double magnitudeAtCenter;

    };

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioBufferQueue<float>& getAudioBufferQueue() { return scopeDataQueue; }
    int& getTable1Index() { return table1Index; }
    int& getTable2Index() { return table2Index; }

    float& getAttack() { return atk; }
    float& getDecay() { return dec; }
    float& getSustain() { return sus; }
    float& getRelease() { return rel; }

    std::vector<float>& getFrequencies() { return frequencies; }
    std::vector<float>& getResonances() { return resonances; }
    std::vector<float>& getGains() { return gains; }

    std::vector<double>& getMagnitudeGraph() { return magnitudeGraph; }
    std::vector<double>& getFrequencyGraph() { return frequencyGraph; }

    New_synthAudioProcessor::Band getBand(size_t index);

    juce::AudioProcessorValueTreeState apvts;
    std::vector<Band> bands;
    LfoData lfo1;
    LfoData lfo2;

    static constexpr size_t lfoUpdateRate = 100;
    size_t lfo1UpdateCounter = lfoUpdateRate;
    size_t lfo2UpdateCounter = lfoUpdateRate;

    juce::AudioProcessorValueTreeState& New_synthAudioProcessor::getPluginState()
    {
        return apvts;
    }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
    juce::StringArray getFilterTypes()
    {
        return {
            TRANS("No Filter"),
            TRANS("High Pass"),
            TRANS("Low Pass"),
            TRANS("All Pass"),
            TRANS("Band Pass"),
            TRANS("Notch"),
            TRANS("Peak"),
            TRANS("Low Shelf"),
            TRANS("High Shelf"),
            TRANS("High Pass 1st"),
            TRANS("Low Pass 1st"),
            TRANS("All Pass 1st"),
        };
    }

    juce::StringArray getModParams()
    {
        return {
            TRANS("..."),
            TRANS("OSC1WAVETYPE"),
            TRANS("OSC2WAVETYPE"),
            TRANS("OSC1DETUNE"),
            TRANS("OSC2DETUNE"),
            TRANS("OSC1PITCH"),
            TRANS("OSC2PITCH"),
            TRANS("0FREQUENCY"),
            TRANS("0RESONANCE"),
            TRANS("0GAIN"),
            TRANS("1FREQUENCY"),
            TRANS("1RESONANCE"),
            TRANS("1GAIN"),
            TRANS("2FREQUENCY"),
            TRANS("2RESONANCE"),
            TRANS("2GAIN"),
            TRANS("3FREQUENCY"),
            TRANS("3RESONANCE"),
            TRANS("3GAIN"),
            TRANS("4FREQUENCY"),
            TRANS("4RESONANCE"),
            TRANS("4GAIN")
        };
    }


    void createDefaultBands();
    void updateBand(const size_t index);
    void updateBypassedStates();
    void updatePlots();

    juce::Synthesiser synth;
    std::vector<double> magnitudeGraph;
    std::vector<double> frequencyGraph;

    AudioBufferQueue<float> scopeDataQueue;
    ScopeDataCollector<float> scopeDataCollector;
    int table1Index;
    int table2Index;

    // ADSR
    float atk;
    float dec;
    float sus;
    float rel;

    double sRate = 0;

    // EQ
    int numBands = 5;

    std::vector<float> frequencies = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    std::vector<float> resonances = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
    std::vector<float> gains = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

    using Oscillator = juce::dsp::Oscillator<float>;
    using FilterBand = juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>;
    using Gain = juce::dsp::Gain<float>;
    juce::dsp::ProcessorChain<FilterBand, FilterBand, FilterBand, FilterBand, FilterBand, Gain> processorChain;

    std::unordered_map<juce::String, FilterType> const table = 
    { { "LowPass",FilterType::LowPass },
{ "HighPass", FilterType::HighPass },
{ "Peak", FilterType::Peak },
{ "Notch", FilterType::Notch },
{ "BandPass", FilterType::BandPass },
{ "LowShelf", FilterType::LowShelf },
{ "HighShelf", FilterType::HighShelf } };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (New_synthAudioProcessor)
};
