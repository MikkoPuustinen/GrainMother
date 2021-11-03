#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"


class AudioformComponent : public::juce::Component
    , private::juce::ChangeListener
{
public:
    AudioformComponent(int sourceSamplesPerThumbnailSample,
        juce::AudioFormatManager& formatManager,
        juce::AudioThumbnailCache& cache,
        GrainMotherAudioProcessor& p)
        : thumbnail(sourceSamplesPerThumbnailSample, formatManager, cache), audioProcessor(p), start(0.0f), end(0.0f)
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
    bool changeZoom(double start, double end)
    {
        this->start = start * thumbnail.getTotalLength();
        this->end = end * thumbnail.getTotalLength();
        repaint();
        return false;
    }
    void reset() {
        start = 0;
        end = thumbnail.getTotalLength();
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
        g.drawFittedText("no file loaded", getLocalBounds(), juce::Justification::centred, 4);
    }

    void paintIfFileLoaded(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(246, 244, 243));
        g.setColour(juce::Colour(255, 105, 120));
        juce::Rectangle<int> bounds = juce::Rectangle(4, 0, getWidth() - 8, getHeight());
        thumbnail.drawChannels(g, bounds, start, end, 1.0f);
        g.setColour(juce::Colour(230, 57, 70));
        thumbnail.drawChannels(g, bounds, start, end, 0.55f);
    }
    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source == &thumbnail)
            thumbnailChanged();
    }

private:

    void thumbnailChanged()
    {
        reset();
        repaint();
    }
    juce::AudioThumbnail thumbnail;
    GrainMotherAudioProcessor& audioProcessor;

    float start;
    float end;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioformComponent)

};