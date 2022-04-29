/*
  ==============================================================================

    LfoData.h
    Created: 16 Apr 2022 1:44:05pm
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class LfoData : public juce::dsp::Oscillator<float>
{
public:
    
    class LfoParameter
    {
    public:
        LfoParameter(juce::String, float);
        ~LfoParameter();

        juce::String& getParameterId() { return parameterId; };
        float& getParameterAmount() { return parameterAmount; };
        float& getInitialValue() { return initialValue; };

        void setParameterAmount(float amount) { parameterAmount = amount; }
        void setParamInitial(float amount) { initialValue = amount; }

    private:
        juce::String parameterId;
        float parameterAmount;
        float initialValue = 0.0f;
    };

    void addAffectedParameter(juce::String, float);
    void removeAffectedParameter(juce::String);
    void updateAffectedParameter(juce::String, float);
    void setEnabled(bool a) { enabled = a; }

    std::vector<LfoParameter> affectedParameters;

    void setWaveType(const int);

    int waveType = 0;
    float frequency = 1.0f;

    bool enabled = true;

private:
    std::vector<juce::String> paramNames;
};
