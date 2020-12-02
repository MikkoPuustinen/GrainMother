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
    , thumbnailCache(5)
    , audioformComponent(1024, formatManager, thumbnailCache, p)
    , grainVisualizer(p, vts)
    , draggingFiles(false)
{
    
    formatManager.registerBasicFormats();
    addAndMakeVisible(&audioformComponent);

    addAndMakeVisible(&grainVisualizer);
    
    setSize(800, 650);
    addAndMakeVisible(audioFileDialogButton);
    audioFileDialogButton.addListener(this);

    addAndMakeVisible(activeGrainsLabel);

    // Sliders

    intervalSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    //intervalSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    intervalSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(intervalSlider);
    intervalAttachment.reset(new SliderAttachment(valueTreeState, "interval", intervalSlider));

    durationSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    durationSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(durationSlider);
    durationAttachment.reset(new SliderAttachment(valueTreeState, "duration", durationSlider));

    panningSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panningSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(panningSlider);
    panningAttachment.reset(new SliderAttachment(valueTreeState, "panning", panningSlider));

    readposSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    readposSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(readposSlider);
    readposAttachment.reset(new SliderAttachment(valueTreeState, "readpos", readposSlider));

    velocitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocitySlider);
    velocityAttachment.reset(new SliderAttachment(valueTreeState, "velocity", velocitySlider));

    directionSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    directionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(directionSlider);
    directionAttachment.reset(new SliderAttachment(valueTreeState, "direction", directionSlider));

    intervalRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    intervalRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(intervalRandSlider);
    intervalRandAttachment.reset(new SliderAttachment(valueTreeState, "intervalRand", intervalRandSlider));

    durationRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    durationRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(durationRandSlider);
    durationRandAttachment.reset(new SliderAttachment(valueTreeState, "durationRand", durationRandSlider));

    panningRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    panningRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(panningRandSlider);
    panningRandAttachment.reset(new SliderAttachment(valueTreeState, "panningRand", panningRandSlider));

    readposRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    readposRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(readposRandSlider);
    readposRandAttachment.reset(new SliderAttachment(valueTreeState, "readposRand", readposRandSlider));

    velocityRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocityRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocityRandSlider);
    velocityRandAttachment.reset(new SliderAttachment(valueTreeState, "velocityRand", velocityRandSlider));


    // Labels 

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

    juce::File file(audioProcessor.filePath.getValue());
    audioformComponent.setFile(file);
    if (!audioformComponent.hasFile())
        grainVisualizer.initialize();
}

GrainMotherAudioProcessorEditor::~GrainMotherAudioProcessorEditor()
{
}

//==============================================================================
void GrainMotherAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void GrainMotherAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    if (draggingFiles)
    {
        g.setColour(juce::Colour(25, 30, juce::uint8(31)));
        g.fillAll(juce::Colour(47, 55, juce::uint8(56), juce::uint8(200)));
        g.fillRect(getLocalBounds().getWidth() / 2 - 150, getLocalBounds().getHeight() / 2 - 50, 300, 100);
        g.fillRect(getLocalBounds().getWidth() / 2 - 50, getLocalBounds().getHeight() / 2 - 150, 100, 300);
    }
}
void GrainMotherAudioProcessorEditor::resized()
{
    audioFileDialogButton.setBounds({ 10, 10, 200, 30 });

    intervalSlider.setBounds(100, 100, 100, 100);
    durationSlider.setBounds(200, 100, 100, 100);
    panningSlider.setBounds(300, 100, 100, 100);
    readposSlider.setBounds(400, 100, 100, 100);
    velocitySlider.setBounds(500, 100, 100, 100);
    directionSlider.setBounds(600, 100, 100, 100);

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

}

void GrainMotherAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y) {
    for (auto& f : files)
    {
        DBG(f);
    }
    juce::File file(files[0]);
    audioProcessor.loadAudioFile(file);
    audioformComponent.setFile(file);
    draggingFiles = false;
    repaint();
}

void GrainMotherAudioProcessorEditor::fileDragEnter(const juce::StringArray& files, int x, int y)
{
    draggingFiles = true;
    repaint();
}

void GrainMotherAudioProcessorEditor::fileDragExit(const juce::StringArray& files)
{
    draggingFiles = false;
    repaint();
}

bool GrainMotherAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    return true;
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
