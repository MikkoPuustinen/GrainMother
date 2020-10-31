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



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessorEditor)
};
