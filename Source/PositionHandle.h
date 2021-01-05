#pragma once

#include <JuceHeader.h>
class AudioformEvents;


class PositionHandle : public juce::Component
{
public:
    PositionHandle(AudioformEvents* par, bool left);

    void mouseDown(const juce::MouseEvent& e) override;

    void mouseDrag(const juce::MouseEvent& e) override;

    void paint(juce::Graphics& g) override;

private:
    juce::ComponentDragger dragger;
    juce::ComponentBoundsConstrainer constrains;
    bool left;

    AudioformEvents* par;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PositionHandle)
};