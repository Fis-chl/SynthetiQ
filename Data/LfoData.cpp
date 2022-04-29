/*
  ==============================================================================

    LfoData.cpp
    Created: 16 Apr 2022 1:44:05pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "LfoData.h"


LfoData::LfoParameter::LfoParameter(const juce::String paramId, const float amt)
{
    parameterId = paramId;
    parameterAmount = amt;
}

LfoData::LfoParameter::~LfoParameter()
{
}

void LfoData::addAffectedParameter(juce::String parameterId, float amount)
{
    auto itr = std::find(paramNames.begin(), paramNames.end(), parameterId);
    if (itr == paramNames.cend())
    {
        paramNames.push_back(parameterId);
        affectedParameters.push_back(LfoParameter(parameterId, amount));
    }
}

void LfoData::removeAffectedParameter(juce::String parameterId)
{
    auto itr = std::find(paramNames.begin(), paramNames.end(), parameterId);
    if (itr != paramNames.cend())
    {
        int index = std::distance(paramNames.begin(), itr);
        affectedParameters.erase(affectedParameters.begin() + index);
        paramNames.erase(paramNames.begin() + index);
    }
}

void LfoData::updateAffectedParameter(juce::String parameterId, float amount)
{
    auto itr = std::find(paramNames.begin(), paramNames.end(), parameterId);
    if (itr != paramNames.cend())
    {
        int index = std::distance(paramNames.begin(), itr);
        affectedParameters[index].setParameterAmount(amount);
    }
    else
    {
        addAffectedParameter(parameterId, amount);
    }
}

void LfoData::setWaveType(const int choice)
{
    switch (choice)
    {
    case 0:
        // Sin wave
        initialise([](float x) { return std::sin(x); }, 128);
        break;

    case 1:
        // Saw wave
        initialise([](float x) { return x / juce::MathConstants<float>::twoPi; }, 128);
        break;

    case 2:
        // Square wave
        initialise([](float x) { return x < 0.0f ? -1.0f : 1.0f; }, 128);
        break;
    case 3:
        // Triangle wave
        initialise([](float x) { return (2 / juce::MathConstants<float>::pi) * (std::asin(std::sin(juce::MathConstants<float>::pi * x))); }, 128);
        break;

    default:
        jassertfalse;  // Shouldn't be here
        break;
    }

    waveType = choice;
}