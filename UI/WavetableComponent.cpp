/*
  ==============================================================================

    WavetableComponent.cpp
    Created: 26 Feb 2022 1:04:28am
    Author:  lukeb_000

  ==============================================================================
*/

#include <JuceHeader.h>
#include "WavetableComponent.h"

//==============================================================================
WavetableComponent::WavetableComponent(int& index) : tableIndex(index)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    setFramePerSecond(5);
}

WavetableComponent::~WavetableComponent()
{
}


void WavetableComponent::paint(juce::Graphics& g)
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
        juce::String text("Oscillator");
        juce::Colour fillColour = juce::Colours::white;
        g.setColour(fillColour);
        g.setFont(panelNameFont);
        g.drawText(text, bounds.removeFromTop(panelNameHeight).reduced(localMargin), juce::Justification::centred, true);
    }

    juce::Rectangle<int> drawArea = getLocalBounds();
    drawArea.removeFromTop(panelNameHeight);
    drawArea.reduce(10, 10);

    //g.setColour(juce::Colours::darkgrey);
    //g.fillRect(drawArea);

    drawGraph(g, drawArea);
    g.setColour(juce::Colours::cyan);

    //plot(drawArea, g, juce::Colours::lightcyan);
}

void WavetableComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..
}

void WavetableComponent::timerCallback()
{
    repaint();
}

void WavetableComponent::drawGraph(juce::Graphics& g, juce::Rectangle<int> drawArea)
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
    g.drawText(TRANS("+1"), newArea.getX() - 22, newArea.getY() - 10, 20, 20, juce::Justification::right);
    g.drawText(TRANS("0"), newArea.getX() - 22, newArea.getY() - 10 + (newArea.getHeight() / 2), 20, 20, juce::Justification::right);
    g.drawText(TRANS("-1"), newArea.getX() - 22, newArea.getBottom() - 10, 20, 20, juce::Justification::right);

    plot(newArea, g, graphColour);
}

void WavetableComponent::plot(juce::Rectangle<int>& drawArea, juce::Graphics& g, juce::Colour graphColour)
{
    juce::Rectangle<int> newDraw = juce::Rectangle<int>{ drawArea.getX(), drawArea.getY(), drawArea.getWidth(), drawArea.getHeight() };

    const int width = newDraw.getWidth();
    const int right = newDraw.getRight();
    const int left = right - width;
    const int height = newDraw.getHeight();
    const int center = (height / 2);
    const int alignedCenter = newDraw.getBottom() - (drawArea.getHeight() / 2);

    g.setColour(graphColour);

    for (size_t i = 1; i < width; i++)
    {
        float x1 = i - 1;
        float x2 = i;
        float y1 = 0.0f;
        float y2 = 0.0f;
        if (tableIndex == 0)
        {
            y1 = alignedCenter + ((sin((x1 * juce::MathConstants<float>::twoPi) / width)) * center);
            y2 = alignedCenter + ((sin((x2 * juce::MathConstants<float>::twoPi) / width)) * center);
        }
        else if (tableIndex == 1)
        {
            y1 = alignedCenter + ((saw(x1 / width)) * center);
            y2 = alignedCenter + ((saw(x2 / width)) * center);
        }
        else
        {
            y1 = alignedCenter + ((sqr(x1 * juce::MathConstants<float>::twoPi / width)) * center);
            y2 = alignedCenter + ((sqr(x2 * juce::MathConstants<float>::twoPi / width)) * center);
        }
        g.drawLine(x1 + left, y1, x2 + left, y2, 1.0f);
    }
}

float WavetableComponent::sin(float x)
{
    return std::sin(x);
}

float WavetableComponent::saw(float x)
{
    float cotx = std::sin(juce::MathConstants<float>::pi * x) / std::cos(juce::MathConstants<float>::pi * x);
    float val = ( - 2 / juce::MathConstants<float>::pi) * std::atan(cotx);
    return val;
}

float WavetableComponent::sqr(float x)
{
    if (std::sin(x) < 0) { return -1.0f; }
    return 1.0f;
}
