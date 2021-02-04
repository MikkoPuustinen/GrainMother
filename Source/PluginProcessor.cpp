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
                                                        juce::NormalisableRange(0.5f, 1000.0f, 0.5f, 0.3f, false),  //range 
                                                        1.0f),                  // default value
            std::make_unique<juce::AudioParameterFloat>("duration"     ,"Duration"     ,  juce::NormalisableRange(0.0f, 1.0f)      ,     0.250f ),
            std::make_unique<juce::AudioParameterFloat>("panning"      ,"Panning"      , -1.0f  ,     1.0f ,     0.0f  ),  
            std::make_unique<juce::AudioParameterFloat>("readpos"      ,"Readpos"      ,  juce::NormalisableRange(0.0f, 1.0f)      ,     0.250f ),
            std::make_unique<juce::AudioParameterInt>  ("velocity"     ,"Velocity"     ,   -12  ,       12 ,     0     ),
            std::make_unique<juce::AudioParameterInt>  ("fineTune"     ,"FineTune"     , -100   ,     100  ,     0     ),
            std::make_unique<juce::AudioParameterFloat>("direction"    ,"Direction"    ,  0.0f  ,     1.0f ,     1.0f  ),
            std::make_unique<juce::AudioParameterFloat>("intervalRand" ,"IntervalRand" ,  0.0f  ,     1.0f ,     0.0f  ),
            std::make_unique<juce::AudioParameterFloat>("durationRand" ,"DurationRand" ,  0.0f  ,     1.0f ,     0.0f  ),
            std::make_unique<juce::AudioParameterFloat>("panningRand"  ,"PanningRand"  ,  0.0f  ,     1.0f ,     0.0f  ),
            std::make_unique<juce::AudioParameterFloat>("readposRand"  ,"ReadposRand"  ,  0.0f  ,     1.0f ,     0.5f  ),
            std::make_unique<juce::AudioParameterFloat>("velocityRand" ,"VelocityRand" ,  0.0f  ,     1.0f ,     0.0f  ),
            std::make_unique<juce::AudioParameterFloat>("volume"       ,"Volume"       ,  0.0f  ,     1.0f ,     0.5f  ),
            std::make_unique<juce::AudioParameterFloat>("filterFreq"   ,"FilterFreq"   ,  juce::NormalisableRange(20.0f, 20000.0f, 1.0f, 0.3f, false) ,     100.0f ),
            std::make_unique<juce::AudioParameterFloat>("resonance"    ,"Resonance"    ,  juce::NormalisableRange(0.1f , 1.0f, 0.005f    ) ,     0.1f   ),
            std::make_unique<juce::AudioParameterInt>  ("filterType"   ,"filterType"   ,  0     ,     3 ,        0 ),
        })
        , lowPassFilter(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 10000.0f, 0.1)), coefficients(juce::dsp::IIR::Coefficients<float>::makeLowPass(44100, 10000.0f, 0.1))
#endif
{
    frequencies.resize(300);
    for (size_t i = 0; i < frequencies.size(); ++i) {
        frequencies[i] = 20.0 * std::pow(2.0, i / 30.0);
    }
    magnitudes.resize(300);
    intervalParameter = parameters.getRawParameterValue("interval");
    durationParameter = parameters.getRawParameterValue("duration");
    panningParameter = parameters.getRawParameterValue("panning");
    readposParameter = parameters.getRawParameterValue("readpos");
    velocityParameter = parameters.getRawParameterValue("velocity");
    directionParameter = parameters.getRawParameterValue("direction");


    intervalRandParameter = parameters.getRawParameterValue("intervalRand");
    durationRandParameter = parameters.getRawParameterValue("durationRand");
    panningRandParameter = parameters.getRawParameterValue("panningRand");
    readposRandParameter = parameters.getRawParameterValue("readposRand");
    velocityRandParameter = parameters.getRawParameterValue("velocityRand");

    masterVolumeParameter = parameters.getRawParameterValue("volume");

    filterFreq = parameters.getRawParameterValue("filterFreq");
    resonance = parameters.getRawParameterValue("resonance");

    parameters.addParameterListener("interval", this);
    parameters.addParameterListener("duration", this);
    parameters.addParameterListener("panning", this);
    parameters.addParameterListener("readpos", this);
    parameters.addParameterListener("velocity", this);
    parameters.addParameterListener("fineTune", this);

    parameters.addParameterListener("direction", this);

    parameters.addParameterListener("intervalRand", this);
    parameters.addParameterListener("durationRand", this);
    parameters.addParameterListener("panningRand", this);
    parameters.addParameterListener("readposRand", this);
    parameters.addParameterListener("velocityRand", this);
    parameters.addParameterListener("filterFreq", this);
    parameters.addParameterListener("resonance", this);
    parameters.addParameterListener("filterType", this);

    filePath = parameters.state.getPropertyAsValue("AUDIO_FILEPATH", nullptr, true);
}

GrainMotherAudioProcessor::~GrainMotherAudioProcessor()
{
    parameters.removeParameterListener("interval", this);
    parameters.removeParameterListener("duration", this);
    parameters.removeParameterListener("panning", this);
    parameters.removeParameterListener("readpos", this);
    parameters.removeParameterListener("velocity", this);
    parameters.removeParameterListener("direction", this);

    parameters.removeParameterListener("intervalRand", this);
    parameters.removeParameterListener("durationRand", this);
    parameters.removeParameterListener("panningRand", this);
    parameters.removeParameterListener("readposRand", this);
    parameters.removeParameterListener("velocityRand", this);
    parameters.removeParameterListener("filterFreq", this);
    parameters.removeParameterListener("resonance", this);
    parameters.removeParameterListener("filterType", this);
}

void GrainMotherAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
    // TODO: REFACTOR THIS MESS TO SOME KIND OF LISTENER CLASS
    if (parameterID == "interval") {
        setInterval(newValue);
    } else if (parameterID == "duration") {
        setDuration(newValue);
    } else if (parameterID == "panning") {
        setPanning(newValue);
    } else if (parameterID == "readpos") {
        setReadpos(newValue);
    } else if (parameterID == "velocity") {
        setVelocity(newValue);
    } else if (parameterID == "fineTune") {
        setFineTune(newValue);
    } else if (parameterID == "direction") {
        setDirection(newValue);
    } else if (parameterID == "intervalRand") {
        setIntervalRand(newValue);
    } else if (parameterID == "durationRand") {
        setDurationRand(newValue);
    } else if (parameterID == "panningRand") {
        setPanningRand(newValue);
    } else if (parameterID == "readposRand") {
        setReadposRand(newValue);
    } else if (parameterID == "velocityRand") {
        setVelocityRand(newValue);
    } else if (parameterID == "filterFreq") {
        updateFilterGraph();
    } else if (parameterID == "resonance") {
        updateFilterGraph();
    } else if (parameterID == "filterType") {
        shouldUpdateFilter = true;
    }
}

void GrainMotherAudioProcessor::setInterval(float interval)
{
    
    puroEngine.intervalParam.centre = interval;
    const float intervalP = puroEngine.intervalParam.get();
    puroEngine.timer.interval = puro::math::round(puroEngine.durationParam.centre / intervalP);
}

void GrainMotherAudioProcessor::setDuration(float duration)
{
    if (duration > 1.0f)
        duration = 1.0f;
    float sampleDuration = (float)(duration * getMaximumPosition() * this->getSampleRate());
    if (sampleDuration < 0)
        sampleDuration = 0.0f;
    puroEngine.durationParam.centre = sampleDuration;
}
void GrainMotherAudioProcessor::setPanning(float panning)
{
    puroEngine.panningParam.centre = (float)panning;
}
void GrainMotherAudioProcessor::setReadpos(float readpos)
{
    puroEngine.readposParam.centre = (float)(readpos * getMaximumPosition() * this->getSampleRate());
}
void GrainMotherAudioProcessor::setVelocity(float velocity)
{
    puroEngine.velocityParam.centre = (float)velocity / 12;
}
void GrainMotherAudioProcessor::setFineTune(float fineTune)
{
    puroEngine.fineTuneParam.centre = (float)fineTune;
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

void GrainMotherAudioProcessor::updateFilterGraph()
{
    if (coefficients)
        coefficients->getMagnitudeForFrequencyArray(frequencies.data(), magnitudes.data(), frequencies.size(), this->getSampleRate());
}

void GrainMotherAudioProcessor::updateFilterPath(juce::Path& p, juce::Rectangle<int> b, double pixels)
{
    if (shouldUpdateFilter) {
        updateFilterGraph();
        shouldUpdateFilter = false;
    }

    p.startNewSubPath(b.getX(), magnitudes[0] > 0 ? b.getCentreY() - pixels * std::log(magnitudes[0]) / std::log(2) : b.getBottom());
    const double xStep = static_cast<double> (b.getWidth()) / frequencies.size();
    for (size_t i = 1; i < frequencies.size(); ++i)
    {
        p.lineTo(float(b.getX() + i * xStep),
            float(magnitudes[i] > 0 ? b.getCentreY() - pixels * std::log(magnitudes[i]) / std::log(2) : b.getBottom()));
    }
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
void GrainMotherAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    auto f = filePath.getValue();
    juce::File file(filePath.getValue());
    loadAudioFile(file);

    previousGain = *masterVolumeParameter;
    lastSampleRate = sampleRate;

    setDirection(parameters.getParameter("direction")->getValue());
    setReadposRand(parameters.getParameter("readposRand")->getValue());
    setPanningRand(parameters.getParameter("panningRand")->getValue());

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    lowPassFilter.prepare(spec);
    lowPassFilter.reset();
    //updateFilter();
    updateFilterGraph();

    float freq = *parameters.getRawParameterValue("filterFreq");
    float res = *parameters.getRawParameterValue("resonance");

    //coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq, res);

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

void GrainMotherAudioProcessor::updateFilter()
{
    int type = *parameters.getRawParameterValue("filterType");
    float freq = *parameters.getRawParameterValue("filterFreq");
    float res = *parameters.getRawParameterValue("resonance");
    switch (type)
    {
    case 0:
    {
        coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(lastSampleRate, freq, res);
        break;
    }
    case 2:
    {
        coefficients = *juce::dsp::IIR::Coefficients<float>::makeHighPass(lastSampleRate, freq, res);
        break;
    }
    case 3:
    {
        coefficients = *juce::dsp::IIR::Coefficients<float>::makeBandPass(lastSampleRate, freq, res);
        break;
    }
    }
}

void GrainMotherAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    juce::MidiBuffer::Iterator i(midiMessages);
    juce::MidiMessage m;
    int t;

    while (i.getNextEvent(m, t))
    {
        if (m.isNoteOn()) {
            activeMidiNotes[m.getNoteNumber()] = m.getVelocity();
        }
        if (m.isNoteOff()) {
            activeMidiNotes[m.getNoteNumber()] = 0;
        }
        if (m.isAllNotesOff()) {
            for (int i = 0; i < 128; ++i)
                activeMidiNotes[i] = 0;
        }
    }
    juce::Array<juce::Array<int>> activeNotes;

    for (int i = 0; i < 128; i++) {
        if (activeMidiNotes[i] > 0) {
            activeNotes.add(juce::Array<int> {i, activeMidiNotes[i] });
        }
    }

    puroEngine.processBlock(buffer, activeNotes);

    juce::dsp::AudioBlock <float> block(buffer);


    updateFilter();

    *lowPassFilter.state = *coefficients;

    lowPassFilter.process(juce::dsp::ProcessContextReplacing<float>(block));

    auto output = (float)*masterVolumeParameter;
    if (output == previousGain)
    {
        buffer.applyGain(output);
    }
    else
    {
        buffer.applyGainRamp(0, buffer.getNumSamples(), previousGain, output);
        previousGain = output;
    }
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
            loadAudioFile(juce::File(filePath.getValue()));
        }
    float freq = *parameters.getRawParameterValue("filterFreq");
    float res = *parameters.getRawParameterValue("resonance");

    coefficients = *juce::dsp::IIR::Coefficients<float>::makeLowPass(this->getSampleRate(), freq, res);
    //updateFilter();
    updateFilterGraph();
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
    setDuration(parameters.getParameter("duration")->getValue());
    setReadpos(parameters.getParameter("readpos")->getValue());
    const float intervalP = puroEngine.intervalParam.get();
    puroEngine.timer.interval = puro::math::round(puroEngine.durationParam.centre / intervalP);
}

float GrainMotherAudioProcessor::getMaximumPosition() {
    return audioFileBuffer.getNumSamples() / getSampleRate();
}

float GrainMotherAudioProcessor::getMaximumSampleCount() {
    return audioFileBuffer.getNumSamples();
}
