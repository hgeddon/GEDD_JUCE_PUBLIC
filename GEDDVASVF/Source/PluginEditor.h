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

        // lambda
        dbRangeSlider.onValueChange = [&] { 
            // Validate
            const auto min = dbRangeSlider.getMinValue();
            const auto max = dbRangeSlider.getMaxValue();

            if (min == max) return;

            if (max < min)
            {
                dbRangeSlider.setMinValue(max, juce::NotificationType::dontSendNotification);
                dbRangeSlider.setMaxValue(min, juce::NotificationType::dontSendNotification);
            }

            // update
            const auto newRange = juce::NormalisableRange<double>(dbRangeSlider.getMinValue(), dbRangeSlider.getMaxValue());
            responseTrace.setDecibelNormalisableRange(newRange);
            grid.setDecibelNormalisableRange(newRange);

            dbLabelBottom.setText(juce::String(static_cast<int>(min)), juce::NotificationType::dontSendNotification);
            dbLabelTop.setText(juce::String(static_cast<int>(max)), juce::NotificationType::dontSendNotification);
        };

        freqRangeSlider.setSliderStyle(juce::Slider::SliderStyle::TwoValueHorizontal);
        freqRangeSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
        freqRangeSlider.setNormalisableRange(gedd::createFrequencyRange(1.0, 22050.0, 1.0));
        freqRangeSlider.setMinAndMaxValues(20.0, 18000.0, juce::NotificationType::dontSendNotification);

        // lambda
        freqRangeSlider.onValueChange = [&] {
            // Validate
            const auto min = freqRangeSlider.getMinValue();
            const auto max = freqRangeSlider.getMaxValue();

            if (min == max) return;

            if (max < min)
            {
                freqRangeSlider.setMinValue(max, juce::NotificationType::dontSendNotification);
                freqRangeSlider.setMaxValue(min, juce::NotificationType::dontSendNotification);
            }

            // Update
            const auto newRange = gedd::createFrequencyRange(freqRangeSlider.getMinValue(), freqRangeSlider.getMaxValue());
            responseTrace.setFrequencyNormalisableRange(newRange);
            grid.setFrequencyNormalisableRange(newRange);

            freqLabelLeft.setText(juce::String(static_cast<int>(min)), juce::NotificationType::dontSendNotification);
            freqLabelRight.setText(juce::String(static_cast<int>(max)), juce::NotificationType::dontSendNotification);
        };

        showMagnitudeToggle.setToggleState(responseTrace.getShowMagnitudeTrace(), juce::NotificationType::dontSendNotification);
        showMagnitudeToggle.onStateChange = [&] {
            responseTrace.setShowMagnitudeTrace(showMagnitudeToggle.getToggleState());
        };

        showPhaseToggle.setToggleState(responseTrace.getShowPhaseTrace(), juce::NotificationType::dontSendNotification);
        showPhaseToggle.onStateChange = [&] {
            responseTrace.setShowPhaseTrace(showPhaseToggle.getToggleState());
        };

        responseTrace.setFrequencyNormalisableRange(frequencyRange);
        responseTrace.setDecibelNormalisableRange(decibelRange);
        grid.setFrequencyNormalisableRange(frequencyRange);
        grid.setDecibelNormalisableRange(decibelRange);

        // labels
        dbLabelTop.setColour(juce::Label::ColourIds::textColourId, juce::Colours::azure);
        dbLabelTop.setJustificationType(juce::Justification::centred);
        dbLabelTop.setText(juce::String(static_cast<int>(dbRangeSlider.getMaxValue())), juce::NotificationType::dontSendNotification);
        dbLabelTop.setEditable(true, false, true);
        dbLabelTop.onTextChange = [&] {
            auto t = dbLabelTop.getText().trimStart();

            while (t.startsWithChar('+'))
                t = t.substring(1).trimStart();

            auto valStr = t.initialSectionContainingOnly("0123456789.,-");
            auto valDbl = valStr.getDoubleValue();

            if (valDbl > dbRangeSlider.getMinValue())
            {
                dbRangeSlider.setMaxValue(valDbl, juce::NotificationType::sendNotificationAsync);
                dbLabelTop.setText(valStr, juce::NotificationType::dontSendNotification);
            }
            else
            {
                dbLabelTop.setText(juce::String(static_cast<int>(dbRangeSlider.getMaxValue())), juce::NotificationType::dontSendNotification);
            }
        };
        
        dbLabelBottom.setColour(juce::Label::ColourIds::textColourId, juce::Colours::azure);
        dbLabelBottom.setJustificationType(juce::Justification::centred);
        dbLabelBottom.setText(juce::String(static_cast<int>(dbRangeSlider.getMinValue())), juce::NotificationType::dontSendNotification);
        dbLabelBottom.setEditable(true, false, true);
        dbLabelBottom.onTextChange = [&] {
            auto t = dbLabelBottom.getText().trimStart();

            while (t.startsWithChar('+'))
                t = t.substring(1).trimStart();

            auto valStr = t.initialSectionContainingOnly("0123456789.,-");
            auto valDbl = valStr.getDoubleValue();

            if (valDbl < dbRangeSlider.getMaxValue())
            {
                dbRangeSlider.setMinValue(valDbl, juce::NotificationType::sendNotificationAsync);
                dbLabelBottom.setText(valStr, juce::NotificationType::dontSendNotification);
            }
            else
            {
                dbLabelBottom.setText(juce::String(static_cast<int>(dbRangeSlider.getMinValue())), juce::NotificationType::dontSendNotification);
            }
        };

        freqLabelLeft.setColour(juce::Label::ColourIds::textColourId, juce::Colours::azure);
        freqLabelLeft.setJustificationType(juce::Justification::centredRight);
        freqLabelLeft.setText(juce::String(static_cast<int>(freqRangeSlider.getMinValue())), juce::NotificationType::dontSendNotification);
        freqLabelLeft.setEditable(true, false, true);
        freqLabelLeft.onTextChange = [&] {
            auto t = freqLabelLeft.getText().trimStart();

            while (t.startsWithChar('+'))
                t = t.substring(1).trimStart();

            auto valStr = t.initialSectionContainingOnly("0123456789.,-");
            auto valDbl = valStr.getDoubleValue();

            if (valDbl < freqRangeSlider.getMaxValue())
            {
                freqRangeSlider.setMinValue(valDbl, juce::NotificationType::sendNotificationAsync);
                freqLabelLeft.setText(valStr, juce::NotificationType::dontSendNotification);
            }
            else
            {
                freqLabelLeft.setText(juce::String(static_cast<int>(freqRangeSlider.getMinValue())), juce::NotificationType::dontSendNotification);
            }
        };

        freqLabelRight.setColour(juce::Label::ColourIds::textColourId, juce::Colours::azure);
        freqLabelRight.setJustificationType(juce::Justification::centredLeft);
        freqLabelRight.setText(juce::String(static_cast<int>(freqRangeSlider.getMaxValue())), juce::NotificationType::dontSendNotification);
        freqLabelRight.setEditable(true, false, true);
        freqLabelRight.onTextChange = [&] {
            auto t = freqLabelRight.getText().trimStart();

            while (t.startsWithChar('+'))
                t = t.substring(1).trimStart();

            auto valStr = t.initialSectionContainingOnly("0123456789.,-");
            auto valDbl = valStr.getDoubleValue();

            if (valDbl > freqRangeSlider.getMinValue())
            {
                freqRangeSlider.setMaxValue(valDbl, juce::NotificationType::sendNotificationAsync);
                freqLabelRight.setText(valStr, juce::NotificationType::dontSendNotification);
            }
            else
            {
                freqLabelRight.setText(juce::String(static_cast<int>(freqRangeSlider.getMaxValue())), juce::NotificationType::dontSendNotification);
            }
        };

        // add to component and make visible
        addAndMakeVisible(grid);
        addAndMakeVisible(responseTrace);

        addAndMakeVisible(dbRangeSlider);
        addAndMakeVisible(freqRangeSlider);
        addAndMakeVisible(showMagnitudeToggle);
        addAndMakeVisible(showPhaseToggle);

        addAndMakeVisible(dbLabelTop);
        addAndMakeVisible(dbLabelBottom);
        addAndMakeVisible(freqLabelLeft);
        addAndMakeVisible(freqLabelRight);
    }

    void paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colours::black);
    }

    void resized() override
    {
        const auto elHeight = 30;
        const auto elWidth = 40;

        auto bounds = getLocalBounds();

        auto toggleRegion = bounds.removeFromTop(elHeight);

        showMagnitudeToggle.setBounds(toggleRegion.removeFromLeft(toggleRegion.getWidth() / 2));
        showPhaseToggle.setBounds(toggleRegion);

        // draw freq range
        auto freqRangeRegion = bounds.removeFromTop(elHeight);

        freqLabelLeft.setBounds(freqRangeRegion.removeFromLeft(elWidth));
        freqLabelRight.setBounds(freqRangeRegion.removeFromRight(elWidth));
        freqRangeSlider.setBounds(freqRangeRegion);

        // draw db range
        auto dbRangeRegion = bounds.removeFromLeft(elWidth);

        dbLabelTop.setBounds(dbRangeRegion.removeFromTop(elHeight));
        dbLabelBottom.setBounds(dbRangeRegion.removeFromBottom(elHeight));
        dbRangeSlider.setBounds(dbRangeRegion);

        // draw grid and trace
        grid.setBounds(bounds);
        responseTrace.setBounds(bounds);
    }

private:
    VASVFTraceComponent responseTrace;
    FrequencyDecibelGridOverlay grid;

    juce::Slider dbRangeSlider{ "dbRange" };
    juce::Slider freqRangeSlider{ "freqRange" };
    juce::ToggleButton showMagnitudeToggle{ "mag" };
    juce::ToggleButton showPhaseToggle{ "phase" };
    juce::Label dbLabelTop, dbLabelBottom, freqLabelLeft, freqLabelRight;
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

    // ! warning ! - this parameter will interrupt audio processing and so is not to be automated
    juce::Slider parameterSmoothingSlider{ "smoothing" };
    juce::Label parameterSmoothingSliderLabel{ "", parameterSmoothingSlider.getName() };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeddvasvfAudioProcessorEditor)
};
