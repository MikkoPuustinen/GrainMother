#pragma once
#include <JuceHeader.h>

#include "IntervalHandle.h"
class AudioformEvents;

class AudioformHandle : public juce::Component
{
public:
    AudioformHandle(AudioformEvents* par);
    void mouseDown(const juce::MouseEvent& e) override;

    void mouseDrag(const juce::MouseEvent& e) override;

    void mouseMove(const juce::MouseEvent& e) override;

    juce::MouseCursor getMouseCursor() override;

    void resized() override;

    void paint(juce::Graphics& g) override;

    void setInterval();

private:

    juce::Rectangle<float> intervalHandle;

    AudioformEvents* par;
    enum state { STATE_NONE, STATE_INTERVAL, STATE_MOVE, STATE_LEFT, STATE_RIGHT } state;
    bool draggingComponent;
    juce::MouseCursor cursor;
    float interval;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformHandle)

};