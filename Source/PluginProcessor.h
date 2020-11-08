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
    //, public juce::Timer
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
    void setIntervalRand(float interval);
    void setDurationRand(float duration);
    void setPanningRand(float panning);
    void setReadposRand(float readpos);
    void setVelocityRand(float velocity);
    int getGrainNum();
    puro::AlignedPool<Grain> getGrainPool();
    float getMaximumPosition();
    float getMaximumSampleCount();
    

private:
    PuroEngine puroEngine;

    juce::AudioSampleBuffer audioFileBuffer;

    juce::AudioParameterFloat* intervalParam;
    juce::AudioParameterFloat* durationParam;
    juce::AudioParameterFloat* panningParam;
    juce::AudioParameterFloat* readposParam;
    juce::AudioParameterFloat* velocityParam;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GrainMotherAudioProcessor)
};
