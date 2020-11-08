/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

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

    void paint(juce::Graphics& g) override
    {
        if (hasFile())
            paintIfNoFileLoaded(g);
        else
            paintIfFileLoaded(g);
    }
    boolean hasFile()
    {
        return thumbnail.getNumChannels() == 0;
    }
    void setFile(const juce::File& file)
    {
        thumbnail.setSource(new juce::FileInputSource(file));
        auto multiplier = audioProcessor.getMaximumPosition();
        audioProcessor.setReadpos(start / getLocalBounds().getWidth());
        const float dur2 = (end - start) / getLocalBounds().getWidth();
        audioProcessor.setDuration(dur2);
    }
    void paintIfNoFileLoaded(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::white);
        g.setColour(juce::Colours::darkgrey);
        g.drawFittedText("No File Loaded", getLocalBounds(), juce::Justification::centred, 1);
    }

    void paintIfFileLoaded(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::white);
        g.setColour(juce::Colour(0,102,102));
        thumbnail.drawChannels(g, getLocalBounds(), 0.0, thumbnail.getTotalLength(), 1.0f);
        g.setColour(juce::Colour(17, 173, 173));
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

    //float multiplier;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformComponent)

};

class GrainVisualizer : public juce::Component
                      , public juce::Timer
{
public:
    GrainVisualizer(GrainMotherAudioProcessor& p) : audioProcessor(p) 
    {
        startTimer(50);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        start = event.position.x;
        auto multiplier = audioProcessor.getMaximumPosition();
        const float rpos = start / getLocalBounds().getWidth() * multiplier;
        audioProcessor.setReadpos(start / getLocalBounds().getWidth() * multiplier);
        repaint();
    }
    void mouseDrag(const juce::MouseEvent& event) override
    {
        end = event.position.x;
        auto multiplier = audioProcessor.getMaximumPosition();
        if (start > end) { // dragging from right to left
            audioProcessor.setReadpos(end / getLocalBounds().getWidth() * multiplier);
            audioProcessor.setDuration((start - end) / getLocalBounds().getWidth() * multiplier);
        }
        else { // dragging from left to right
            const float dur2 = (end - start) / getLocalBounds().getWidth() * multiplier;
            audioProcessor.setDuration(dur2);
        }

        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        float duration = 0;
        if (start > end) {
            duration = start - end;
            if (start > 0 && duration > 0) {
                g.setColour(juce::Colour(150, 255, juce::uint8(248), juce::uint8(128)));
                g.fillRect((int)end, 0, (int)duration, getLocalBounds().getHeight());
            }
        } else {
            duration = end - start;
            if (start > 0 && duration > 0) {
                g.setColour(juce::Colour(150, 255, juce::uint8(248), juce::uint8(128)));
                g.fillRect((int)start, 0, (int)duration, getLocalBounds().getHeight());
            }
        }
        g.setColour(juce::Colour(0,181, 142));
        int grainAmount = (int)grains.size() / 2;
        for (auto&& grain : grains) {
            auto& x = grain.get();
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

private:
    puro::AlignedPool<Grain> grains;

    GrainMotherAudioProcessor& audioProcessor;


    float start;
    float end;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GrainVisualizer)
};

/*

*/
class GrainMotherAudioProcessorEditor  : public juce::AudioProcessorEditor
                                       , public juce::Button::Listener
                                       , public juce::Timer
{
public:
    GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor&);
    ~GrainMotherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void buttonClicked(juce::Button* button) override;

    void timerCallback() override;

    //formatManager.registerBasicFormats();


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GrainMotherAudioProcessor& audioProcessor;

    juce::TextButton audioFileDialogButton;

    juce::Slider intervalSlider;
    juce::Slider durationSlider;
    juce::Slider panningSlider;
    juce::Slider readposSlider;
    juce::Slider velocitySlider;

    juce::Slider intervalRandSlider;
    juce::Slider durationRandSlider;
    juce::Slider panningRandSlider;
    juce::Slider readposRandSlider;
    juce::Slider velocityRandSlider;

    juce::Label intervalLabel;
    juce::Label durationLabel;
    juce::Label panningLabel;
    juce::Label readposLabel;
    juce::Label velocityLabel;
    juce::Label randomLabel;

    juce::Label activeGrainsLabel;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    AudioformComponent audioformComponent;

    GrainVisualizer grainVisualizer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};


