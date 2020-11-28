#pragma once

#include <JuceHeader.h>

class GrainMotherSliderLookAndFeel : public juce::LookAndFeel_V4
{
public:
	static GrainMotherSliderLookAndFeel& getInstance();

	void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
		const float rotaryStartAngle, const float rotaryEndAngle, ::juce::Slider& slider) override;
};