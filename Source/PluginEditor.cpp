/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#define JUCE_ENABLE_REPAINT_DEBUGGING
//==============================================================================
GrainMotherAudioProcessorEditor::GrainMotherAudioProcessorEditor (GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p)
    , valueTreeState(vts)
    , audioProcessor (p)
    , audioFileDialogButton("loadAudioButton")
    , thumbnailCache(5)
    , audioformComponent(1024, formatManager, thumbnailCache, p)
    , audioformEvents(p, vts)
    , grainVisualizer(p, vts)
    , draggingFiles(false)
{
    setOpaque(true);
    juce::LookAndFeel::setDefaultLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    formatManager.registerBasicFormats();
    addAndMakeVisible(&audioformComponent);
    
    addAndMakeVisible(&audioformEvents);
    addAndMakeVisible(&grainVisualizer);
    grainVisualizer.setInterceptsMouseClicks(false, false);
    
    setSize(1000, 750);
    
    addAndMakeVisible(header);
    header.setText("GrainMother", juce::dontSendNotification);
    header.setJustificationType(juce::Justification::centred);
    header.setFont(juce::Font(80.0f));
    // Sliders

    velocitySlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    velocitySlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(velocitySlider);
    velocityAttachment.reset(new SliderAttachment(valueTreeState, "velocity", velocitySlider));

    fineTuneSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryVerticalDrag);
    fineTuneSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(fineTuneSlider);
    fineTuneAttachment.reset(new SliderAttachment(valueTreeState, "fineTune", fineTuneSlider));

    directionSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    directionSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    directionSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(directionSlider);
    directionAttachment.reset(new SliderAttachment(valueTreeState, "direction", directionSlider));

    outputSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    outputSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    outputSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(outputSlider);
    outputAttachment.reset(new SliderAttachment(valueTreeState, "volume", outputSlider));

    panningRandSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    panningRandSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    panningRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(panningRandSlider);
    panningRandAttachment.reset(new SliderAttachment(valueTreeState, "panningRand", panningRandSlider));

    readposRandSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    readposRandSlider.setLookAndFeel(&GrainMotherSliderLookAndFeel::getInstance());
    readposRandSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 50, 20);
    addAndMakeVisible(readposRandSlider);
    readposRandAttachment.reset(new SliderAttachment(valueTreeState, "readposRand", readposRandSlider));

    // Labels 
    tuneLabel.setText("tune", juce::dontSendNotification);

    outputLabel.setText("output", juce::dontSendNotification);
    randomPanningLabel.setText("width", juce::dontSendNotification);
    randomReadposLabel.setText("scatter", juce::dontSendNotification);
    directionLabel.setText("direction", juce::dontSendNotification);

    tuneLabel.setJustificationType(juce::Justification::centred);
    outputLabel.setJustificationType(juce::Justification::centred);
    randomPanningLabel.setJustificationType(juce::Justification::centred);
    randomReadposLabel.setJustificationType(juce::Justification::centred);
    directionLabel.setJustificationType(juce::Justification::centred);
    outputLabel.setFont(22.0f);
    randomPanningLabel.setFont(22.0f);
    randomReadposLabel.setFont(22.0f);
    directionLabel.setFont(22.0f);
    tuneLabel.setFont(22.0f);

    addAndMakeVisible(outputLabel);
    addAndMakeVisible(randomPanningLabel);
    addAndMakeVisible(randomReadposLabel);
    addAndMakeVisible(directionLabel);
    addAndMakeVisible(tuneLabel);

    juce::File file(audioProcessor.filePath.getValue());
    audioformComponent.setFile(file);
    if (!audioformComponent.hasFile())
        audioformEvents.initialize();

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
    juce::Path uiPath;
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
    sliders.items.add(juce::FlexItem(outputSlider).withFlex(0, 1, slidersW * 0.25f));
    
    sliders.performLayout(sliderBounds);

    juce::FlexBox labels;
    labels.flexDirection = juce::FlexBox::Direction::row;

    juce::Rectangle<int> sliderLabelBounds(getWidth() * 0.25f, sliderPStartY +10, slidersW, 40);

    labels.items.add(juce::FlexItem(directionLabel).withFlex(0, 1, slidersW * 0.25f));
    labels.items.add(juce::FlexItem(randomReadposLabel).withFlex(0, 1, slidersW * 0.25f));
    labels.items.add(juce::FlexItem(randomPanningLabel).withFlex(0, 1, slidersW * 0.25f));
    labels.items.add(juce::FlexItem(outputLabel).withFlex(0, 1, slidersW * 0.25f));
    labels.performLayout(sliderLabelBounds);

    tuneLabel.setBounds(70, sliderPStartY - 30, 100, 40);
    juce::FlexBox leftP;
    juce::Rectangle<int> leftPBounds(70, sliderPStartY, 100, 130);
    
    leftP.flexDirection = juce::FlexBox::Direction::row;

    leftP.items.add(juce::FlexItem(velocitySlider).withFlex(0, 1, 100));

    leftP.performLayout(leftPBounds);
    fineTuneSlider.setBounds(40, getHeight() - 180, 60, 80);
    juce::Rectangle<int> thumbnailBounds(50, 80, getWidth() - 100, 300);
    audioformComponent.setBounds(thumbnailBounds);
    juce::Rectangle<int> eventBounds(50 - 5, 80, getWidth() - 90, 300);
    audioformEvents.setBounds(eventBounds);

    grainVisualizer.setBounds(thumbnailBounds);

    audioFileDialogButton.setBounds({ getWidth() - 90, thumbnailBounds.getY(), 50, 50 });

}

void GrainMotherAudioProcessorEditor::filesDropped(const juce::StringArray& files, int x, int y) {

    juce::File file(files[0]);
    audioProcessor.loadAudioFile(file);
    audioformComponent.setFile(file);
    audioformEvents.initialize();
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
            audioformEvents.initialize();
        }
    }
}
