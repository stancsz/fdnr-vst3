#include "ReverbProcessor.h"
#include "PluginProcessor.h" // For Mode enum if needed, but we passed int

ReverbProcessor::ReverbProcessor()
{
    // Initialize defaults
    reverbParams.roomSize = 0.5f;
    reverbParams.damping = 0.5f;
    reverbParams.wetLevel = 0.33f;
    reverbParams.dryLevel = 0.4f;
    reverbParams.width = 1.0f;
    reverbParams.freezeMode = 0.0f;

    reverb.setParameters(reverbParams);

    // Initialize Chorus for modulation
    chorus.setRate(0.5f);
    chorus.setDepth(0.5f);
    chorus.setCentreDelay(10.0f);
    chorus.setFeedback(0.0f);
    chorus.setMix(1.0f); // We use chorus purely as a modulator block

    lowPassFilter.setType(juce::dsp::FirstOrderTPTFilterType::lowpass);
    highPassFilter.setType(juce::dsp::FirstOrderTPTFilterType::highpass);
}

ReverbProcessor::~ReverbProcessor()
{
}

void ReverbProcessor::prepare(const juce::dsp::ProcessSpec& spec)
{
    sampleRate = spec.sampleRate;

    reverb.prepare(spec);
    delayLine.prepare(spec);
    chorus.prepare(spec);
    lowPassFilter.prepare(spec);
    highPassFilter.prepare(spec);

    delayLine.setMaximumDelayInSamples(2.0 * sampleRate);

    // Pre-allocate wet buffer
    wetBuffer.setSize(spec.numChannels, spec.maximumBlockSize);
}

void ReverbProcessor::process(juce::dsp::ProcessContextReplacing<float>& context)
{
    // Get Audio Block
    auto& inputBlock = context.getInputBlock();
    auto& outputBlock = context.getOutputBlock();

    // Ensure wetBuffer is compatible (should be from prepare)
    // Copy input to wet buffer
    for(size_t i=0; i<inputBlock.getNumChannels(); ++i)
    {
         wetBuffer.copyFrom(i, 0, inputBlock.getChannelPointer(i), (int)inputBlock.getNumSamples());
    }

    // Create a sub-block for the current process size
    juce::dsp::AudioBlock<float> wetBlock(wetBuffer.getArrayOfWritePointers(), inputBlock.getNumChannels(), inputBlock.getNumSamples());
    juce::dsp::ProcessContextReplacing<float> wetContext(wetBlock);

    // 1. Delay (Pre-delay / Echo)
    // Warp affects delay time modulation or simply feedback in delay?
    // "Warp" in Supermassive is related to the length of delay steps relative to each other in the FDN.
    // Here we map Delay -> Delay Line length
    // Warp -> Chorus Depth/Feedback variation or secondary delay?
    // Let's use Delay for DelayLine.

    delayLine.setDelay(currentDelay * sampleRate / 1000.0f); // ms to samples
    delayLine.process(wetContext);

    // 2. Modulation (Warp + Mod Rate/Depth)
    // We use Chorus to simulate modulation/warp.
    // Mod Rate -> Chorus Rate
    // Mod Depth -> Chorus Depth
    // Warp -> maybe additional feedback or depth

    chorus.setRate(currentModRate);
    chorus.setDepth(currentModDepth / 100.0f);
    // If warp is high, maybe we increase chorus mix or feedback?
    // Let's map Warp to Chorus Feedback for "warping" sound.
    chorus.setFeedback((currentWarp / 100.0f) * 0.8f - 0.4f); // -0.4 to 0.4 feedback
    chorus.process(wetContext);

    // 3. Reverb (Density/Feedback/Decay)
    // We map:
    // Feedback -> Room Size / Damping (Decay)
    // Density -> Room Size / Width ?
    // In Valhalla: Feedback is decay length, Density is echo density.

    // Mapping Mode to internal characteristics
    // "Gemini": Fast attack, short decay, high density
    // "Hydra": Fast attack, short decay, density var

    float roomSize = currentFeedback / 100.0f; // 0 to 1
    float damping = 1.0f - (currentDensity / 100.0f); // Higher density -> less damping (more reflections)? Or inverse?
    // Actually Density usually means how many echoes per second.
    // juce::Reverb doesn't have density. It has roomSize (feedback) and damping (filter).

    // Let's tweak based on Mode
    // This is where we'd have different FDN matrices. For now, we just tweak Reverb params.

    // Mode Implementation Logic
    // We approximate the modes by tweaking reverb parameters, delay settings, and modulation

    float baseRoomSize = roomSize;
    float baseDamping = damping;
    float baseWidth = currentWidth / 100.0f;

    // Reset defaults
    chorus.setMix(0.5f);

    switch (currentMode) {
        case 0: // Gemini: Fast attack, short decay, high density
             // High density -> Lower damping?
             // Short decay -> Limit room size
             reverbParams.roomSize = baseRoomSize * 0.7f;
             reverbParams.damping = baseDamping * 0.5f;
             break;

        case 1: // Hydra: Fast-ish attack, shorter decay
             reverbParams.roomSize = baseRoomSize * 0.8f;
             chorus.setDepth(0.8f); // More modulation
             break;

        case 2: // Centaurus: Medium attack, longer decay
             reverbParams.roomSize = baseRoomSize;
             // Slightly delayed attack simulated by pre-delay (already set by user, but maybe we add offset?)
             break;

        case 3: // Sagittarius: Slow attack, longer decay
             reverbParams.roomSize = baseRoomSize;
             reverbParams.damping = baseDamping * 1.2f; // More damping for "slow" feel?
             break;

        case 4: // Great Annihilator: Medium attack, very long decay
             reverbParams.roomSize = 0.95f + (baseRoomSize * 0.04f); // Push to limit
             reverbParams.damping = 0.1f; // Long sustain
             break;

        case 5: // Andromeda: Slowest attack, very long decay
             reverbParams.roomSize = 0.98f;
             chorus.setRate(0.2f); // Slow modulation
             break;

        case 8: // Large Magellanic Cloud: Long reverb, repeating echoes
             reverbParams.roomSize = 0.9f;
             // Emphasize delay
             delayLine.setDelay(currentDelay * sampleRate / 1000.0f * 1.5f); // Longer delay
             break;

        case 14: // Aquarius: EchoVerb
             reverbParams.roomSize = baseRoomSize * 0.4f; // Smaller reverb
             reverbParams.wetLevel = 0.5f; // Less reverb, more delay
             // We need to boost the delay signal relative to reverb?
             // In this simple chain, they are serial.
             break;

        default:
             reverbParams.roomSize = baseRoomSize;
             reverbParams.damping = baseDamping;
             break;
    }

    reverbParams.width = baseWidth;
    if(reverbParams.wetLevel == 0.33f) reverbParams.wetLevel = 1.0f; // Ensure we are 100% wet in the parallel path unless modified
    reverbParams.dryLevel = 0.0f;

    reverb.setParameters(reverbParams);
    reverb.process(wetContext);

    // 4. EQ
    lowPassFilter.setCutoffFrequency(currentEqHigh);
    highPassFilter.setCutoffFrequency(currentEqLow);

    lowPassFilter.process(wetContext);
    highPassFilter.process(wetContext);

    // 5. Mix
    // dry is inputBlock
    // wet is wetBlock

    float wetAmount = currentMix / 100.0f;
    float dryAmount = 1.0f - wetAmount;

    // Sum
    outputBlock.multiplyBy(dryAmount);
    // Add wet
    // There is no simple add for blocks, iterate channels
    for (size_t ch = 0; ch < outputBlock.getNumChannels(); ++ch)
    {
        outputBlock.getChannelPointer(ch);
        juce::FloatVectorOperations::addWithMultiply(
            outputBlock.getChannelPointer(ch),
            wetBlock.getChannelPointer(ch),
            wetAmount,
            outputBlock.getNumSamples());
    }
}

void ReverbProcessor::reset()
{
    reverb.reset();
    delayLine.reset();
    chorus.reset();
    lowPassFilter.reset();
    highPassFilter.reset();
}

void ReverbProcessor::setParameters(float mix, float width, float delay, float warp,
                                    float feedback, float density, float modRate,
                                    float modDepth, float eqHigh, float eqLow, int mode)
{
    currentMix = mix;
    currentWidth = width;
    currentDelay = delay;
    currentWarp = warp;
    currentFeedback = feedback;
    currentDensity = density;
    currentModRate = modRate;
    currentModDepth = modDepth;
    currentEqHigh = eqHigh;
    currentEqLow = eqLow;
    currentMode = mode;
}

void ReverbProcessor::updateInternalParameters()
{
    // Logic moved to process or setParameters to avoid atomic issues,
    // but ideally should be here.
}
