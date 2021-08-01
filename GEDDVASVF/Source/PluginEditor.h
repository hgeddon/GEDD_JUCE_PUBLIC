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
        setInterceptsMouseClicks(false, true);
        setPaintingIsUnclipped(true);
        setRepaintsOnMouseActivity(false);

        const auto frequencyRange = gedd::createFrequencyRange(20.0, 22000.0);
        const auto decibelRange = juce::NormalisableRange<double>(-24.0, 24.0);

        dbRangeSlider.setSliderStyle(juce::Slider::SliderStyle::TwoValueVertical);
        dbRangeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        dbRangeSlider.setRange(juce::Range<double>(-48.0, 48.0), 1.0);
        dbRangeSlider.setMinAndMaxValues(-24.0, 24.0, juce::NotificationType::dontSendNotification);
        dbRangeSlider.onValueChange = [&] { 
            const auto newRange = juce::NormalisableRange<double>(dbRangeSlider.getMinValue(), dbRangeSlider.getMaxValue());
            responseTrace.setDecibelNormalisableRange(newRange);
            grid.setDecibelNormalisableRange(newRange);
        };

        freqRangeSlider.setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
        freqRangeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        freqRangeSlider.setNormalisableRange(gedd::createFrequencyRange(1.0, 22050.0));
        freqRangeSlider.setMinAndMaxValues(20.0, 18000.0, juce::NotificationType::dontSendNotification);
        freqRangeSlider.onValueChange = [&] {
            const auto newRange = gedd::createFrequencyRange(freqRangeSlider.getMinValue(), freqRangeSlider.getMaxValue());
            responseTrace.setFrequencyNormalisableRange(newRange);
            grid.setFrequencyNormalisableRange(newRange);
        };

        responseTrace.setFrequencyNormalisableRange(frequencyRange);
        responseTrace.setDecibelNormalisableRange(decibelRange);
        grid.setFrequencyNormalisableRange(frequencyRange);
        grid.setDecibelNormalisableRange(decibelRange);

        addAndMakeVisible(grid);
        addAndMakeVisible(responseTrace);
        addAndMakeVisible(dbRangeSlider);
        addAndMakeVisible(freqRangeSlider);
    }

    void paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        auto bounds = getLocalBounds();

        dbRangeSlider.setBounds(bounds.removeFromLeft(30));
        freqRangeSlider.setBounds(bounds.removeFromTop(30));

        grid.setBounds(bounds);
        responseTrace.setBounds(bounds);
    }

private:
    VASVFTraceComponent responseTrace;
    FrequencyDecibelGridOverlay grid;

    juce::Slider dbRangeSlider{ "dbRange" };
    juce::Slider freqRangeSlider{ "freqRange" };
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
