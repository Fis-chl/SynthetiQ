/*
  ==============================================================================

    LfoComponent.cpp
    Created: 16 Apr 2022 1:31:22pm
    Author:  lukeb_000

  ==============================================================================
*/

#include "LfoComponent.h"

LfoComponent::LfoComponent(juce::AudioProcessorValueTreeState& apvts, int& index, juce::String id) : tableIndex(index), graphComponent(index), lfoId(id)
{
    addAndMakeVisible(graphComponent);
    setSliderParams(waveSelector);
    waveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, juce::String((lfoId)+"WAVETYPE"), waveSelector);

    onButton.setClickingTogglesState(true);
    onButton.setColour(juce::TextButton::buttonOnColourId, juce::Colours::cyan);
    onButton.setColour(juce::TextButton::textColourOnId, juce::Colours::black);
    onAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, juce::String((lfoId)+"ENABLED"), onButton);
    addAndMakeVisible(onButton);
    onButton.setTooltip(TRANS("Activate or deactivate this LFO"));

    if (auto* choiceParameter = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(TRANS(juce::String(lfoId) + "PARAMETER"))))
        parameterSelect.addItemList(choiceParameter->choices, 1);

    paramAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, TRANS(juce::String(lfoId) + "PARAMETER"), parameterSelect);
    amountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, TRANS(juce::String(lfoId) + "AMOUNT"), paramAmount);
    frequencyAttachment = std::make_unique < juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, TRANS(juce::String(lfoId) + "FREQUENCY"), frequency);

    addAndMakeVisible(parameterSelect);
    setSliderParams(paramAmount);
    setSliderParams(frequency);

    waveSelector.setTooltip("Select the type of waveform for this LFO");
    parameterSelect.setTooltip("Select parameter to be automated");
    paramAmount.setTooltip("The amount that the selected parameter should be automated");
    frequency.setTooltip("Oscillator frequency in hz");

}

LfoComponent::~LfoComponent()
{
}

void LfoComponent::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 40, 20);
    slider.setColour(juce::Slider::trackColourId, juce::Colours::aliceblue);
    addAndMakeVisible(slider);
}

LfoComponent::LfoGraphComponent::LfoGraphComponent(int& index) : tableIndex(index)
{
    setFramePerSecond(30);
}

LfoComponent::LfoGraphComponent::~LfoGraphComponent()
{
}

void LfoComponent::LfoGraphComponent::paint(juce::Graphics& g)
{
    juce::Rectangle<int> drawArea = getLocalBounds();

    //g.setColour(juce::Colours::darkgrey);
    //g.fillRect(drawArea);
    //g.setColour(juce::Colours::cyan);
    getLookAndFeel().setColour(juce::Slider::thumbColourId, juce::Colours::cyan);

    drawGraph(g);
}

void LfoComponent::LfoGraphComponent::resized()
{
    // Lol
}

void LfoComponent::LfoGraphComponent::timerCallback()
{
    repaint();
}

void LfoComponent::LfoGraphComponent::drawGraph(juce::Graphics& g)
{
    juce::Colour panelColour = juce::Colour(26, 33, 38);
    juce::Colour lineColour = juce::Colour(44, 59, 69);
    //53, 72, 84
    juce::Colour thickLineColour = juce::Colour(69, 91, 107);
    juce::Colour textColour = juce::Colour(120, 162, 191);

    juce::Colour graphColour = juce::Colours::lightcyan;

    g.setColour(panelColour);
    g.fillRoundedRectangle(float(getLocalBounds().getX()), float(getLocalBounds().getY()), float(getLocalBounds().getWidth()), float(getLocalBounds().getHeight()), 5.0f);

    g.setColour(lineColour);
    juce::Rectangle<int> drawArea = getLocalBounds();
    drawArea.reduce(20, 10);
    g.drawHorizontalLine(getLocalBounds().getHeight() / 2, getLocalBounds().getX(), getLocalBounds().getRight());
    g.drawHorizontalLine(drawArea.getBottom(), getLocalBounds().getX(), getLocalBounds().getRight());
    g.drawHorizontalLine(drawArea.getY(), getLocalBounds().getX(), getLocalBounds().getRight());

    g.drawVerticalLine(drawArea.getX(), getLocalBounds().getY(), getLocalBounds().getBottom());
    g.drawVerticalLine(drawArea.getX() + (drawArea.getWidth() / 2), getLocalBounds().getY(), getLocalBounds().getBottom());
    g.drawVerticalLine(drawArea.getRight(), getLocalBounds().getY(), getLocalBounds().getBottom());

    g.setColour(textColour);
    g.drawText(TRANS("+1"), drawArea.getX() - 22, drawArea.getY() - 10, 20, 20, juce::Justification::right);
    g.drawText(TRANS("0"), drawArea.getX() - 22, drawArea.getY() - 10 + (drawArea.getHeight() / 2), 20, 20, juce::Justification::right);
    g.drawText(TRANS("-1"), drawArea.getX() - 22, drawArea.getBottom() - 10, 20, 20, juce::Justification::right);

    plot(drawArea, g, graphColour);
}

void LfoComponent::LfoGraphComponent::plot(juce::Rectangle<int>& drawArea, juce::Graphics& g, juce::Colour lineColour)
{
    juce::Rectangle<int> newDraw = juce::Rectangle<int>{ drawArea.getX(), drawArea.getY(), drawArea.getWidth(), drawArea.getHeight() };

    g.setColour(lineColour);

    const int width = newDraw.getWidth();
    const int right = newDraw.getRight();
    const int left = right - width;
    const int height = newDraw.getHeight();
    const int center = (height / 2);
    const int alignedCenter = newDraw.getBottom() - (drawArea.getHeight() / 2);

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
        else if (tableIndex == 2)
        {
            y1 = alignedCenter + ((sqr(x1 * juce::MathConstants<float>::twoPi / width)) * center);
            y2 = alignedCenter + ((sqr(x2 * juce::MathConstants<float>::twoPi / width)) * center);
        }
        else
        {
            y1 = alignedCenter + ((tri(x1 * juce::MathConstants<float>::pi / width)) * center);
            y2 = alignedCenter + ((tri(x2 * juce::MathConstants<float>::pi / width)) * center);
        }
        g.drawLine(x1 + left, y1, x2 + left, y2, 1.0f);
    }

}

void LfoComponent::paint(juce::Graphics& g)
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
        juce::String text("LFO");
        juce::Colour fillColour = juce::Colours::white;
        g.setColour(fillColour);
        g.setFont(panelNameFont);
        g.drawText(text, bounds.removeFromTop(panelNameHeight).reduced(localMargin), juce::Justification::centred, true);
    }
}

void LfoComponent::resized()
{
    graphComponent.setBounds(10, 42, getWidth() - 20, ((getHeight() * 3) / 5) - 45);
    onButton.setBounds(10, 10, 20, 20);
    waveSelector.setBounds(getLocalBounds().getX() + 10, getLocalBounds().getY() + getLocalBounds().getBottom() - 65, 65, 65);
    frequency.setBounds(waveSelector.getX() + 85, waveSelector.getY(), 65, 65);

    parameterSelect.setBounds(getLocalBounds().getRight() - 195, (getLocalBounds().getBottom() - (55 / 2) - 20), 100, 20);
    paramAmount.setBounds(parameterSelect.getRight() + 10, waveSelector.getY(), 65, 65);
}
