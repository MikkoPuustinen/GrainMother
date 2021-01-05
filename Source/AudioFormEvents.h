#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AudioFormHandle.h"
#include "PositionHandle.h"


class AudioformEvents : public juce::Component
    , public juce::Timer
{
public:
    AudioformEvents(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : drag(0),
        audioProcessor(p), valueTreeState(vts), state(STATE_DRAG), events(this), left(this,true), right(this, false)
    {
        addAndMakeVisible(events);
        addAndMakeVisible(left);
        addAndMakeVisible(right);
        startTimer(40);
    }
    ~AudioformEvents()
    {
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (std::abs(event.position.x - handleX) < 15 && std::abs(event.position.y - handleY) < 15) {
            state = STATE_INTERVAL;
            valueTreeState.getParameter("interval")->beginChangeGesture();
        }
        else if (!(event.position.x > start && event.position.x < end)) {
            state = STATE_DRAG;
            valueTreeState.getParameter("duration")->beginChangeGesture();
            valueTreeState.getParameter("readpos")->beginChangeGesture();
            eventStart = event.position.x + 5;
            start = event.position.x + 5;
            end = event.position.x + 5;
        }
        else {
            valueTreeState.getParameter("readpos")->beginChangeGesture();
            state = STATE_MOVE;
        }
    }
    void changeStart(const float newStart) {
        this->start = newStart;
        const float e = end / getLocalBounds().getWidth();
        const float s = newStart / getLocalBounds().getWidth();
        valueTreeState.getParameter("duration")->setValueNotifyingHost(e - s);
        valueTreeState.getParameter("readpos")->setValueNotifyingHost(s);
    }
    void changeEnd(const float newEnd) {
        this->end = newEnd;
        const float s = start / getLocalBounds().getWidth();
        const float e = newEnd / getLocalBounds().getWidth();
        valueTreeState.getParameter("duration")->setValueNotifyingHost(e - s);
    }
    void setProcessorValues()
    {
        float dur2 = (end - start) / (getLocalBounds().getWidth() - 10);
        const float readpos = (start - 5) / (getLocalBounds().getWidth() - 10);
        if (dur2 <= 0)
            dur2 = 0.001f;

        valueTreeState.getParameter("duration")->setValue(dur2);
        valueTreeState.getParameter("readpos")->setValue(readpos);
        audioProcessor.setDuration(dur2);
        audioProcessor.setReadpos(readpos);
    }
    void resized() override
    {
        events.setBounds(start, 0, end - start, getLocalBounds().getHeight());
        left.setBounds(start - 10, 0, 10, getLocalBounds().getHeight());
        right.setBounds(end, 0, 10, getLocalBounds().getHeight());
    }

    void timerCallback() override
    {
        events.setInterval(getLocalBounds().getHeight() * valueTreeState.getParameter("interval")->getValue());
        const float s = getLocalBounds().getWidth() * valueTreeState.getParameter("readpos")->getValue();
        const float e = getLocalBounds().getWidth() * valueTreeState.getParameter("duration")->getValue();
        events.setBounds(s, 0, e, getLocalBounds().getHeight());
        // TODO try to just change position
        left.setBounds(s - 10, 0, 10, getLocalBounds().getHeight());
        right.setBounds(e + s, 0, 10, getLocalBounds().getHeight());
    }

    void mouseUp(const juce::MouseEvent& event) override
    {
        switch (state)
        {
        case STATE_DRAG:
        {
            valueTreeState.getParameter("duration")->endChangeGesture();
            valueTreeState.getParameter("readpos")->endChangeGesture();
            break;
        }
        case STATE_MOVE:
        {
            valueTreeState.getParameter("readpos")->endChangeGesture();
            break;
        }
        case STATE_INTERVAL:
        {
            valueTreeState.getParameter("interval")->endChangeGesture();
            break;
        }
        }
        drag = 0;
    }
    void mouseDrag(const juce::MouseEvent& event) override
    {
        switch (state)
        {
        case STATE_DRAG:
        {
            const float x = event.position.x;
            if (x < eventStart) {
                start = x;
                end = eventStart;
            }
            else {
                end = x;
                start = eventStart;
            }
            if (start <= 5) {
                start = 5;
            }
            if (end >= getLocalBounds().getWidth() - 5) {
                end = getLocalBounds().getWidth() - 5;
            }


            diff = end - start;
            setProcessorValues();
            break;
        }
        case STATE_MOVE:
        {
            if (drag == 0) {
                drag = event.position.x;
            }
            else {
                start += event.position.x - drag;
                end += event.position.x - drag;

                if (start <= 5) {
                    start = 5;
                    end = start + diff;
                }
                if (end >= getLocalBounds().getWidth() - 5) {
                    end = getLocalBounds().getWidth() - 5;
                    start = getLocalBounds().getWidth() - diff - 5;
                }
                drag = event.position.x;
            }
            setProcessorValues();
            break;
        }
        case STATE_INTERVAL:
        {
            if (drag == 0) {
                drag = event.position.y;
            }
            else {
                interval -= event.position.y - drag;
                if (interval < 1) { interval = 1; }
                if (interval > getLocalBounds().getHeight()) { interval = getLocalBounds().getHeight(); }
                const float intervalP = (interval) / getLocalBounds().getHeight();
                valueTreeState.getParameter("interval")->setValueNotifyingHost(intervalP);
                drag = event.position.y;
            }
            break;
        }
        }
        repaint();
    }
    void paint(juce::Graphics& g) override
    {

    }
    void initialize()
    {
        auto range = valueTreeState.getParameter("interval")->getNormalisableRange();

        interval = valueTreeState.getParameter("interval")->getValue() * getLocalBounds().getHeight();
        start = valueTreeState.getParameter("readpos")->getValue() * getLocalBounds().getWidth() + 5;
        end = start + valueTreeState.getParameter("duration")->getValue() * getLocalBounds().getWidth();
        diff = end - start;
        setProcessorValues();
        repaint();
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    GrainMotherAudioProcessor& audioProcessor;

    AudioformHandle events;

    PositionHandle left;
    PositionHandle right;

    enum state { STATE_DRAG, STATE_MOVE, STATE_INTERVAL } state;
    float end;
    float start;
    float interval;
    float diff;

    float handleX;
    float handleY;
    float drag;
    float eventStart;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformEvents)
};