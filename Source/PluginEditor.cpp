#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VST3OpenValhallaAudioProcessorEditor::VST3OpenValhallaAudioProcessorEditor (VST3OpenValhallaAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&lookAndFeel);

    addSlider(mixSlider, mixAtt, "MIX", "Mix");
    addSlider(widthSlider, widthAtt, "WIDTH", "Width");

    addSlider(delaySlider, delayAtt, "DELAY", "Delay");
    addSlider(warpSlider, warpAtt, "WARP", "Warp");

    addSlider(feedbackSlider, feedbackAtt, "FEEDBACK", "Feedback");
    addSlider(densitySlider, densityAtt, "DENSITY", "Density");

    addSlider(modRateSlider, modRateAtt, "MODRATE", "Mod Rate");
    addSlider(modDepthSlider, modDepthAtt, "MODDEPTH", "Mod Depth");

    addSlider(eqHighSlider, eqHighAtt, "EQHIGH", "EQ High");
    addSlider(eqLowSlider, eqLowAtt, "EQLOW", "EQ Low");

    // Mode
    addAndMakeVisible(modeComboBox);
    modeAtt = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.getAPVTS(), "MODE", modeComboBox);
    modeComboBox.addItemList(audioProcessor.getAPVTS().getParameter("MODE")->getAllValueStrings(), 1);

    modeLabel.setText("MODE:", juce::dontSendNotification);
    modeLabel.setJustificationType(juce::Justification::right);
    modeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(modeLabel);

    setSize (800, 400);
}

VST3OpenValhallaAudioProcessorEditor::~VST3OpenValhallaAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void VST3OpenValhallaAudioProcessorEditor::addSlider(juce::Slider& slider, std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment, const juce::String& paramID, const juce::String& name)
{
    slider.setSliderStyle(juce::Slider::Rotary);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(slider);

    // Use a label? Or just rely on lookandfeel?
    // For now, I'll just add the slider. The image has labels separate.
    // I'll paint the labels in paint() for simplicity or add Label components.
    // The attachment
    attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.getAPVTS(), paramID, slider);
}

//==============================================================================
void VST3OpenValhallaAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Background Dark Blue/Purple
    g.fillAll (juce::Colour(0xFF101020)); // Dark blue-ish

    g.setColour (juce::Colours::white);
    g.setFont (24.0f);
    g.drawFittedText ("ValhallaSupermassive Clone", getLocalBounds().removeFromTop(40), juce::Justification::left, 1);

    // Draw Group Boxes
    g.setColour (juce::Colour(0xFF202040));

    // Groups layout is handled in resized, but we can draw rects here if we knew coords.
    // Better to use a helper or just draw based on hardcoded relative positions for now.
}

void VST3OpenValhallaAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10);
    auto topBar = area.removeFromTop(40);
    auto bottomBar = area.removeFromBottom(40);

    // Mode Selector at bottom
    modeLabel.setBounds(bottomBar.removeFromLeft(80));
    modeComboBox.setBounds(bottomBar.removeFromLeft(200));

    // Main panels
    // Mix/Width | Delay/Warp | Feedback/Density | Mod | EQ

    auto width = area.getWidth() / 5;

    auto mixGroup = area.removeFromLeft(width);
    auto delayGroup = area.removeFromLeft(width * 1.5); // Bigger
    auto feedbackGroup = area.removeFromLeft(width);
    auto modGroup = area.removeFromLeft(width * 0.8);
    auto eqGroup = area; // Remainder

    int knobSize = 80;
    int padding = 10;

    // Mix Group
    mixSlider.setBounds(mixGroup.removeFromTop(mixGroup.getHeight()/2).reduced(padding));
    widthSlider.setBounds(mixGroup.reduced(padding));

    // Delay Group - Big Delay Knob?
    delaySlider.setBounds(delayGroup.removeFromLeft(delayGroup.getWidth()/2).reduced(padding));
    warpSlider.setBounds(delayGroup.reduced(padding));

    // Feedback Group
    feedbackSlider.setBounds(feedbackGroup.removeFromTop(feedbackGroup.getHeight()/2).reduced(padding));
    densitySlider.setBounds(feedbackGroup.reduced(padding));

    // Mod Group
    modRateSlider.setBounds(modGroup.removeFromTop(modGroup.getHeight()/2).reduced(padding));
    modDepthSlider.setBounds(modGroup.reduced(padding));

    // EQ Group
    eqHighSlider.setBounds(eqGroup.removeFromTop(eqGroup.getHeight()/2).reduced(padding));
    eqLowSlider.setBounds(eqGroup.reduced(padding));
}
