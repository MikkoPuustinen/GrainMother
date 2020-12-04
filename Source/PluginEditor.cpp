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
    , audioFileDialogButton("loadAudioButton")
    , thumbnailCache(5)
    , audioformComponent(1024, formatManager, thumbnailCache, p)
    , grainVisualizer(p, vts)
    , draggingFiles(false)
{
    juce::LookAndFeel::setDefaultLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    formatManager.registerBasicFormats();
    addAndMakeVisible(&audioformComponent);

    addAndMakeVisible(&grainVisualizer);
    
    setSize(1000, 750);
    
    addAndMakeVisible(header);
    header.setText("GrainMother", juce::dontSendNotification);
    header.setJustificationType(juce::Justification::centred);
    header.setFont(juce::Font(80.0f));
    //header.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    // Sliders

    /*intervalSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
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
    readposAttachment.reset(new SliderAttachment(valueTreeState, "readpos", readposSlider));*/

    velocitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocitySlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    velocitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocitySlider);
    velocityAttachment.reset(new SliderAttachment(valueTreeState, "velocity", velocitySlider));

    directionSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    directionSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    directionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(directionSlider);
    directionAttachment.reset(new SliderAttachment(valueTreeState, "direction", directionSlider));

    /*intervalRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    intervalRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(intervalRandSlider);
    intervalRandAttachment.reset(new SliderAttachment(valueTreeState, "intervalRand", intervalRandSlider));*/

    durationRandSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    durationRandSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    durationRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(durationRandSlider);
    durationRandAttachment.reset(new SliderAttachment(valueTreeState, "durationRand", durationRandSlider));

    panningRandSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    panningRandSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    panningRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(panningRandSlider);
    panningRandAttachment.reset(new SliderAttachment(valueTreeState, "panningRand", panningRandSlider));

    readposRandSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    readposRandSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    readposRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(readposRandSlider);
    readposRandAttachment.reset(new SliderAttachment(valueTreeState, "readposRand", readposRandSlider));

    /*velocityRandSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocityRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 100, 20);
    addAndMakeVisible(velocityRandSlider);
    velocityRandAttachment.reset(new SliderAttachment(valueTreeState, "velocityRand", velocityRandSlider));*/


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

    addAndMakeVisible(audioFileDialogButton);
    audioFileDialogButton.addListener(this);
}

GrainMotherAudioProcessorEditor::~GrainMotherAudioProcessorEditor()
{
}

//==============================================================================
void GrainMotherAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(246,244,243));

    const int sliderPStartY = getHeight() * 0.5f + 50;
    auto uiPath = juce::Path();
    const float curve = 10;
    //uiPath.startNewSubPath(0, getHeight());
    uiPath.startNewSubPath(0, getHeight() - 75);
    uiPath.lineTo(150 - curve, getHeight() - 75);
    uiPath.quadraticTo(150 + curve * 0.1f, getHeight() - (75 + curve * 0.1f), 150 + curve * 0.5f, getHeight() - (75 + curve));
    uiPath.lineTo(225, sliderPStartY + curve);
    uiPath.quadraticTo(225 + (curve * 0.5f), sliderPStartY - (curve * 0.1f), 225 + curve * 1.5f, sliderPStartY);
    uiPath.lineTo(getWidth() - 225 - curve * 1.5f, sliderPStartY);
    uiPath.quadraticTo(getWidth() - 225 - (curve * 0.5f), sliderPStartY - (curve * 0.1f), getWidth() - 225, sliderPStartY + curve);
    uiPath.lineTo(getWidth() - 150 - curve * 0.5f, getHeight() - 75 - curve);
    uiPath.quadraticTo(getWidth() - 150 + (curve * 0.1f), getHeight() - (75 - curve * 0.1f), getWidth() - 150 + curve, getHeight() - 75);
    uiPath.lineTo(getWidth(), getHeight() - 75);
    //uiPath.lineTo(getWidth(), getHeight());
    //uiPath.closeSubPath();

    auto fillType = juce::FillType();
    fillType.setColour(juce::Colour(51,51,51));
    g.setFillType(fillType);
    //g.fillPath(uiPath);
    g.setColour(juce::Colour(51, 51, 51));
    g.strokePath(uiPath, juce::PathStrokeType(5));
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
    header.setBounds(0, 0, getWidth(), 80);

    juce::FlexBox sliders;
    sliders.flexDirection = juce::FlexBox::Direction::row;

    const int slidersW = getWidth() * 0.5f;
    const int sliderPStartY = getHeight() * 0.5f + 50;
    juce::Rectangle<int> sliderBounds(getWidth() * 0.25f, sliderPStartY + 50, slidersW, 250);
    
    sliders.items.add(juce::FlexItem(directionSlider).withFlex(0, 1, slidersW * 0.25f));
    sliders.items.add(juce::FlexItem(readposRandSlider).withFlex(0, 1, slidersW * 0.25f));
    sliders.items.add(juce::FlexItem(panningRandSlider).withFlex(0, 1, slidersW * 0.25f));
    sliders.items.add(juce::FlexItem(durationRandSlider).withFlex(0, 1, slidersW * 0.25f));
    
    sliders.performLayout(sliderBounds);

    juce::FlexBox leftP;
    juce::Rectangle<int> leftPBounds(75, sliderPStartY, 100, 130);
    leftP.flexDirection = juce::FlexBox::Direction::row;

    leftP.items.add(juce::FlexItem(velocitySlider).withFlex(0, 1, 100));

    leftP.performLayout(leftPBounds);

    

    /*intervalSlider.setBounds(100, 100, 100, 100);
    durationSlider.setBounds(200, 100, 100, 100);
    panningSlider.setBounds(300, 100, 100, 100);
    readposSlider.setBounds(400, 100, 100, 100);
    

    intervalRandSlider.setBounds(100, 200, 100, 100);
    
    velocityRandSlider.setBounds(500, 200, 100, 100);

    intervalLabel.setBounds(100, 50, 100, 30);
    durationLabel.setBounds(200, 50, 100, 30);
    panningLabel.setBounds(300, 50, 100, 30);
    readposLabel.setBounds(400, 50, 100, 30);
    velocityLabel.setBounds(500, 50, 100, 30);*/

    juce::Rectangle<int> thumbnailBounds(50, 80, getWidth() - 100, 300);
    audioformComponent.setBounds(thumbnailBounds);

    grainVisualizer.setBounds(thumbnailBounds);

    audioFileDialogButton.setBounds({ getWidth() - 90, thumbnailBounds.getY(), 50, 50 });

}

void GrainMotherAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y) {

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
