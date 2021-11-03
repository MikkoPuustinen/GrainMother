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
#include "FilterGraph.h"
#include "AudioformComponent.h"

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
                g.setColour(juce::Colour((juce::uint8)102, (juce::uint8)153, (juce::uint8)204, 0.7f));  // right to left
            }
            else {
                g.setColour(juce::Colour((juce::uint8)108, (juce::uint8)105, (juce::uint8)141, 0.7f)); // left to right
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
                                       , public AudioformEvents::Listener
{
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~GrainMotherAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void paintOverChildren(juce::Graphics& g) override;

    void resized() override;

    void onZoomChange(double start, double end) override;

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

    juce::Slider resonanceSlider;
    juce::Slider filterFreqSlider;
    std::unique_ptr<SliderAttachment> resonanceAttachment;
    std::unique_ptr<SliderAttachment> filterFreqAttachment;

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

    juce::ComboBox filterCombo;
    std::unique_ptr<ComboBoxAttachment> filterComboAttachment;

    juce::Label tuneLabel;
    juce::Label directionLabel;
    juce::Label randomPanningLabel;
    juce::Label randomReadposLabel;
    juce::Label outputLabel;

    juce::Label cutoffLabel;
    juce::Label resonanceLabel;

    juce::Label header;

    juce::AudioFormatManager formatManager;

    juce::AudioThumbnailCache thumbnailCache;

    FilterGraph filterGraph;
    AudioformComponent audioformComponent;
    AudioformEvents audioformEvents;
    GrainVisualizer grainVisualizer;

    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};


