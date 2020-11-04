/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrainMotherAudioProcessorEditor::GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor& p)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , audioFileDialogButton("Load audio file")
    , intervalSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , durationSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , panningSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , readposSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , velocitySlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)

    , intervalRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , durationRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , panningRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , readposRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , velocityRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , thumbnailCache(5)
    , audioformComponent(1024, formatManager, thumbnailCache, p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    addAndMakeVisible(&audioformComponent);
    formatManager.registerBasicFormats();

    setSize(800, 650);
    addAndMakeVisible(audioFileDialogButton);
    audioFileDialogButton.addListener(this);

    addAndMakeVisible(activeGrainsLabel);
    startTimerHz(30);

    addAndMakeVisible(intervalSlider);
    intervalSlider.setRange(0.1, 5000);
    intervalSlider.setSkewFactorFromMidPoint(1);
    intervalSlider.onValueChange = [this] {
        audioProcessor.setInterval((float)intervalSlider.getValue());
    };
    intervalSlider.setValue(1.0);

    addAndMakeVisible(durationSlider);
    durationSlider.setRange(1, 88200);
    durationSlider.onValueChange = [this] {
        //audioProcessor.setDuration((float)durationSlider.getValue());
    };
    durationSlider.setValue(44100);

    addAndMakeVisible(panningSlider);
    panningSlider.setRange(-1, 1);
    panningSlider.onValueChange = [this] {
        audioProcessor.setPanning((float)panningSlider.getValue());
    };
    panningSlider.setValue(0);

    addAndMakeVisible(readposSlider);
    readposSlider.setRange(0, 1);
    readposSlider.onValueChange = [this] {
        audioProcessor.setReadpos((float)readposSlider.getValue());
    };
    readposSlider.setValue(0);

    addAndMakeVisible(velocitySlider);
    velocitySlider.setRange(0.25, 4);
    velocitySlider.onValueChange = [this] {
        audioProcessor.setVelocity((float)velocitySlider.getValue());
    };
    velocitySlider.setValue(1);

    addAndMakeVisible(intervalRandSlider);
    intervalRandSlider.setRange(0, 1);
    intervalRandSlider.onValueChange = [this] {
        audioProcessor.setIntervalRand((float)intervalRandSlider.getValue());
    };
    intervalRandSlider.setValue(0.0);

    addAndMakeVisible(durationRandSlider);
    durationRandSlider.setRange(0, 1);
    durationRandSlider.onValueChange = [this] {
        audioProcessor.setDurationRand((float)durationRandSlider.getValue());
    };
    durationRandSlider.setValue(0);

    addAndMakeVisible(panningRandSlider);
    panningRandSlider.setRange(0, 1);
    panningRandSlider.onValueChange = [this] {
        audioProcessor.setPanningRand((float)panningRandSlider.getValue());
    };
    panningRandSlider.setValue(0);

    addAndMakeVisible(readposRandSlider);
    readposRandSlider.setRange(0, 1);
    readposRandSlider.onValueChange = [this] {
        audioProcessor.setReadposRand((float)readposRandSlider.getValue());
    };
    readposRandSlider.setValue(0);

    addAndMakeVisible(velocityRandSlider);
    velocityRandSlider.setRange(0, 1);
    velocityRandSlider.setSkewFactorFromMidPoint(0.1);
    velocityRandSlider.onValueChange = [this] {
        audioProcessor.setVelocityRand((float)velocityRandSlider.getValue());
    };
    velocityRandSlider.setValue(0.0);

    addAndMakeVisible(intervalLabel);
    addAndMakeVisible(durationLabel);
    addAndMakeVisible(panningLabel);
    addAndMakeVisible(readposLabel);
    addAndMakeVisible(velocityLabel);
    addAndMakeVisible(randomLabel);

    intervalLabel.setText("interval", juce::dontSendNotification);
    durationLabel.setText("duration", juce::dontSendNotification);
    panningLabel.setText("panning", juce::dontSendNotification);
    readposLabel.setText("readpos", juce::dontSendNotification);
    velocityLabel.setText("velocity", juce::dontSendNotification);
    randomLabel.setText("random", juce::dontSendNotification);

    intervalLabel.setJustificationType(juce::Justification::centred);
    durationLabel.setJustificationType(juce::Justification::centred);
    panningLabel.setJustificationType(juce::Justification::centred);
    readposLabel.setJustificationType(juce::Justification::centred);
    velocityLabel.setJustificationType(juce::Justification::centred);
    randomLabel.setJustificationType(juce::Justification::centred);

}

GrainMotherAudioProcessorEditor::~GrainMotherAudioProcessorEditor()
{
}

//==============================================================================
void GrainMotherAudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

}
void GrainMotherAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    audioFileDialogButton.setBounds({ 10, 10, 200, 30 });

    intervalSlider.setBounds(100, 100, 100, 100);
    durationSlider.setBounds(200, 100, 100, 100);
    panningSlider.setBounds(300, 100, 100, 100);
    readposSlider.setBounds(400, 100, 100, 100);
    velocitySlider.setBounds(500, 100, 100, 100);

    intervalRandSlider.setBounds(100, 200, 100, 100);
    durationRandSlider.setBounds(200, 200, 100, 100);
    panningRandSlider.setBounds(300, 200, 100, 100);
    readposRandSlider.setBounds(400, 200, 100, 100);
    velocityRandSlider.setBounds(500, 200, 100, 100);

    intervalLabel.setBounds(100, 50, 100, 30);
    durationLabel.setBounds(200, 50, 100, 30);
    panningLabel.setBounds(300, 50, 100, 30);
    readposLabel.setBounds(400, 50, 100, 30);
    velocityLabel.setBounds(500, 50, 100, 30);
    juce::Rectangle<int> thumbnailBounds(10, 300, getWidth() - 20, getHeight() - 300);
    audioformComponent.setBounds(thumbnailBounds);

    activeGrainsLabel.setBounds(10, getHeight() - 40, 100, 30);

}


void GrainMotherAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &audioFileDialogButton)
    {
        juce::FileChooser fileChooser("Please select the moose you want to load...",
            juce::File::getSpecialLocation(juce::File::userHomeDirectory),
            "*.wav");

        if (fileChooser.browseForFileToOpen())
        {
            juce::File file(fileChooser.getResult());
            audioProcessor.loadAudioFile(file);
            audioformComponent.setFile(file);
        }
    }

}


void GrainMotherAudioProcessorEditor::timerCallback()
{
    activeGrainsLabel.setText(juce::String(audioProcessor.getGrainNum()), juce::dontSendNotification);
}

