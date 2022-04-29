/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
New_synthAudioProcessor::New_synthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
    //, equalizer(getSampleRate())
    , apvts(*this, nullptr, "Parameters", createParameters())
#endif
    , scopeDataCollector(scopeDataQueue)
{
    createDefaultBands();
    for (int i = 0; i < 16; i++)
    {
        synth.addVoice(new SynthVoice());
    }

    frequencyGraph.resize(300);
    for (size_t i = 0; i < frequencyGraph.size(); i++) {
        frequencyGraph[i] = 20.0 * std::pow(2.0, i / 30.0);
    }
    magnitudeGraph.resize(frequencyGraph.size());

    for (size_t i = 0; i < 5; i++)
    {
        bands[i].magnitudes.resize(frequencyGraph.size(), 1.0);
    }

    synth.clearSounds();
    synth.addSound(new SynthSound());
}

New_synthAudioProcessor::~New_synthAudioProcessor()
{
}

//==============================================================================
const juce::String New_synthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool New_synthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool New_synthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool New_synthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double New_synthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int New_synthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int New_synthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void New_synthAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String New_synthAudioProcessor::getProgramName (int index)
{
    return {};
}

void New_synthAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void New_synthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    sRate = sampleRate;
    synth.setCurrentPlaybackSampleRate(sampleRate);

    for (size_t i = 0; i < bands.size(); i++)
    {
        updateBand(i);
    }

    updatePlots();

    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();

    processorChain.prepare(spec);

    for (int i = 0; i < synth.getNumVoices(); i++)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
        }
    }

    lfo1.prepare({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
    lfo2.prepare({ spec.sampleRate / lfoUpdateRate, spec.maximumBlockSize, spec.numChannels });
}

void New_synthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool New_synthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void New_synthAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    auto& lfo1WaveType = *apvts.getRawParameterValue("LFO1WAVETYPE");
    auto& lfo2WaveType = *apvts.getRawParameterValue("LFO2WAVETYPE");
    auto& lfo1Frequency = *apvts.getRawParameterValue("LFO1FREQUENCY");
    auto& lfo2Frequency = *apvts.getRawParameterValue("LFO2FREQUENCY");
    auto& lfo1Enabled = *apvts.getRawParameterValue("LFO1ENABLED");
    auto& lfo2Enabled = *apvts.getRawParameterValue("LFO1ENABLED");
    auto& lfo1Parameter = *apvts.getRawParameterValue("LFO1PARAMETER");
    auto& lfo2Parameter = *apvts.getRawParameterValue("LFO2PARAMETER");
    auto& lfo1Amount = *apvts.getRawParameterValue("LFO1AMOUNT");
    auto& lfo2Amount = *apvts.getRawParameterValue("LFO2AMOUNT");
    lfo1.setWaveType(lfo1WaveType.load());
    lfo2.setWaveType(lfo2WaveType.load());
    lfo1.setFrequency(lfo1Frequency.load());
    lfo2.setFrequency(lfo2Frequency.load());
    lfo1.setEnabled(lfo1Enabled.load());
    lfo2.setEnabled(lfo2Enabled.load());

    if (lfo1.enabled)
    {
        if (int(lfo1Parameter.load()) > 0)
        {
            // check mod amount
            if (lfo1Amount.load() > 0)
                lfo1.updateAffectedParameter(getModParams()[lfo1Parameter.load()], lfo1Amount.load());
            else
                lfo1.removeAffectedParameter(getModParams()[lfo1Parameter.load()]);
        }
        for (size_t i = 0; i < lfo1.affectedParameters.size(); i++)
        {
            auto lfoOut = lfo1.processSample(0.0f);
            float amt = lfo1.affectedParameters[i].getParameterAmount();
            float min = (amt * apvts.getParameterRange(lfo1.affectedParameters[i].getParameterId()).getRange().getStart());
            float max = (amt * apvts.getParameterRange(lfo1.affectedParameters[i].getParameterId()).getRange().getEnd());
            DBG(min);
            DBG(max);
            auto paramMod = juce::jmap(lfoOut, -1.0f, 1.0f, min, max);
            apvts.getParameter(lfo1.affectedParameters[i].getParameterId())->setValueNotifyingHost(paramMod);
        }
    }
    if (lfo2.enabled)
    {
        if (int(lfo2Parameter.load()) > 0)
        {
            // check mod amount
            if (lfo2Amount.load() != 0.0f)
                lfo2.updateAffectedParameter(getModParams()[lfo2Parameter.load()], lfo2Amount.load());
            else
                lfo2.removeAffectedParameter(getModParams()[lfo2Parameter.load()]);
        }
        for (size_t i = 0; i < lfo2.affectedParameters.size(); i++)
        {
            auto lfoOut = lfo2.processSample(0.0f);
            float amt = lfo2.affectedParameters[i].getParameterAmount();
            float min = (amt * apvts.getParameterRange(lfo2.affectedParameters[i].getParameterId()).getRange().getStart());
            float max = (amt * apvts.getParameterRange(lfo2.affectedParameters[i].getParameterId()).getRange().getEnd());
            auto paramMod = juce::jmap(lfoOut, -1.0f, 1.0f, min, max);
            apvts.getParameter(lfo2.affectedParameters[i].getParameterId())->setValueNotifyingHost(paramMod);
        }
    }

    auto& globalDetune = *apvts.getRawParameterValue(TRANS("GLOBALDETUNE"));
    auto& globalPitch = *apvts.getRawParameterValue(TRANS("GLOBALPITCH"));

    auto& attack = *apvts.getRawParameterValue("ATTACK");
    auto& decay = *apvts.getRawParameterValue("DECAY");
    auto& sustain = *apvts.getRawParameterValue("SUSTAIN");
    auto& release = *apvts.getRawParameterValue("RELEASE");

    auto& osc1WaveChoice = *apvts.getRawParameterValue("OSC1WAVETYPE");
    auto& osc2WaveChoice = *apvts.getRawParameterValue("OSC2WAVETYPE");

    auto& osc1Gain = *apvts.getRawParameterValue("OSC1GAIN");
    auto& osc2Gain = *apvts.getRawParameterValue("OSC2GAIN");

    auto& osc1enabled = *apvts.getRawParameterValue("OSC1ENABLED");
    auto& osc2enabled = *apvts.getRawParameterValue("OSC2ENABLED");

    auto& osc1Detune = *apvts.getRawParameterValue("OSC1DETUNE");
    auto& osc2Detune = *apvts.getRawParameterValue("OSC2DETUNE");

    auto& osc1Pitch = *apvts.getRawParameterValue("OSC1PITCH");
    auto& osc2Pitch = *apvts.getRawParameterValue("OSC2PITCH");

    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            // Oscillator controls
            // ADSR
            // LFO

            //auto& masterGain = *apvts.getRawParameterValue("MASTERGAIN");

            voice->update(attack.load(), decay.load(), sustain.load(), release.load());
            voice->getOsc(0).update(osc1WaveChoice.load(), osc1Gain.load(), osc1Detune.load() + globalDetune.load(), osc1Pitch.load() + globalPitch.load());
            voice->getOsc(1).update(osc2WaveChoice.load(), osc2Gain.load(), osc2Detune.load() + globalDetune.load(), osc2Pitch.load() + globalPitch.load());
            voice->setOscEnabled(0, osc1enabled);
            voice->setOscEnabled(1, osc2enabled);

            table1Index = osc1WaveChoice.load();
            table2Index = osc2WaveChoice.load();

            atk = attack.load();
            dec = decay.load();
            sus = sustain.load();
            rel = release.load();
        }
    }

    for (int i = 0; i < 5; i++)
    {
        auto& filterType = *apvts.getRawParameterValue(TRANS(juce::String(i) + "TYPE"));
        auto& filterFrequency = *apvts.getRawParameterValue(TRANS(juce::String(i) + "FREQUENCY"));
        auto& filterResonance = *apvts.getRawParameterValue(TRANS(juce::String(i) + "RESONANCE"));
        auto& filterGain = *apvts.getRawParameterValue(TRANS(juce::String(i) + "GAIN"));
        auto& filterActive = *apvts.getRawParameterValue(TRANS(juce::String(i) + "ACTIVE"));
        auto& filterSolo = *apvts.getRawParameterValue(TRANS(juce::String(i) + "SOLO"));
        bands[i].filterType = static_cast<FilterType>(filterType.load());
        bands[i].frequency = filterFrequency.load();
        bands[i].resonance = filterResonance.load();
        bands[i].gain = filterGain.load();
        bands[i].active = filterActive.load();

        frequencies[i] = filterFrequency.load();
        resonances[i] = filterResonance.load();
        gains[i] = filterGain.load();

        updateBand(i);
    }

    auto& globalGain = *apvts.getRawParameterValue(TRANS("GLOBALGAIN"));
    processorChain.get<5>().setGainLinear(globalGain.load());


    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> audioBlock{ buffer };
    processorChain.process(juce::dsp::ProcessContextReplacing<float>(audioBlock));
    
    scopeDataCollector.process(buffer.getReadPointer(0), (size_t)buffer.getNumSamples());
}

//==============================================================================
bool New_synthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* New_synthAudioProcessor::createEditor()
{
    return new New_synthAudioProcessorEditor (*this);
}

//==============================================================================
void New_synthAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void New_synthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new New_synthAudioProcessor();
}

// Value Tree
juce::AudioProcessorValueTreeState::ParameterLayout New_synthAudioProcessor::createParameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS("GLOBALGAIN"), "Global Gain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f}, 0.1f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS("GLOBALDETUNE"), "Global Detune", juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique <juce::AudioParameterInt>(TRANS("GLOBALPITCH"), "Global Pitch", -12, 12, 0));

    // ADSR
    params.push_back(std::make_unique <juce::AudioParameterFloat>("ATTACK", "Attack", juce::NormalisableRange<float> { 0.001f, 1.0f, 0.001f}, 0.1f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("DECAY", "Decay", juce::NormalisableRange<float> { 0.001f, 1.0f, 0.001f}, 0.1f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("SUSTAIN", "Sustain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.001f}, 1.0f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("RELEASE", "Release", juce::NormalisableRange<float> { 0.001f, 3.0f, 0.001f}, 0.4f));

    params.push_back(std::make_unique <juce::AudioParameterInt>("OSC1WAVETYPE", "Osc 1 Wave Type", 0, 2, 0));
    params.push_back(std::make_unique <juce::AudioParameterInt>("OSC2WAVETYPE", "Osc 2 Wave Type", 0, 2, 0));

    params.push_back(std::make_unique <juce::AudioParameterFloat>("OSC1GAIN", "Osc 1 Gain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("OSC2GAIN", "Osc 2 Gain", juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.5f));

    params.push_back(std::make_unique <juce::AudioParameterBool>("OSC1ENABLED", "Osc 1 Enabled", 1));
    params.push_back(std::make_unique <juce::AudioParameterBool>("OSC2ENABLED", "Osc 2 Enabled", 1));

    params.push_back(std::make_unique <juce::AudioParameterFloat>("OSC1DETUNE", "Osc 1 Detune", juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("OSC2DETUNE", "Osc 2 Detune", juce::NormalisableRange<float> {-1.0f, 1.0f, 0.01f }, 0.0f));

    params.push_back(std::make_unique <juce::AudioParameterInt>("OSC1PITCH", "Osc 1 Pitch", -12, 12, 0));
    params.push_back(std::make_unique <juce::AudioParameterInt>("OSC2PITCH", "Osc 2 Pitch", -12, 12, 0));

    float maxGain = juce::Decibels::decibelsToGain(24.0f);

    std::vector<float> bandFreqs = { 50.0f, 250.0f, 1000.0f, 3000.0f, 12000.0f };

    for (int band = 0; band < 5; band++)
    {
        juce::String bandString = juce::String(band);
        params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS(bandString + "FREQUENCY"), TRANS(bandString + " Frequency"), juce::NormalisableRange<float> { 20.0f, 20000.0f, 1.0f, 
            std::log(0.5f) / std::log(980.0f / 19980.0f)}, 
            bandFreqs[band],
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return (value < 1000.0f) ?
            juce::String(value, 0) + " Hz" :
            juce::String(value / 1000.0f, 2) + " kHz"; },
            [](juce::String text) { return text.endsWith(" kHz") ?
            text.dropLastCharacters(4).getFloatValue() * 1000.0f :
            text.dropLastCharacters(3).getFloatValue(); }));
        params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS(bandString + "RESONANCE"), TRANS(bandString + " Resonance"), juce::NormalisableRange<float> { 0.1f, 10.0f, 0.1f, 
            std::log(0.5f) / std::log(0.9f / 9.9f)}, 
            1.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) { return juce::String(value, 1); },
            [](const juce::String& text) { return text.getFloatValue(); }));
        params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS(bandString + "GAIN"), TRANS(bandString + " Gain"), juce::NormalisableRange<float> { 1.0f / maxGain, maxGain, 0.001f, 
            std::log(0.5f) / std::log((1.0f - (1.0f / maxGain)) / (maxGain - (1.0f / maxGain)))}, 
            1.0f,
            juce::String(),
            juce::AudioProcessorParameter::genericParameter,
            [](float value, int) {return juce::String(juce::Decibels::gainToDecibels(value), 1) + " dB"; },
            [](juce::String text) {return juce::Decibels::decibelsToGain(text.dropLastCharacters(3).getFloatValue()); }));
        params.push_back(std::make_unique <juce::AudioParameterBool>(TRANS(bandString + "ACTIVE"), TRANS(bandString + " Active"), true));
        params.push_back(std::make_unique <juce::AudioParameterChoice>(TRANS(bandString + "TYPE"), TRANS(bandString + " Type"), getFilterTypes(), New_synthAudioProcessor::NoFilter));
    }

    // Create list of params for each lfo

    params.push_back(std::make_unique <juce::AudioParameterInt>("LFO1WAVETYPE", "LFO 1 Wave Type", 0, 3, 0));
    params.push_back(std::make_unique <juce::AudioParameterInt>("LFO2WAVETYPE", "LFO 2 Wave Type", 0, 3, 0));

    params.push_back(std::make_unique <juce::AudioParameterFloat>("LFO1FREQUENCY", "LFO 1 Frequency", juce::NormalisableRange<float> {0.1f, 20.0f, 0.1f }, 1.0f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>("LFO2FREQUENCY", "LFO 2 Frequency", juce::NormalisableRange<float> {0.1f, 20.0f, 0.1f }, 1.0f));

    params.push_back(std::make_unique <juce::AudioParameterBool>("LFO1ENABLED", "LFO 1 Enabled", 1));
    params.push_back(std::make_unique <juce::AudioParameterBool>("LFO2ENABLED", "LFO 2 Enabled", 1));

    params.push_back(std::make_unique <juce::AudioParameterChoice>(TRANS("LFO1PARAMETER"), TRANS("LFO 1 Parameter"), getModParams(), 0));
    params.push_back(std::make_unique <juce::AudioParameterChoice>(TRANS("LFO2PARAMETER"), TRANS("LFO 2 Parameter"), getModParams(), 0));

    params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS("LFO1AMOUNT"), TRANS("LFO 1 Amount"), juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f));
    params.push_back(std::make_unique <juce::AudioParameterFloat>(TRANS("LFO2AMOUNT"), TRANS("LFO 2 Amount"), juce::NormalisableRange<float> { 0.0f, 1.0f, 0.01f }, 0.0f));

    return { params.begin(), params.end() };
}

void New_synthAudioProcessor::createDefaultBands()
{
    bands.push_back(New_synthAudioProcessor::Band(juce::String("Low"), New_synthAudioProcessor::NoFilter, juce::Colours::limegreen, 50.0f, 0.7f, 1.0f));
    bands.push_back(New_synthAudioProcessor::Band(juce::String("Low-mid"), New_synthAudioProcessor::NoFilter, juce::Colours::lightpink, 250.f, 0.7f, 1.0f));
    bands.push_back(New_synthAudioProcessor::Band(juce::String("Mid"), New_synthAudioProcessor::NoFilter, juce::Colours::cyan, 1000.0f, 0.7f, 1.0f));
    bands.push_back(New_synthAudioProcessor::Band(juce::String("High-mid"), New_synthAudioProcessor::NoFilter, juce::Colours::orangered, 3000.0f, 0.7f, 1.0f));
    bands.push_back(New_synthAudioProcessor::Band(juce::String("High"), New_synthAudioProcessor::NoFilter, juce::Colours::magenta, 12000.0f, 0.7f, 1.0f));
    DBG("made bands");
}

void New_synthAudioProcessor::updateBand(const size_t index)
{
    juce::dsp::IIR::Coefficients<float>::Ptr newCoefficients;
    switch (bands[index].filterType)
    {
    case New_synthAudioProcessor::NoFilter:
        newCoefficients = new juce::dsp::IIR::Coefficients<float>(1, 0, 1, 0);
        break;
    case New_synthAudioProcessor::HighPass:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sRate, bands[index].frequency, bands[index].resonance);
        break;
    case New_synthAudioProcessor::LowPass:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sRate, bands[index].frequency, bands[index].resonance);
        break;
    case New_synthAudioProcessor::AllPass:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeAllPass(sRate, bands[index].frequency, bands[index].resonance);
        break;
    case New_synthAudioProcessor::BandPass:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeBandPass(sRate, bands[index].frequency, bands[index].resonance);
        break;
    case New_synthAudioProcessor::Peak:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sRate, bands[index].frequency, bands[index].resonance, bands[index].gain);
        break;
    case New_synthAudioProcessor::Notch:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeNotch(sRate, bands[index].frequency, bands[index].resonance);
        break;
    case New_synthAudioProcessor::LowShelf:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeLowShelf(sRate, bands[index].frequency, bands[index].resonance, bands[index].gain);
        break;
    case New_synthAudioProcessor::HighShelf:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighShelf(sRate, bands[index].frequency, bands[index].resonance, bands[index].gain);
        break;
    case New_synthAudioProcessor::LowPassFirstOrder:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sRate, bands[index].frequency);
        break;
    case New_synthAudioProcessor::HighPassFirstOrder:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sRate, bands[index].frequency);
        break;
    case New_synthAudioProcessor::AllPassFirstOrder:
        newCoefficients = juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(sRate, bands[index].frequency);
        break;
    default:
        break;
    }

    newCoefficients->getMagnitudeForFrequencyArray(frequencyGraph.data(), bands[index].magnitudes.data(), frequencyGraph.size(), sRate);
    if (bands[index].filterType != New_synthAudioProcessor::Notch)
        bands[index].magnitudeAtCenter = newCoefficients->getMagnitudeForFrequency(bands[index].frequency, sRate);
    else
        bands[index].magnitudeAtCenter = juce::Decibels::decibelsToGain(0);

    if (newCoefficients)
    {
        {
            if (index == 0)
                *processorChain.get<0>().state = *newCoefficients;
            else if (index == 1)
                *processorChain.get<1>().state = *newCoefficients;
            else if (index == 2)
                *processorChain.get<2>().state = *newCoefficients;
            else if (index == 3)
                *processorChain.get<3>().state = *newCoefficients;
            else if (index == 4)
                *processorChain.get<4>().state = *newCoefficients;
        }
    }
    if (index == 0)
        processorChain.setBypassed<0>(!bands[index].active);
    else if (index == 1)
        processorChain.setBypassed<1>(!bands[index].active);
    else if (index == 2)
        processorChain.setBypassed<2>(!bands[index].active);
    else if (index == 3)
        processorChain.setBypassed<3>(!bands[index].active);
    else if (index == 4)
        processorChain.setBypassed<4>(!bands[index].active);

    updatePlots();
}

New_synthAudioProcessor::Band New_synthAudioProcessor::getBand(size_t index)
{
    return bands[index];
}

void New_synthAudioProcessor::updatePlots()
{
    std::fill(magnitudeGraph.begin(), magnitudeGraph.end(), juce::Decibels::decibelsToGain(0));
    for (size_t i = 0; i < 5; i++)
    {
        if (bands[i].active)
        {
            juce::FloatVectorOperations::multiply(magnitudeGraph.data(), bands[i].magnitudes.data(), static_cast<int> (magnitudeGraph.size()));
        }
    }
}
