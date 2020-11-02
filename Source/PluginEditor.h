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
        juce::AudioThumbnailCache& cache)
        : thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache)

    {
        thumbnail.addChangeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
        if (thumbnail.getNumChannels() == 0)
            paintIfNoFileLoaded(g);
        else
            paintIfFileLoaded(g);
    }
    void setFile(const juce::File& file)
    {
        thumbnail.setSource(new juce::FileInputSource(file));
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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformComponent)

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};


