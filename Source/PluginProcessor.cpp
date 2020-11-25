/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GrainMotherAudioProcessor::GrainMotherAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor ( BusesProperties()
        #if ! JucePlugin_IsMidiEffect
        #if ! JucePlugin_IsSynth
                 .withInput("Input", juce::AudioChannelSet::stereo(), true)
        #endif
                 .withOutput("Output", juce::AudioChannelSet::stereo(), true)
        #endif
     ) 
        , parameters(*this, nullptr, juce::Identifier("GrainMother"), 
        {
            std::make_unique<juce::AudioParameterFloat>("interval",             // parameterID
                                                        "Interval",             // parameter name
                                                        0.5f,                   // minimum value
                                                        1000.0f,                // maximum value
                                                        1.0f),                  // default value
                                                    std::make_unique<juce::AudioParameterFloat>("duration",
                                                        "Duration",
                                                        1.0f,
                                                        88200.0f,
                                                        44100.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("panning",
                                                        "Panning",
                                                        -1.0f,
                                                        1.0f, 
                                                        0.0f), 
                                                    std::make_unique<juce::AudioParameterFloat>("readpos",
                                                        "Readpos",
                                                        0.0f, 
                                                        1.0f,
                                                        0.01f),
                                                    std::make_unique<juce::AudioParameterFloat>("velocity",
                                                        "Velocity",
                                                        0.25f,
                                                        4.0f,
                                                        1.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("direction",
                                                        "Direction",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("intervalRand",
                                                        "IntervalRand",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("durationRand",
                                                        "DurationRand",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("panningRand",
                                                        "PanningRand",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("readposRand",
                                                        "ReadposRand",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                    std::make_unique<juce::AudioParameterFloat>("velocityRand",
                                                        "VelocityRand",
                                                        0.0f,
                                                        1.0f,
                                                        0.0f),
                                                        std::make_unique<puro::Parameter<true, false>>()

        })
#endif
{
    intervalParameter = parameters.getRawParameterValue("interval");
    durationParameter = parameters.getRawParameterValue("duration");
    panningParameter = parameters.getRawParameterValue("panning");
    readposParameter = parameters.getRawParameterValue("readpos");
    velocityParameter = parameters.getRawParameterValue("velocity");
    directionParameter = parameters.getRawParameterValue("direction");
    

    filePath = parameters.state.getPropertyAsValue("AUDIO_FILEPATH", nullptr, true);
}

GrainMotherAudioProcessor::~GrainMotherAudioProcessor()
{
}

void GrainMotherAudioProcessor::setInterval(float interval)
{
    puroEngine.intervalParam.centre = interval;
    const float intervalP = puroEngine.intervalParam.get();
    puroEngine.timer.interval = puro::math::round(puroEngine.durationParam.centre / intervalP);
}

void GrainMotherAudioProcessor::setDuration(float duration)
{
    puroEngine.durationParam.centre = (float)(duration * this->getSampleRate());
}
void GrainMotherAudioProcessor::setPanning(float panning)
{
    puroEngine.panningParam.centre = (float)panning;
}
void GrainMotherAudioProcessor::setReadpos(float readpos)
{
    puroEngine.readposParam.centre = (float)(readpos * this->getSampleRate());
}
void GrainMotherAudioProcessor::setVelocity(float velocity)
{
    puroEngine.velocityParam.centre = (float)velocity;
}
void GrainMotherAudioProcessor::setDirection(float direction)
{
    puroEngine.directionParam.centre = direction;
}
void GrainMotherAudioProcessor::setIntervalRand(float deviation)
{
    puroEngine.intervalParam.deviation = deviation;
}

void GrainMotherAudioProcessor::setDurationRand(float deviation)
{
    puroEngine.durationParam.deviation = (float)(deviation / 1000.0f * this->getSampleRate());
}
void GrainMotherAudioProcessor::setPanningRand(float deviation)
{
    puroEngine.panningParam.deviation = deviation;
}
void GrainMotherAudioProcessor::setReadposRand(float deviation)
{
    puroEngine.readposParam.deviation = (float)(deviation * this->getSampleRate());
}
void GrainMotherAudioProcessor::setVelocityRand(float velocity)
{
    puroEngine.velocityParam.deviation = (float)velocity;
}

//==============================================================================
const juce::String GrainMotherAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int GrainMotherAudioProcessor::getGrainNum()
{
    return puroEngine.pool.size();
}

puro::AlignedPool<Grain> GrainMotherAudioProcessor::getGrainPool()
{
    return puroEngine.pool;
}

bool GrainMotherAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GrainMotherAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GrainMotherAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GrainMotherAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GrainMotherAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GrainMotherAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GrainMotherAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String GrainMotherAudioProcessor::getProgramName (int index)
{
    return {};
}

void GrainMotherAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void GrainMotherAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::File file(filePath.getValue());
    loadAudioFile(file);
}

void GrainMotherAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GrainMotherAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GrainMotherAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    puroEngine.processBlock(buffer);
}

//==============================================================================
bool GrainMotherAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* GrainMotherAudioProcessor::createEditor()
{
    return new GrainMotherAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void GrainMotherAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GrainMotherAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
        {
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
            filePath.referTo(parameters.state.getPropertyAsValue("AUDIO_FILEPATH", nullptr, true));
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GrainMotherAudioProcessor();
}


void GrainMotherAudioProcessor::loadAudioFile(juce::File file)
{
    filePath.setValue(file.getFullPathName());
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader != 0)
    {
        audioFileBuffer = juce::AudioSampleBuffer(reader->numChannels, (int)reader->lengthInSamples);
        reader->read(&audioFileBuffer, 0, (int)reader->lengthInSamples, 0, true, true);
    }

    puroEngine.sourceBuffer = puro::dynamic_buffer<MAX_NUM_SRC_CHANNELS, float>(audioFileBuffer.getNumChannels()
        , audioFileBuffer.getNumSamples()
        , audioFileBuffer.getArrayOfWritePointers());

    puroEngine.durationParam.maximum = audioFileBuffer.getNumSamples();
    puroEngine.readposParam.maximum = audioFileBuffer.getNumSamples();
}

float GrainMotherAudioProcessor::getMaximumPosition() {
    return audioFileBuffer.getNumSamples() / getSampleRate();
}

float GrainMotherAudioProcessor::getMaximumSampleCount() {
    return audioFileBuffer.getNumSamples();
}
