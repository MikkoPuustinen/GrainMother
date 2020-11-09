/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrainMotherAudioProcessorEditor::GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p)
    , valueTreeState(vts)
    , audioProcessor (p)
    , audioFileDialogButton("Load audio file")
   // , intervalSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    //, durationSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
   // , panningSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    //, readposSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    //, velocitySlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)

    , intervalRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , durationRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , panningRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , readposRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , velocityRandSlider(juce::Slider::SliderStyle::RotaryVerticalDrag, juce::Slider::TextEntryBoxPosition::TextBoxBelow)
    , thumbnailCache(5)
    , audioformComponent(1024, formatManager, thumbnailCache, p)
    , grainVisualizer(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.

    
    formatManager.registerBasicFormats();
    addAndMakeVisible(&audioformComponent);

    addAndMakeVisible(&grainVisualizer);
    


    setSize(800, 650);
    addAndMakeVisible(audioFileDialogButton);
    audioFileDialogButton.addListener(this);

    addAndMakeVisible(activeGrainsLabel);
    startTimerHz(30);

    intervalSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    intervalSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(intervalSlider);
    intervalAttachment.reset(new SliderAttachment(valueTreeState, "interval", intervalSlider));
    intervalSlider.onValueChange = [this] {
        audioProcessor.setInterval((float)intervalSlider.getValue());
    };

    durationSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    durationSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(durationSlider);
    durationAttachment.reset(new SliderAttachment(valueTreeState, "duration", durationSlider));
    durationSlider.onValueChange = [this] {
        //audioProcessor.setDuration((float)durationSlider.getValue());
    };

    panningSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panningSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(panningSlider);
    panningAttachment.reset(new SliderAttachment(valueTreeState, "panning", panningSlider));
    panningSlider.onValueChange = [this] {
        audioProcessor.setPanning((float)panningSlider.getValue());
    };

    readposSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    readposSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(readposSlider);
    readposAttachment.reset(new SliderAttachment(valueTreeState, "readpos", readposSlider));
    readposSlider.onValueChange = [this] {
        audioProcessor.setReadpos((float)readposSlider.getValue());
    };

    velocitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocitySlider);
    velocityAttachment.reset(new SliderAttachment(valueTreeState, "velocity", velocitySlider));
    velocitySlider.onValueChange = [this] {
        audioProcessor.setVelocity((float)velocitySlider.getValue());
    };

    intervalRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    intervalRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(intervalRandSlider);
    intervalRandAttachment.reset(new SliderAttachment(valueTreeState, "intervalRand", intervalRandSlider));
    intervalRandSlider.onValueChange = [this] {
        audioProcessor.setIntervalRand((float)intervalRandSlider.getValue());
    };

    durationRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    durationRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(durationRandSlider);
    durationRandAttachment.reset(new SliderAttachment(valueTreeState, "durationRand", durationRandSlider));
    durationRandSlider.onValueChange = [this] {
        audioProcessor.setDurationRand((float)durationRandSlider.getValue());
    };

    panningRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panningRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(panningRandSlider);
    panningRandAttachment.reset(new SliderAttachment(valueTreeState, "panningRand", panningRandSlider));
    panningRandSlider.onValueChange = [this] {
        audioProcessor.setPanningRand((float)panningRandSlider.getValue());
    };

    readposRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    readposRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(readposRandSlider);
    readposRandAttachment.reset(new SliderAttachment(valueTreeState, "readposRand", readposRandSlider));
    readposRandSlider.onValueChange = [this] {
        audioProcessor.setInterval((float)readposRandSlider.getValue());
    };

    velocityRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocityRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocityRandSlider);
    velocityRandAttachment.reset(new SliderAttachment(valueTreeState, "velocityRand", velocityRandSlider));
    velocityRandSlider.onValueChange = [this] {
        audioProcessor.setInterval((float)velocityRandSlider.getValue());
    };

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
    grainVisualizer.setBounds(thumbnailBounds);

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

