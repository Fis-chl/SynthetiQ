/*
  ==============================================================================

    AdsrGraphComponent.cpp
    Created: 12 Apr 2022 3:05:39am
    Author:  lukeb_000

  ==============================================================================
*/

#include "AdsrGraphComponent.h"

AdsrGraphComponent::AdsrGraphComponent(float& attack, float& decay, float& sustain, float& release) :
    attack(attack), decay(decay), sustain(sustain), release(release)
{
    setFramePerSecond(60);
}

AdsrGraphComponent::~AdsrGraphComponent()
{
}

void AdsrGraphComponent::paint(juce::Graphics& g)
{
    int panelNameHeight = 42;
    int localMargin = 2;
    juce::Font panelNameFont = juce::Font(24.0f, juce::Font::plain).withTypefaceStyle("Italic");

    {
        float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
        juce::Colour panelColour = juce::Colour(36, 36, 36);
        g.setColour(panelColour);
        g.fillRoundedRectangle(x, y, width, height, 10.0f);
    }

    {
        juce::Rectangle<int> bounds = getLocalBounds();
        juce::String text("Envelope");
        juce::Colour fillColour = juce::Colours::white;
        g.setColour(fillColour);
        g.setFont(panelNameFont);
        g.drawText(text, bounds.removeFromTop(panelNameHeight).reduced(localMargin), juce::Justification::centred, true);
    }

    juce::Rectangle<int> drawArea = getLocalBounds();
    drawArea.removeFromTop(panelNameHeight);
    drawArea.reduce(10, 10);

    g.setColour(juce::Colours::cyan);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);

    drawGraph(g, drawArea);
    //plot(drawArea, g);
}

void AdsrGraphComponent::resized()
{
    // Lol
}

void AdsrGraphComponent::timerCallback()
{
    repaint();
}

void AdsrGraphComponent::drawGraph(juce::Graphics& g, juce::Rectangle<int> drawArea)
{
    juce::Colour panelColour = juce::Colour(26, 33, 38);
    juce::Colour lineColour = juce::Colour(44, 59, 69);
    //53, 72, 84
    juce::Colour thickLineColour = juce::Colour(69, 91, 107);
    juce::Colour textColour = juce::Colour(120, 162, 191);

    juce::Colour graphColour = juce::Colours::lightcyan;

    juce::Font f = juce::Font(14.0f, juce::Font::plain);
    g.setFont(f);

    g.setColour(panelColour);
    juce::Rectangle<int> newArea = drawArea;
    g.fillRoundedRectangle(float(drawArea.getX()), float(drawArea.getY()), float(drawArea.getWidth()), float(drawArea.getHeight()), 5.0f);
    newArea.reduce(20, 10);
    g.setColour(lineColour);
    g.drawHorizontalLine(newArea.getBottom() - (newArea.getHeight() / 2), drawArea.getX(), drawArea.getRight());
    g.drawHorizontalLine(newArea.getBottom(), drawArea.getX(), drawArea.getRight());
    g.drawHorizontalLine(newArea.getY(), drawArea.getX(), drawArea.getRight());

    g.drawVerticalLine(newArea.getX(), drawArea.getY(), drawArea.getBottom());
    g.drawVerticalLine(newArea.getX() + (newArea.getWidth() / 2), drawArea.getY(), drawArea.getBottom());
    g.drawVerticalLine(newArea.getRight(), drawArea.getY(), drawArea.getBottom());

    g.setColour(textColour);
    g.drawText(TRANS("1"), newArea.getX() - 22, newArea.getY() - 10, 20, 20, juce::Justification::right);
    g.drawText(TRANS(".5"), newArea.getX() - 22, newArea.getY() - 10 + (newArea.getHeight() / 2), 20, 20, juce::Justification::right);
    g.drawText(TRANS("0"), newArea.getX() - 22, newArea.getBottom() - 10, 20, 20, juce::Justification::right);

    g.setColour(graphColour);
    plot(newArea, g);
}

void AdsrGraphComponent::plot(juce::Rectangle<int>& drawArea, juce::Graphics& g)
{
    juce::Rectangle<int> newDraw = juce::Rectangle<int>{ drawArea.getX(), drawArea.getY(), drawArea.getWidth(), drawArea.getHeight() };

    const int width = newDraw.getWidth();
    const int right = newDraw.getRight();
    const int left = right - width;
    const int height = newDraw.getHeight();
    const int center = (height / 2);
    const int bottom = newDraw.getBottom();

    // Calculate the x-coordinate ranges for each part of ADSR
    float graphTotalWidth = attack + decay + release;

    float attackWidth = (attack / graphTotalWidth) * width;
    float decayWidth = (decay / graphTotalWidth) * width;
    float releaseWidth = (release / graphTotalWidth) * width;

    // Attack line
    g.drawLine(0 + left, bottom, attackWidth + left, bottom - height, 1.0f);
    // Decay line
    g.drawLine(attackWidth + left, bottom - height, attackWidth + decayWidth + left, bottom - (sustain * height), 1.0f);
    // Release line
    g.drawLine(attackWidth + decayWidth + left, bottom - (sustain * height), width + left, bottom, 1.0f);

}
