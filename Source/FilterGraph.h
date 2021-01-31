#pragma once


#include <JuceHeader.h>
#include "PluginProcessor.h"

class FilterGraph : public juce::Component, public juce::Timer
{
public:
	FilterGraph(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);

	void paint(juce::Graphics& g) override;

	void mouseDown(const juce::MouseEvent& e) override;

	void mouseDrag(const juce::MouseEvent& e) override;

	void timerCallback() override;

private:
	GrainMotherAudioProcessor& audioProcessor;
	juce::AudioProcessorValueTreeState& valueTreeState;

	juce::Path frequencyResponse;
	float freq;
	float resonance;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FilterGraph)
};