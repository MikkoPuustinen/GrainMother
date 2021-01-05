/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "LookAndFeel.h"
#include "AudioFormEvents.h"

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
        setBufferedToImage(true);
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
* Draws the visualization for the individual grains
*/

class GrainVisualizer : public juce::Component
                      , public juce::Timer
{
public:
    
    GrainVisualizer(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : audioProcessor(p)
    {
        startTimer(60);
    }

    ~GrainVisualizer()
    {
    }
    void paint(juce::Graphics& g) override
    {
        int maxDev = (int)grains.size() * 0.25f;

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
            if (maxDev < 1) {
                heightDev = 0;
            } else {
                heightDev = juce::Random::getSystemRandom().nextInt(juce::Range<int>(-1 * maxDev, maxDev));
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

    juce::Slider velocitySlider;
    juce::Slider directionSlider;

    juce::Slider fineTuneSlider;
    std::unique_ptr<SliderAttachment> fineTuneAttachment;
    juce::Slider outputSlider;
    std::unique_ptr<SliderAttachment> outputAttachment;

    std::unique_ptr<SliderAttachment> velocityAttachment;
    std::unique_ptr<SliderAttachment> directionAttachment;

    juce::Slider panningRandSlider;
    juce::Slider readposRandSlider;

    std::unique_ptr<SliderAttachment> panningRandAttachment;
    std::unique_ptr<SliderAttachment> readposRandAttachment;

    juce::Label tuneLabel;
    juce::Label directionLabel;
    juce::Label randomPanningLabel;
    juce::Label randomReadposLabel;
    juce::Label outputLabel;

    juce::Label header;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    AudioformComponent audioformComponent;
    AudioformEvents audioformEvents;
    GrainVisualizer grainVisualizer;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};


