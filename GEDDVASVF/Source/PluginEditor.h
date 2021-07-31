/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "CommonFunctions.h"
#include "VASVFTraceComponent.h"
#include "FrequencyDecibelGridOverlay.h"

//====================Attachments
class AttachedSlider : public juce::Component
{
public:
    explicit AttachedSlider(juce::RangedAudioParameter& param)
        : label("", param.name),
        attachment(param, slider)
    {
        addAndMakeVisible(slider);
        addAndMakeVisible(label);

        slider.setTextValueSuffix(" " + param.label);

        label.attachToComponent(&slider, false);
        label.setJustificationType(juce::Justification::centred);
    }

    void resized() override { slider.setBounds(getLocalBounds().reduced(0, 20)); }
private:
    juce::Slider slider{ juce::Slider::SliderStyle::LinearBarVertical, juce::Slider::TextBoxBelow };
    juce::Label label;
    juce::SliderParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AttachedSlider)
};

class AttachedCombo : public juce::Component
{
public:
    explicit AttachedCombo(juce::RangedAudioParameter& param)
        : combo(param),
        label("", param.name),
        attachment(param, combo)
    {
        addAndMakeVisible(combo);
        addAndMakeVisible(label);

        label.attachToComponent(&combo, false);
        label.setJustificationType(juce::Justification::centred);
    }

    void resized() override
    {
        combo.setBounds(getLocalBounds().withSizeKeepingCentre(juce::jmin(getWidth(), 150), 24));
    }

private:
    struct ComboWithItems : public juce::ComboBox
    {
        explicit ComboWithItems(juce::RangedAudioParameter& param)
        {
            // Adding the list here in the constructor means that the combo
            // is already populated when we construct the attachment below
            addItemList(dynamic_cast<juce::AudioParameterChoice&>(param).choices, 1);
        }
    };

    ComboWithItems combo;
    juce::Label label;
    juce::ComboBoxParameterAttachment attachment;
};

class AttachedToggle : public juce::Component
{
public:
    explicit AttachedToggle(juce::RangedAudioParameter& param)
        : attachment(param, toggle)
    {
        addAndMakeVisible(toggle);
        toggle.setButtonText(param.getName(64));
    }

    void resized() override { toggle.setBounds(getLocalBounds().reduced(20)); }
private:
    juce::ToggleButton toggle{ "toggle" };
    juce::ButtonParameterAttachment attachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AttachedToggle)
};

class TraceAndGrid : public juce::Component
{
public:
    TraceAndGrid(EQParameterReference& ref)
        : responseTrace(ref)
    {
        setOpaque(true);
        setBufferedToImage(true);
        setInterceptsMouseClicks(false, false);
        setPaintingIsUnclipped(true);
        setRepaintsOnMouseActivity(false);

        const auto frequencyRange = gedd::createFrequencyRange(20.0, 22000.0);
        const auto decibelRange = juce::NormalisableRange<double>(-24.0, 24.0);

        responseTrace.setFrequencyNormalisableRange(frequencyRange);
        responseTrace.setDecibelNormalisableRange(decibelRange);
        grid.setFrequencyNormalisableRange(frequencyRange);
        grid.setDecibelNormalisableRange(decibelRange);

        addAndMakeVisible(grid);
        addAndMakeVisible(responseTrace);
    }

    void paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        const auto bounds = getLocalBounds();

        grid.setBounds(bounds);
        responseTrace.setBounds(bounds);
    }

private:
    VASVFTraceComponent responseTrace;
    FrequencyDecibelGridOverlay grid;
};

//==============================================================================
/**
*/
class GeddvasvfAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    GeddvasvfAudioProcessorEditor (GeddvasvfAudioProcessor&);
    ~GeddvasvfAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    GeddvasvfAudioProcessor& audioProcessor;

    AttachedSlider freqSlider;
    AttachedSlider qSlider;
    AttachedSlider gainSlider;
    AttachedCombo  filterTypeCombo;
    AttachedToggle autoqToggle;
    TraceAndGrid   responseTrace;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeddvasvfAudioProcessorEditor)
};
