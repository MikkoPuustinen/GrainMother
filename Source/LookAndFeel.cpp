#include "LookAndFeel.h"


GrainMotherSliderLookAndFeel& GrainMotherSliderLookAndFeel::getInstance()
{
	static GrainMotherSliderLookAndFeel instance;
	return instance;
}

void GrainMotherSliderLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
	const float rotaryStartAngle, const float rotaryEndAngle, ::juce::Slider& slider)
{

}