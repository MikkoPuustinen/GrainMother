#pragma once
#include <JuceHeader.h>
class AudioformEvents;

class AudioformHandle : public juce::Component
{
public:
    AudioformHandle(AudioformEvents* par);
    void mouseDown(const juce::MouseEvent& e) override;

    void mouseDrag(const juce::MouseEvent& e) override;

    void paint(juce::Graphics& g) override;

    void setInterval(const float& interval);

private:

    juce::ComponentDragger dragger;
    juce::ComponentBoundsConstrainer constrains;

    AudioformEvents* par;

    float interval;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformHandle)

};