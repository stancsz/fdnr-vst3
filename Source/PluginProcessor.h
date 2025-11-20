#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include "ReverbProcessor.h"

class VST3OpenValhallaAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    VST3OpenValhallaAudioProcessor();
    ~VST3OpenValhallaAudioProcessor() override;

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

    //==============================================================================
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    ReverbProcessor reverbProcessor;

    // Mode Enum
    enum Mode
    {
        Gemini = 0,
        Hydra,
        Centaurus,
        Sagittarius,
        GreatAnnihilator,
        Andromeda,
        Lyra,
        Capricorn,
        LargeMagellanicCloud,
        Triangulum,
        CirrusMajor,
        CirrusMinor,
        Cassiopeia,
        Orion,
        Aquarius,
        Pisces,
        Scorpio,
        Libra,
        Leo,
        Virgo,
        Pleiades
    };

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST3OpenValhallaAudioProcessor)
};
