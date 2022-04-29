/*
  ==============================================================================

    ScopeComponent.h
    Created: 10 Apr 2022 12:15:21am
    Author:  lukeb_000

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include <array>

//==============================================================================
template <typename SampleType>
class AudioBufferQueue
{
public:
	static constexpr size_t order = 9;					
	static constexpr size_t bufferSize = 1U << order;		
	static constexpr size_t numBuffers = 5;					

	void push(const SampleType* dataToPush, size_t numSamples)
	{
		jassert(numSamples <= bufferSize);
		int start1, size1, start2, size2;

		abstractFifo.prepareToWrite(1, start1, size1, start2, size2);

		jassert(size1 <= 1);
		jassert(size2 == 0);

		if (size1 > 0) {
			juce::FloatVectorOperations::copy(buffers[(size_t)start1].data(), dataToPush, (int)juce::jmin(bufferSize, numSamples));
		}

		abstractFifo.finishedWrite(size1);
	}


	void pop(SampleType* outputBuffer)
	{
		int start1, size1, start2, size2;

		abstractFifo.prepareToRead(1, start1, size1, start2, size2);

		jassert(size1 <= 1);
		jassert(size2 == 0);

		if (size1 > 0) {
			juce::FloatVectorOperations::copy(outputBuffer, buffers[(size_t)start1].data(), (int)bufferSize);
		}

		abstractFifo.finishedRead(size1);
	}

private:
	std::array <std::array<SampleType, bufferSize>, numBuffers> buffers;

	juce::AbstractFifo abstractFifo{ numBuffers };

};

//==============================================================================
template<typename SampleType>
class ScopeDataCollector
{
public:
	ScopeDataCollector(AudioBufferQueue<SampleType>& queueToUse)
		: audioBufferQueue(queueToUse)
	{}

	void process(const SampleType* data, size_t numSamples)
	{
		size_t index = 0;

		if (currentState == State::WaitingForTrigger)
		{
			while (index++ < numSamples)
			{
				auto currentSample = *data++;
				if (currentSample >= triggerLevel && prevSample < triggerLevel)
				{
					numCollected = 0;						
					currentState = State::Collecting;	
					break;										
				}
				prevSample = currentSample;				

			}	
		}	

		if (currentState == State::Collecting)
		{
			while (index++ < numSamples)
			{
				buffer[numCollected++] = *data++;

				if (numCollected == buffer.size())
				{
					audioBufferQueue.push(buffer.data(), buffer.size());	
					currentState = State::WaitingForTrigger;				
					prevSample = SampleType(100);							
					break;													
				}
			}
		}

	}	

private:
	enum class State
	{
		WaitingForTrigger,
		Collecting
	};

	static constexpr auto triggerLevel = SampleType(0.001);

	std::array<SampleType, AudioBufferQueue<SampleType>::bufferSize> buffer;	
	State currentState{ State::WaitingForTrigger };								
	AudioBufferQueue<SampleType>& audioBufferQueue; 							
	size_t numCollected;														
	SampleType prevSample = SampleType(100);									
};

//==============================================================================
template<typename SampleType>
class ScopeComponent : public juce::Component, private juce::Timer
{
public:

	using Queue = AudioBufferQueue<SampleType>;

	ScopeComponent(Queue& queueuToUse)
		: audioBufferQueue(queueuToUse)
	{
		sampleData.fill(SampleType(0));
		setFramePerSecond(60);

		setOpaque(true);
	}

	~ScopeComponent()
	{
	}

	void setFramePerSecond(int framePerSecond)
	{
		jassert(framePerSecond > 0 && framePerSecond < 1000);
		startTimerHz(framePerSecond);
	}

	void paint(juce::Graphics& g) override
	{
		int panelNameHeight = 20;
		int localMargin = 2;
		juce::Font panelNameFont = juce::Font(16.0f, juce::Font::plain).withTypefaceStyle("Italic");

		{
			float x = 0.0f, y = 0.0f, width = (float)getWidth(), height = (float)getHeight();
			juce::Colour panelColour = juce::Colour(36, 36, 36);
			g.setColour(panelColour);
			g.fillRoundedRectangle(x, y, width, height, 10.0f);
		}

		{
			juce::Rectangle<int> bounds = getLocalBounds();
			juce::String text("Output");
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

		drawArea.reduce(20, 10);

		SampleType drawX = (SampleType)drawArea.getX();
		SampleType drawY = (SampleType)drawArea.getY();
		SampleType drawH = (SampleType)drawArea.getHeight();
		SampleType drawW = (SampleType)drawArea.getWidth();
		juce::Rectangle<SampleType> scopeRect = juce::Rectangle<SampleType>{ drawX, drawY, drawW, drawH };

		//g.setColour(juce::Colours::cyan);

		plot(sampleData.data(), sampleData.size(), g, scopeRect, SampleType(0.4), scopeRect.getHeight() / 2);

	}

	void resized() override {}

private:
	void drawGraph(juce::Graphics& g, juce::Rectangle<int> drawArea)
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

		g.setColour(graphColour);
	}

	void timerCallback() override
	{
		audioBufferQueue.pop(sampleData.data());			
		repaint();											
	}

	static void plot(const SampleType* data
		, size_t numSamples
		, juce::Graphics& g
		, juce::Rectangle<SampleType> rect
		, SampleType scaler = SampleType(1)
		, SampleType offset = SampleType(0))
	{
		auto w = rect.getWidth();							
		auto h = rect.getHeight();								
		auto right = rect.getRight();						
		auto alignedCentre = rect.getBottom() - offset;		
		auto gain = h * scaler;									


		for (size_t i = 1; i < numSamples; ++i)
		{
			const float x1 = juce::jmap(SampleType(i - 1), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
			const float y1 = alignedCentre - gain * juce::jmax(SampleType(-1.0), juce::jmin(SampleType(1.0), data[i - 1]));
			const float x2 = juce::jmap(SampleType(i), SampleType(0), SampleType(numSamples - 1), SampleType(right - w), SampleType(right));
			const float y2 = alignedCentre - gain * juce::jmax(SampleType(-1.0), juce::jmin(SampleType(1.0), data[i]));
			const float t = 1.0f;
			g.drawLine(x1, y1, x2, y2, t);
		}
	}

	Queue& audioBufferQueue;										
	std::array<SampleType, Queue::bufferSize> sampleData;			

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeComponent)
};
