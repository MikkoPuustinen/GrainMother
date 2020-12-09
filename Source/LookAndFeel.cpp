#include "LookAndFeel.h"

using namespace juce;


GrainMotherSliderLookAndFeel::GrainMotherSliderLookAndFeel()
{
    setColour(juce::Slider::trackColourId, juce::Colour(230,57,70));
    setColour(juce::Slider::backgroundColourId, juce::Colour(246, 244, 243));
    setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(230, 57, 70));
    setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(255, 105, 120));
    setColour(juce::Slider::thumbColourId, juce::Colour(51, 51, 51));
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0.0f, 0.0f, 0.0f, 0.0f));
    setColour(juce::Slider::textBoxTextColourId, juce::Colour(51, 51, 51));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(246, 244, 243));
    setColour(juce::Slider::textBoxHighlightColourId, juce::Colour(246, 244, 243));
    setColour(juce::Label::textColourId, juce::Colour(51, 51, 51));
    setColour(juce::TextButton::buttonColourId, juce::Colour(51, 51, 51));
    setColour(juce::TextButton::buttonOnColourId, juce::Colour(230, 57, 70));

}

GrainMotherSliderLookAndFeel& GrainMotherSliderLookAndFeel::getInstance()
{
    static GrainMotherSliderLookAndFeel instance;
    return instance;
}

Typeface::Ptr GrainMotherSliderLookAndFeel::getTypefaceForFont(const Font& f)
{
    static Typeface::Ptr myFont = Typeface::createSystemTypefaceFor(BinaryData::ComfortaaLight_ttf,
        BinaryData::ComfortaaLight_ttfSize);
    return myFont;
}

void GrainMotherSliderLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider)
{
    FillType fillType;
    auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
    auto fill = slider.findColour(Slider::rotarySliderFillColourId);

    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = jmin(8.0f, radius * 0.5f);
    auto arcRadiusB = radius - lineW * 0.05f;
    auto arcRadius = radius - lineW * 0.5f;
    Point<float> knobPoint(bounds.getCentreX(), bounds.getCentreY());

    Rectangle<float> minR(2, 10);
    Rectangle<float> maxR(2, 10);
    const float offset = radius + 5;

    Point<float> minPoint(bounds.getCentreX() + offset * std::cos(rotaryStartAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + offset * std::sin(rotaryStartAngle - MathConstants<float>::halfPi));
    Point<float> maxPoint(bounds.getCentreX() + offset * std::cos(rotaryEndAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + offset * std::sin(rotaryEndAngle - MathConstants<float>::halfPi));

    AffineTransform t1 = AffineTransform::rotation(rotaryStartAngle, minPoint.getX(), minPoint.getY());
    AffineTransform t2 = AffineTransform::rotation(rotaryEndAngle, maxPoint.getX(), maxPoint.getY());

    Path min, max;
    fillType.setColour(slider.findColour(Slider::thumbColourId));
    min.addRectangle(minR.withCentre(minPoint));
    max.addRectangle(maxR.withCentre(maxPoint));
    g.fillPath(min, t1);
    g.fillPath(max, t2);

    g.setColour(outline);
    g.fillEllipse(Rectangle<float>(bounds.getWidth() + 8, bounds.getWidth() + 8).withCentre(knobPoint));
    g.setColour(fill);
 
    
    g.fillEllipse(Rectangle<float>(bounds.getWidth(), bounds.getWidth()).withCentre(knobPoint));

    auto thumbWidth = lineW * 2.0f;
    Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
        bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));

    g.setColour(slider.findColour(Slider::thumbColourId));
    Rectangle<float> thumbRect(thumbWidth * 0.5f, thumbWidth);
    AffineTransform t = AffineTransform::rotation(toAngle, thumbPoint.getX(), thumbPoint.getY());
    Path p; 
    p.addRectangle(thumbRect.withCentre(thumbPoint));
    p.closeSubPath();
    
    fillType.setColour(fill);
    g.fillPath(p, t);


}

void GrainMotherSliderLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, float minSliderPos, float maxSliderPos,
    const Slider::SliderStyle style, Slider& slider)
{
    auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
    auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

    auto trackWidth = jmin(4.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

    Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(slider.findColour(Slider::backgroundColourId));
    g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    Path valueTrack;
    Point<float> minPoint, maxPoint, thumbPoint;

    if (isTwoVal || isThreeVal)
    {
        minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
                        slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

        if (isThreeVal)
            thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
                            slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

        maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
                        slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
    }
    else
    {
        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

        minPoint = startPoint;
        maxPoint = { kx, ky };
    }

    auto thumbWidth = getSliderThumbRadius(slider);

    valueTrack.startNewSubPath(minPoint);
    valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);
    g.setColour(slider.findColour(Slider::trackColourId));
    g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

    g.setColour(slider.findColour(Slider::thumbColourId));
    g.fillRoundedRectangle(Rectangle<float>(static_cast<float> (thumbWidth * 4), static_cast<float> (thumbWidth * 0.5f)).withCentre(maxPoint), 2);
}

Label* GrainMotherSliderLookAndFeel::createSliderTextBox(Slider& slider)
{
    auto* l = LookAndFeel_V4::createSliderTextBox(slider);
    l->setColour(TextEditor::focusedOutlineColourId, slider.findColour(Slider::textBoxOutlineColourId));
    l->setColour(TextEditor::highlightedTextColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(TextEditor::highlightColourId, slider.findColour(Slider::backgroundColourId));
    l->setColour(TextEditor::textColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setColour(Label::textWhenEditingColourId, slider.findColour(Slider::textBoxTextColourId));
    l->setFont(24.0f);
    return l;
}

void GrainMotherSliderLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button, const juce::Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);
    auto bColor = shouldDrawButtonAsHighlighted ? button.findColour(TextButton::buttonOnColourId) : button.findColour(TextButton::buttonColourId);

    g.setColour(bColor);
    Point<float> centre(bounds.getWidth() * 0.5f, bounds.getHeight() * 0.5f);
    Rectangle<float> h(centre.getX(), centre.getY(), bounds.getWidth() * 0.5f, 5.0f);
    g.fillRoundedRectangle(h.withCentre(centre), 2.0f);
    Rectangle<float> v(centre.getX(), centre.getY(), 5.0f, bounds.getWidth() * 0.5f);
    g.fillRoundedRectangle(v.withCentre(centre), 2.0f);
}

void GrainMotherSliderLookAndFeel::drawButtonText(juce::Graphics&, juce::TextButton&, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
}

