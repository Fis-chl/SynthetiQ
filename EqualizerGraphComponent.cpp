/*
  ==============================================================================

    EqualizerGraphComponent.cpp
    Created: 14 Apr 2022 5:58:12pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "EqualizerGraphComponent.h"

EqualizerGraphComponent::EqualizerGraphComponent(New_synthAudioProcessor& p)
    : processor(p)
{
    startTimerHz(60);
}

EqualizerGraphComponent::~EqualizerGraphComponent()
{
}

void EqualizerGraphComponent::paint(juce::Graphics& g)
{
    juce::Colour panelColour = juce::Colour(26, 33, 38);
    juce::Colour lineColour = juce::Colour(44, 59, 69);
    //53, 72, 84
    juce::Colour thickLineColour = juce::Colour(69, 91, 107);
    juce::Colour textColour = juce::Colour(120, 162, 191);

    juce::Colour graphColour = juce::Colours::lightcyan;

    float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
    float textPadding = 2;
    juce::Rectangle<float> drawArea = juce::Rectangle<float>(x, y, width, height);

    g.setColour(panelColour);
    g.fillRoundedRectangle(drawArea, 10.0f);


    juce::Rectangle<float> newArea = drawArea;
    newArea.reduce(10, 20);

    g.setColour(lineColour);
    auto numberToDraw = 24;
    juce::String sign = TRANS("+");
    int center = 0;
    for (size_t i = 0; i < 5; i++)
    {
        float lineVertPos = 10 + ((i * newArea.getHeight()) / 4);
        g.setColour(textColour);
        g.drawFittedText(sign + juce::String(numberToDraw), 0.0f, lineVertPos - 15.0f, 25.0f, 30.0f, juce::Justification::right, 1);
        g.setColour(lineColour);
        g.drawHorizontalLine(lineVertPos, drawArea.getRight() - drawArea.getWidth() + 25, drawArea.getRight());
        numberToDraw = numberToDraw - 12;
        if (numberToDraw < 0)
            sign = TRANS("");
        if (i == 2)
            center = lineVertPos;
    }
    std::vector<int> logs = { 30, 40, 50, 60, 70, 80, 90, 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000, 15000 };
    for (int log : logs)
    {
        if (log == 100 || log == 1000 || log == 10000)
            drawGraphLog(g, log, drawArea, true, thickLineColour, textColour);
        else 
            drawGraphLog(g, log, drawArea, false, lineColour, textColour);
        g.setColour(lineColour);
    }
    drawFrequencyMagnitudes(g, graphColour, center, newArea.getHeight());
    drawEqBands(g, center, newArea.getWidth(), newArea.getHeight());
}

void EqualizerGraphComponent::drawGraphLog(juce::Graphics& g, int toDraw, juce::Rectangle<float> drawArea, bool drawText, juce::Colour lineColour, juce::Colour textColour)
{
    float logFactor = std::log(20000) - std::log(20);
    float xPos = (std::log(toDraw) / logFactor) - (std::log(20) / logFactor);
    if (drawText)
    {
        juce::String suffix = TRANS("hz");
        juce::String logText = juce::String(toDraw);
        if (toDraw >= 1000)
        {
            suffix = TRANS("khz");
            logText = juce::String(toDraw / 1000);
        }
        g.setColour(textColour);
        g.drawFittedText(logText + suffix, xPos * drawArea.getWidth() + getX() - 20, drawArea.getHeight() - 30, 40.0f, 30.0f, juce::Justification::centred, 1);
        g.setColour(lineColour);
        g.drawVerticalLine((xPos * drawArea.getWidth()) + getX(), drawArea.getHeight() - 8, drawArea.getBottom());
        g.drawVerticalLine((xPos * drawArea.getWidth()) + getX(), drawArea.getY(), drawArea.getHeight() - 22);
    }
    else
    {
        g.drawVerticalLine((xPos * drawArea.getWidth()) + getX(), drawArea.getY(), drawArea.getBottom());
    }
}

void EqualizerGraphComponent::drawFrequencyMagnitudes(juce::Graphics& g, juce::Colour lineColour, int center, float drawHeight)
{
    float logFactor = std::log(20000) - std::log(20);

    auto frequencies = processor.getFrequencyGraph();
    auto magnitudes = processor.getMagnitudeGraph();

    const auto xFactor = static_cast<double> (getWidth()) / frequencies.size();
    const auto yFactor = 2.0f * drawHeight / juce::Decibels::decibelsToGain(24.0f);

    g.setColour(lineColour);

    for (int i = 1; i < frequencies.size(); i++)
    {
        auto startX = (getX()) + ((i - 1) * xFactor);
        auto startY = float(magnitudes[i - 1] > 0 ? center - yFactor * std::log(magnitudes[i - 1]) / std::log(2.0f) : getBottom());
        auto endX = (getX()) + (i * xFactor);
        auto endY = float(magnitudes[i] > 0 ? center - yFactor * std::log(magnitudes[i]) / std::log(2.0f) : getBottom());
        g.drawLine(startX, startY, endX, endY, 1.0f);
    }
}

void EqualizerGraphComponent::drawEqBands(juce::Graphics& g, int center, float drawWidth, float drawHeight)
{
    float logFactor = std::log(20000) - std::log(20);

    auto frequencies = processor.getFrequencyGraph();
    auto magnitudes = processor.getMagnitudeGraph();

    const auto xFactor = static_cast<double> (getWidth()) / frequencies.size();
    const auto yFactor = 2.0f * drawHeight / juce::Decibels::decibelsToGain(24.0f);

    int counter = 1;
    for (New_synthAudioProcessor::Band& band : processor.bands)
    {
        if (band.active)
        {
            auto yPos = float(band.magnitudeAtCenter > 0 ? center - yFactor * std::log(band.magnitudeAtCenter) / std::log(2.0f) : getBottom());
            auto xPos = (((std::log(band.frequency / 20.0f) / std::log(2.0f)) / 10.0f));
            xPos = juce::roundToInt (getX() + (drawWidth * xPos));
            xPos = xPos + (2.0f * std::log(band.frequency));
            g.setColour(band.colour);
            g.fillEllipse(xPos - 10, yPos - 10, 20, 20);
            g.setColour(juce::Colours::black);
            g.drawFittedText(juce::String(counter), xPos - 10, yPos - 10, 20, 20, juce::Justification::centred, 1);
        }
        counter++;
    }
}

void EqualizerGraphComponent::resized()
{
}

void EqualizerGraphComponent::timerCallback()
{
    repaint();
}
