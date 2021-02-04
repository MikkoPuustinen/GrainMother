/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PuroEngine.h"

//==============================================================================
/**
*/
class GrainMotherAudioProcessor : public juce::AudioProcessor
                                , public juce::AudioProcessorValueTreeState::Listener
{
public:
    //==============================================================================
    GrainMotherAudioProcessor();
    ~GrainMotherAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    void parameterChanged(const juce::String& parameterID, float newValue) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void loadAudioFile(juce::File file);
    void setInterval(float interval);
    void setDuration(float duration);
    void setPanning(float panning);
    void setReadpos(float readpos);
    void setVelocity(float velocity);
    void setFineTune(float fineTune);
    void setDirection(float direction);
    void setIntervalRand(float interval);
    void setDurationRand(float duration);
    void setPanningRand(float panning);
    void setReadposRand(float readpos);
    void setVelocityRand(float velocity);
    void updateFilterGraph();
    void updateFilterPath(juce::Path& p, juce::Rectangle<int> b, double pixels);
    int getGrainNum();
    void updateFilter();
    puro::AlignedPool<Grain> getGrainPool();
    float getMaximumPosition();
    float getMaximumSampleCount();
    juce::Value filePath;

private:
    PuroEngine puroEngine;

    juce::AudioSampleBuffer audioFileBuffer;

    juce::AudioProcessorValueTreeState parameters;

    float lastSampleRate;
    bool shouldUpdateFilter;

    int activeMidiNotes[128] = { 0 };

    juce::dsp::IIR::Coefficients<float>::Ptr coefficients;
    std::vector<double> magnitudes;
    std::vector<double> frequencies;

    juce::dsp::ProcessorDuplicator <juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients <float>> lowPassFilter;

    std::atomic<float>* filterFreq = nullptr;
    std::atomic<float>* resonance  = nullptr;

    std::atomic<float>* intervalParameter = nullptr;
    std::atomic<float>* durationParameter = nullptr;
    std::atomic<float>* panningParameter = nullptr;
    std::atomic<float>* readposParameter = nullptr;

    std::atomic<float>* velocityParameter = nullptr;
    std::atomic<float>* fineTuneParameter = nullptr;

    std::atomic<float>* directionParameter = nullptr;

    std::atomic<float>* intervalRandParameter = nullptr;
    std::atomic<float>* durationRandParameter = nullptr;
    std::atomic<float>* panningRandParameter = nullptr;
    std::atomic<float>* readposRandParameter = nullptr;
    std::atomic<float>* velocityRandParameter = nullptr;

    std::atomic<float>* filterType = nullptr;

    std::atomic<float>* masterVolumeParameter = nullptr;

    float previousGain;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessor)
};
