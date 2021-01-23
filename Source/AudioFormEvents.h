#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AudioFormHandle.h"

class AudioformEvents : public juce::Component
                      , public juce::Timer
{
public:
    AudioformEvents(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) :
        audioProcessor(p), valueTreeState(vts), state(STATE_DRAG), selectableRange(0, 0, 910, 1), events(this)
    {
        addAndMakeVisible(events);
        events.addMouseListener(this, false);
        startTimer(40);
    }
    ~AudioformEvents()
    {
        events.removeMouseListener(this);
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (selectableRange.contains(e.getPosition().getX(), e.getPosition().getY()))
        {
            state = STATE_NONE;
        }
        else if (left.contains(e.getPosition().getX(), e.getPosition().getY()))
        {
            state = STATE_LEFT;
        }
        else if (right.contains(e.getPosition().getX(), e.getPosition().getY()))
        {
            state = STATE_RIGHT;
        }
        else {
            state = STATE_DRAG;
            valueTreeState.getParameter("duration")->beginChangeGesture();
            valueTreeState.getParameter("readpos")->beginChangeGesture();
            eventStart = e.position.x;
        }
    }
    void changeStart(const float newStart) 
    {
        start = newStart;
        const float e = (getReadpos() + getDuration()) * getLocalBounds().getWidth() / getLocalBounds().getWidth();
        const float s = newStart / getLocalBounds().getWidth();
        valueTreeState.getParameter("duration")->setValueNotifyingHost(e - s);
        valueTreeState.getParameter("readpos")->setValueNotifyingHost(s);
    }
    void changeEnd(const float newEnd)
    {
        end = newEnd;
        const float s = getReadpos() * getLocalBounds().getWidth() / getLocalBounds().getWidth();
        const float e = newEnd / getLocalBounds().getWidth();
        valueTreeState.getParameter("duration")->setValueNotifyingHost(e - s);
    }
    void changeReadpos(const float newPos)
    {
        end = end - start + newPos;
        start = newPos;
        const float rPos = newPos / getLocalBounds().getWidth();
        valueTreeState.getParameter("readpos")->setValueNotifyingHost(rPos);
    }
    void changeInterval(const float interval)
    {
        const float tInterval = getLocalBounds().getHeight() - interval;
        const float intervalP = tInterval / getLocalBounds().getHeight();
        valueTreeState.getParameter("interval")->setValueNotifyingHost(intervalP);
    }
    void setProcessorValues(float e, float s)
    {
        float dur2 = (e - s) / (getLocalBounds().getWidth());
        const float readpos = (s) / (getLocalBounds().getWidth());
        if (dur2 <= 0)
            dur2 = 0.001f;

        valueTreeState.getParameter("duration")->setValueNotifyingHost(dur2);
        valueTreeState.getParameter("readpos")->setValueNotifyingHost(readpos);
        //audioProcessor.setDuration(dur2);
        //audioProcessor.setReadpos(readpos);
    }
    void resized() override
    {
        events.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
        left.setBounds(start - 10, 0, 10, getLocalBounds().getHeight());
        right.setBounds(end, 0, 10, getLocalBounds().getHeight());
    }

    void timerCallback() override
    {
        this->intervalY = getLocalBounds().getHeight() * valueTreeState.getParameter("interval")->getValue();
        this->readpos = valueTreeState.getParameter("readpos")->getValue();
        this->duration = valueTreeState.getParameter("duration")->getValue();
        const float s = getLocalBounds().getWidth() * valueTreeState.getParameter("readpos")->getValue();
        const float e = getLocalBounds().getWidth() * valueTreeState.getParameter("duration")->getValue();
        events.setBounds(0, 0, getLocalBounds().getWidth(), getLocalBounds().getHeight());
        events.setInterval();
        selectableRange.setBounds(s, 0, e, getLocalBounds().getHeight());
        left.setBounds(s - 10, 0, 10, getLocalBounds().getHeight());
        right.setBounds(e + s, 0, 10, getLocalBounds().getHeight());
        events.repaint();
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
        }
    }
    void mouseDrag(const juce::MouseEvent& event) override
    {
        switch (state)
        {
        case STATE_DRAG:
        {
            const float x = event.position.x;
            float s = 0;
            float e = 0;
            if (x < eventStart) {
                s = x;
                e = eventStart;
            }
            else {
                e = x;
                s = eventStart;
            }
            setProcessorValues(e, s);
            break;
        }
        }
    }
    void paint(juce::Graphics& g) override
    {
    }

    float getInterval()
    {
        return intervalY;
    }
    float getReadpos()
    {
        return readpos;
    }
    float getDuration()
    {
        return duration;
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    GrainMotherAudioProcessor& audioProcessor;

    AudioformHandle events;

    juce::Rectangle<float> left;
    juce::Rectangle<float> right;

    juce::Rectangle<float> selectableRange;

    enum state { STATE_DRAG, STATE_MOVE, STATE_INTERVAL, STATE_NONE, STATE_LEFT, STATE_RIGHT } state;
    float end;
    float start;

    float intervalY;
    float readpos;
    float duration;

    float eventStart;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformEvents)
};