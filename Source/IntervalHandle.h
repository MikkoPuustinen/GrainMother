#pragma once
#include <JuceHeader.h>

class AudioformEvents;

class IntervalHandle : public juce::Component
{
public:
	IntervalHandle(AudioformEvents* par);

	void mouseDown(const juce::MouseEvent& e) override;

	void mouseDrag(const juce::MouseEvent& e) override;

	void mouseUp(const juce::MouseEvent& e) override;

	void paint(juce::Graphics& g) override;

private:
	juce::ComponentDragger dragger;
	juce::ComponentBoundsConstrainer constrains;

	AudioformEvents* par;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IntervalHandle)
};
