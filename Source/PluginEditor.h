#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ValhallaLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ValhallaLookAndFeel()
    {
        setColour(juce::Slider::thumbColourId, juce::Colours::white);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::white);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
        setColour(juce::Label::textColourId, juce::Colours::white);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                           const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto radius = (float) juce::jmin (width / 2, height / 2) - 4.0f;
        auto centreX = (float) x + (float) width  * 0.5f;
        auto centreY = (float) y + (float) height * 0.5f;
        auto rx = centreX - radius;
        auto ry = centreY - radius;
        auto rw = radius * 2.0f;
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        // Fill
        g.setColour (juce::Colours::black);
        g.fillEllipse (rx, ry, rw, rw);

        // Pointer
        juce::Path p;
        auto pointerLength = radius * 0.8f;
        auto pointerThickness = 3.0f;
        p.addRectangle (-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform (juce::AffineTransform::rotation (angle).translated (centreX, centreY));

        g.setColour (juce::Colours::white);
        g.fillPath (p);
    }
};

class VST3OpenValhallaAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    VST3OpenValhallaAudioProcessorEditor (VST3OpenValhallaAudioProcessor&);
    ~VST3OpenValhallaAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    VST3OpenValhallaAudioProcessor& audioProcessor;
    ValhallaLookAndFeel lookAndFeel;

    // Helper to add slider
    void addSlider(juce::Slider& slider, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, const juce::String& paramID, const juce::String& name);

    // Sliders
    juce::Slider mixSlider, widthSlider;
    juce::Slider delaySlider, warpSlider;
    juce::Slider feedbackSlider, densitySlider;
    juce::Slider modRateSlider, modDepthSlider;
    juce::Slider eqHighSlider, eqLowSlider;

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAtt, widthAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAtt, warpAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> feedbackAtt, densityAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modRateAtt, modDepthAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqHighAtt, eqLowAtt;

    // Labels
    std::vector<std::unique_ptr<juce::Label>> labels;

    // Mode Selector
    juce::ComboBox modeComboBox;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> modeAtt;
    juce::Label modeLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VST3OpenValhallaAudioProcessorEditor)
};
