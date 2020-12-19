#pragma once

#ifdef JUCE_DEBUG
#define PURO_DEBUG 1
#else
#define PURO_DEBUG 0
#endif

#define MAX_NUM_CHANNELS 2
#define MAX_NUM_SRC_CHANNELS 2
#define MAX_BLOCK_SIZE 2048

#include "puro.hpp"
#include <JuceHeader.h>

#include <cmath>

/** A grain is a single audio playback instance. */
struct Grain
{
    Grain(int offset, int length, int startIndex, float panning, double velocity, int sourceLength, int direction, int midiNote)
        : alignment({ offset, length })
        , readInc(velocity)
        , readPos(startIndex)
        , envelopeInc(puro::envelope_halfcos_get_increment<float>(length))
        , envelopePos(envelopeInc)
        , panCoeffs(puro::pan_create_stereo(panning))
        , direction(direction)
        , note(midiNote)
        , adsr(0)
        , length(length)
    {
        std::tie(alignment, readPos) = puro::interp_avoid_out_of_bounds_reads<3>(alignment, readPos, readInc, sourceLength, direction);
    }

    puro::relative_alignment alignment;

    const double readInc;
    double readPos;

    float envelopeInc;
    float envelopePos;
    int length;
    int adsr;

    int direction;

    int note;

    puro::PanCoeffs<float, 2> panCoeffs;
};

/** Context is passed to process_grain function, and may contain persistent data or memory for the algorithm.
    In this case, it holds temp buffers to do the audio processing on.
 */
struct Context
{
    puro::heap_block_pool<float, std::allocator<float>> context_pool;
    puro::dynamic_buffer<MAX_NUM_CHANNELS, float> temp;
    puro::buffer<1, float> envelope;

    Context() : temp(MAX_NUM_CHANNELS, MAX_BLOCK_SIZE, context_pool), envelope(MAX_BLOCK_SIZE, context_pool) {}
};

template <typename BufferType, typename PositionType>
PositionType envelope_release(BufferType buffer, PositionType position, const PositionType increment) noexcept
{
    auto dst = buffer.channel(0);
    for (int i = 0; i < buffer.length(); ++i)
    {
        const auto sample = std::cos(position);
        dst[i] = sample;
        position += increment;
    }

    for (int ch = 1; ch < buffer.num_channels(); ++ch)
    {
        puro::math::copy(buffer.channel(ch), buffer.channel(0), buffer.length());
    }

    return position;
}

template <typename BufferType, typename PositionType>
PositionType envelope_attack(BufferType buffer, PositionType position, const PositionType increment) noexcept
{
    auto dst = buffer.channel(0);
    for (int i = 0; i < buffer.length(); ++i)
    {
        const auto sample = std::sin(position);
        dst[i] = sample;
        position += increment;
    }

    for (int ch = 1; ch < buffer.num_channels(); ++ch)
    {
        puro::math::copy(buffer.channel(ch), buffer.channel(0), buffer.length());
    }

    return position;
}

/** The actual beef of the processor, this function gets called for every grain in every block
    It receives a buffer dst that the grain should add its signal into.
 */
template <typename BufferType, typename ElementType, typename ContextType, typename SourceType>
bool process_grain(BufferType dst, ElementType& grain, ContextType& context, const SourceType source, bool held)
{
    // Crop the dst buffer to fit the grain's offset and length, i.e. if the grain starts in the middle of the block, or ends
    // before the end of the block. Update the dst buffer and grain.alignment.
   
    std::tie(dst, grain.alignment) = puro::alignment_advance_and_crop_buffer(dst, grain.alignment);

    // audio buffer uses the memory from context.temp, casting it into a buffer with compile-time constant number of channels, and truncating it to needed length
    auto audio = context.temp.template as_buffer<SourceType>().trunc(dst.length()); // clang "feature", requires the keyword "template" with the function call



    // read audio from source, cubic interpolation
    if (grain.direction == 0) {
        grain.readPos = puro::interp3_fill(audio, source, grain.readPos, -1 * grain.readInc);
    }
    else {
        
        grain.readPos = puro::interp3_fill(audio, source, grain.readPos, grain.readInc);
    }
    

    // truncate temp envelope buffer to fit the length of our audio, and fill
    auto  envelope = context.envelope.trunc(audio.length());
    if (held) {
        grain.envelopePos = puro::envelope_halfcos_fill(envelope, grain.envelopePos, grain.envelopeInc);
    }
    else {
        if (grain.adsr == 0) {
            grain.adsr = 1;
            if (grain.length * 0.5f < grain.alignment.remaining) {
                grain.alignment.remaining = grain.length -  grain.alignment.remaining;
                grain.envelopePos = puro::math::pi - grain.envelopePos;
            }
        }
        grain.envelopePos = puro::envelope_halfcos_fill(envelope, grain.envelopePos, grain.envelopeInc);
        //grain.envelopePos = envelope_kill(envelope, grain.envelopePos, grain.envelopeInc);
    }

    puro::multiply(audio, envelope);

    // get the cropped output segment that we want to add our output to, apply panning and add the audio signal
    BufferType output = dst.trunc(envelope.length());
    puro::pan_apply_and_add(output, audio, grain.panCoeffs);

    // return true if should be removed
    return (grain.alignment.remaining <= 0) // grain has depleted
        || (output.length() != dst.length()); // we've run out of source material to read
}

class PuroEngine
{
public:

    PuroEngine()
        : timer(0)
        , intervalParam(1.0f, 0.0f, 0.1f, 5000.0f)
        , durationParam(100.0f, 100.0f, 0, 44100 * 5)
        , panningParam(0.0f, 0.0f, -1.0f, 1.0f)
        , readposParam(44100.0f, 0.0f, 0, 88200)
        , velocityParam(0.0f, 0.0f,-1.0f, 1.0f)
        , fineTuneParam(-100, 100)
        , directionParam(0.0f, 1.0f)
        , sourceBuffer(0, 0)
        , halfStep(std::exp(std::log(2) / 1200))
    {
        pool.elements.reserve(4096);
    }

    /**
     A bootstrap function to call processGrains with correct number of source channels.
     This forces the compiler to compile an optimised version for each n of source channels. This removes potential branch misses down the line,
     since the actual inner loop (process_grain) runs with complete information about the number of channels it is processing.
     */
    void processBlock(juce::AudioBuffer<float>& writeBuffer, juce::Array<juce::Array<int>> activeNotes)
    {
        // TODO potential bug if number of output channels is not 2
        errorif(writeBuffer.getNumChannels() < 2, "BUG: implement different write buffer sizes");

        auto dstBuffer = puro::buffer_from_juce_buffer<puro::buffer<2, float>>(writeBuffer);

        const int numChannels = sourceBuffer.num_channels();
        if (numChannels == 1)
        {
            processGrains(dstBuffer, sourceBuffer.as_buffer<puro::buffer<1, float>>(), activeNotes);
        }
        else if (numChannels == 2)
        {
            processGrains(dstBuffer, sourceBuffer.as_buffer<puro::buffer<2, float>>(), activeNotes);
        }
        else if (numChannels != 0)
        {
            errorif(true, "source buffer channel number not supported");
        }
        else
        {
            // if numChannels == 0, the audio source file has not been loaded, and we do nothing
        }
    }

    template <typename BufferType, typename SourceType>
    void processGrains(BufferType buffer, SourceType source, juce::Array<juce::Array<int>> activeNotes)
    {
        const int blockSize = buffer.length();

        // iterate all grains, adding their output to audio buffer
        for (auto&& it : pool)
        {
            bool held = false;
            for (int i = 0; i < activeNotes.size(); i++) {
                if (activeNotes[i][0] - 60 == it.get().note)
                {
                    held = true; 
                }
            }
            if (process_grain(buffer, it.get(), context, source, held))
            {
                pool.pop(it);
            }
        }

        // add new grains
        int n = buffer.length();
        while ((n = timer.advance(n)) > 0)
        {
            for (int i = 0; i < activeNotes.size(); i++) {
                // for each tick, reset interval, get new values for the grain, and create grain
                int midiNote = 0;
                midiNote = activeNotes[i][0] - 60;
                const float interval = intervalParam.get();
                timer.interval = puro::math::round(durationParam.centre / interval);
                errorif(timer.interval < 0, "Well this is unexpected, timer shouldn't be let to do that");
                int direction = 0;
                if (directionParam.get() > (float)std::rand() / RAND_MAX)
                {
                    direction = 1;
                }
                const int duration = durationParam.get();
                const float panning = panningParam.get();
                int readpos;
                
                const float fineTune = fineTuneParam.get();
                
                float cent = pow(halfStep, fineTune);
                const float note = (float)midiNote / 12.0f;
                const float fineT = (float)fineTune / 1200.0f;
                float div = ((float)velocityParam.get() + note) ;
                float exp = pow(2, div);
                
                const float velocity = exp * cent;
                //const float velocity = exp - 1;

                if (direction == 0) { // 0 = reverse, 1 = normal playback
                    readpos = readposParam.get() + duration;
                }
                else {
                    readpos = readposParam.get();
                    if (readpos + duration > sourceBuffer.length())
                        readpos -= (readpos + duration) - sourceBuffer.length();
                }

                // push to the pool of grains, get a handle in return
                if (readpos < sourceBuffer.length())
                {
                    auto it = pool.push(Grain(blockSize - n, duration, readpos, panning, velocity, sourceBuffer.length(), direction, midiNote));

                    // immediately process the first block for the newly created grain, potentially also already removing it
                    // if the pool is full, it rejects the push operation and returns invalid iterator, and the grain doesn't get added
                    if (it.is_valid())
                    {
                        if (process_grain(buffer, it.get(), context, source, true))
                        {
                            pool.pop(it);
                        }
                    }
                }
            }
        }
        if (activeNotes.size() < 1) {
            timer.counter = timer.interval;
        }
    }

    Context context;
    puro::Timer<int> timer;
    puro::AlignedPool<Grain> pool;
    puro::dynamic_buffer<MAX_NUM_SRC_CHANNELS, float> sourceBuffer;

    puro::Parameter<float, true> intervalParam;
    puro::Parameter<int, false> durationParam;
    puro::Parameter<float, false> panningParam;
    puro::Parameter<int, false> readposParam;
    puro::Parameter<float, true> velocityParam;
    puro::Parameter<float, true> fineTuneParam;
    puro::Parameter<float, true> directionParam;

    private:
        const float halfStep;
};