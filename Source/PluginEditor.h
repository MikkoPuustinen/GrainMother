/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"

//==============================================================================
/**
*
* 
*
*/

class AudioformComponent : public::juce::Component
    , private::juce::ChangeListener
{
public:
    AudioformComponent(int sourceSamplesPerThumbnailSample,
        juce::AudioFormatManager& formatManager,
        juce::AudioThumbnailCache& cache,
        GrainMotherAudioProcessor& p)
        : thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache), audioProcessor(p), start(3.0f), end(400.0f)
    {
        thumbnail.addChangeListener(this);
    }
    ~AudioformComponent()
    {
        thumbnail.removeChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        if (hasFile())
            paintIfNoFileLoaded(g);
        else
            paintIfFileLoaded(g);
    }
    bool hasFile()
    {
        return thumbnail.getNumChannels() == 0;
    }
    void setFile(const juce::File& file)
    {
        thumbnail.setSource(new juce::FileInputSource(file));
    }
    void paintIfNoFileLoaded(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(246, 244, 243));
        g.setColour(juce::Colours::darkgrey);
        g.drawFittedText("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
    }

    void paintIfFileLoaded(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(246, 244, 243));
        g.setColour(juce::Colour(255, 105, 120));
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
        g.setColour(juce::Colour(230, 57, 70));
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 0.55f);        
    }
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }

private:

    void thumbnailChanged()
    {
        repaint();
    }
    juce::AudioThumbnail thumbnail;
    GrainMotherAudioProcessor& audioProcessor;

    float start;
    float end;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformComponent)

};

/**
*
* Draws the visualization for the individual grains and handles the mouse events happening on the audioform
*/

class GrainVisualizer : public juce::Component
                      , public juce::Timer
                      , public juce::AudioProcessorValueTreeState::Listener
{
public:
    
    GrainVisualizer(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : audioProcessor(p) , drag(0), state(STATE_DRAG), valueTreeState(vts)
    {
        startTimer(75);

        valueTreeState.addParameterListener("interval", this);
        valueTreeState.addParameterListener("duration", this);
        valueTreeState.addParameterListener("readpos", this);
    }

    ~GrainVisualizer()
    {
        valueTreeState.removeParameterListener("interval", this);
        valueTreeState.removeParameterListener("duration", this);
        valueTreeState.removeParameterListener("readpos", this);
    }
    void parameterChanged(const juce::String& parameterID, float newValue) override
    {
        if (parameterID == "interval")
        {
            interval = valueTreeState.getParameter("interval")->convertTo0to1(newValue) * getLocalBounds().getHeight();
        }
        else if (parameterID == "duration") 
        {
            end = start + (newValue * getLocalBounds().getWidth());
            diff = end - start;
        }
        else if (parameterID == "readpos") 
        {
            start = newValue * getLocalBounds().getWidth();
            end = start + diff;
        }
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
            eventStart = event.position.x;
            start = event.position.x;
            end = event.position.x;
        }
        else {
            valueTreeState.getParameter("readpos")->beginChangeGesture();
            state = STATE_MOVE;
        }
    }
    void setProcessorValues()
    {
        float dur2 = (end - start) / getLocalBounds().getWidth();
        const float readpos = start / getLocalBounds().getWidth();
        if (dur2 <= 0)
            dur2 = 0.01;

        valueTreeState.getParameter("duration")->setValueNotifyingHost(dur2);
        valueTreeState.getParameter("readpos")->setValueNotifyingHost(readpos);
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
                if (start < 0) {
                    start = 0;
                }
                if (end > getLocalBounds().getWidth()) {
                    end = getLocalBounds().getWidth();
                }
                if (start == end)
                    end++;

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

                    if (start < 0) {
                        start = 0;
                        end = diff;
                    }
                    if (end > getLocalBounds().getWidth()) {
                        end = getLocalBounds().getWidth();
                        start = getLocalBounds().getWidth() - diff;
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
                    interval -= event.position.y  - drag;
                    if (interval < 1)                            { interval = 1; }
                    if (interval > getLocalBounds().getHeight()) { interval = getLocalBounds().getHeight(); }
                    const float intervalP = (interval) / getLocalBounds().getHeight();
                    valueTreeState.getParameter("interval")->setValueNotifyingHost(intervalP);
                    drag = event.position.y;
                }
                break;
            }
        }
        //repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        float duration = 0;
        duration = end - start;
        if (start >= 0 && duration != 0) {
            g.setColour(juce::Colour(255, 105, juce::uint8(128), juce::uint8(128)));
            g.fillRect((int)start, getLocalBounds().getHeight() - (int)interval + 2, (int)duration, getLocalBounds().getHeight());
            g.setColour(juce::Colour(51, 51, juce::uint8(51)));
            g.fillRoundedRectangle((int)start - 2, 0, 4, getLocalBounds().getHeight(), 1.5f);
            g.fillRoundedRectangle((int)end   - 2, 0, 4, getLocalBounds().getHeight(), 1.5f);
            handleX = end - duration * 0.5f;
            handleY = getLocalBounds().getHeight() - interval;
            g.setColour(juce::Colour(51, 51, juce::uint8(51), juce::uint8(128)));
            g.fillRect((int)start, getLocalBounds().getHeight() - (int)interval - 2, (int)duration, 4);
            g.setColour(juce::Colour(51, 51, 51));
            juce::Point<float> handleP(handleX, handleY);
            juce::Rectangle<float> handle(0, 0, 50, 10);
            g.fillRoundedRectangle(handle.withCentre(handleP), 3.0f);
        }
        g.setColour(juce::Colour(0,181, 142));
        int grainAmount = (int)grains.size() / 2;

        // iterate over the grains and draw them on the audioform
        for (auto&& grain : grains) {
            auto& x = grain.get();
            if (x.direction == 0) { 
                g.setColour(juce::Colour(102, 153, 204));  // right to left
            }
            else {
                g.setColour(juce::Colour(108, 105, 141)); // left to right
            }
            int heightDev;
            if (grainAmount < 1) {
                heightDev = 0;
            } else {
                heightDev = juce::Random::getSystemRandom().nextInt(juce::Range<int>(-1 * grainAmount, grainAmount));
            }
            const float position = x.readPos / audioProcessor.getMaximumSampleCount() * getLocalBounds().getWidth();
            g.fillEllipse(position, getLocalBounds().getHeight() / 2 + heightDev, 10, 10);
        }
    }
    void timerCallback() override
    {
        this->grains = audioProcessor.getGrainPool();
        repaint();
    }
    void initialize()
    {
        auto range = valueTreeState.getParameter("interval")->getNormalisableRange();
        
        interval = valueTreeState.getParameter("interval")->getValue() * getLocalBounds().getHeight();
        start = valueTreeState.getParameter("readpos")->getValue() * getLocalBounds().getWidth();
        end = start + valueTreeState.getParameter("duration")->getValue() * getLocalBounds().getWidth();
        diff = end - start;
        setProcessorValues();
        repaint();
    }

private:
    puro::AlignedPool<Grain> grains;

    GrainMotherAudioProcessor& audioProcessor;
    float drag;

    enum state { STATE_DRAG, STATE_MOVE, STATE_INTERVAL } state;

    float diff;
    float start;
    float end;

    float eventStart;

    float interval;

    juce::AudioProcessorValueTreeState& valueTreeState;

    float handleX;
    float handleY;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer)
};

/*

*/
class GrainMotherAudioProcessorEditor  : public juce::AudioProcessorEditor
                                       , public juce::Button::Listener
                                       , public juce::FileDragAndDropTarget
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~GrainMotherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void paintOverChildren(juce::Graphics& g) override;

    void resized() override;

    void buttonClicked(juce::Button* button) override;


    bool isInterestedInFileDrag(const juce::StringArray& files) override;
    void filesDropped(const juce::StringArray& files, int x, int y) override;
    void fileDragEnter(const juce::StringArray& files, int x, int y) override;
    void fileDragExit(const juce::StringArray& files) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrainMotherAudioProcessor& audioProcessor;

    bool draggingFiles;

    juce::AudioProcessorValueTreeState& valueTreeState;

    juce::TextButton audioFileDialogButton;

    juce::Slider intervalSlider;
    juce::Slider durationSlider;
    juce::Slider panningSlider;
    juce::Slider readposSlider;
    juce::Slider velocitySlider;
    juce::Slider directionSlider;

    juce::Slider fineTuneSlider;
    std::unique_ptr<SliderAttachment> fineTuneAttachment;
    juce::Slider outputSlider;
    std::unique_ptr<SliderAttachment> outputAttachment;

    std::unique_ptr<SliderAttachment> intervalAttachment;
    std::unique_ptr<SliderAttachment> durationAttachment;
    std::unique_ptr<SliderAttachment> panningAttachment;
    std::unique_ptr<SliderAttachment> readposAttachment;
    std::unique_ptr<SliderAttachment> velocityAttachment;
    std::unique_ptr<SliderAttachment> directionAttachment;

    juce::Slider intervalRandSlider;
    juce::Slider durationRandSlider;
    juce::Slider panningRandSlider;
    juce::Slider readposRandSlider;
    juce::Slider velocityRandSlider;

    std::unique_ptr<SliderAttachment> intervalRandAttachment;
    std::unique_ptr<SliderAttachment> durationRandAttachment;
    std::unique_ptr<SliderAttachment> panningRandAttachment;
    std::unique_ptr<SliderAttachment> readposRandAttachment;
    std::unique_ptr<SliderAttachment> velocityRandAttachment;


    juce::Label tuneLabel;
    juce::Label directionLabel;
    juce::Label randomPanningLabel;
    juce::Label randomReadposLabel;
    juce::Label outputLabel;

    juce::Label header;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    AudioformComponent audioformComponent;

    GrainVisualizer grainVisualizer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};


