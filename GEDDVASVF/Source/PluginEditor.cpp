/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeddvasvfAudioProcessorEditor::GeddvasvfAudioProcessorEditor (GeddvasvfAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p),
    freqSlider(p.getParameterReferences().eqParamRef.freq),
    qSlider(p.getParameterReferences().eqParamRef.q),
    gainSlider(p.getParameterReferences().eqParamRef.gain),
    filterTypeCombo(p.getParameterReferences().eqParamRef.type),
    autoqToggle(p.getParameterReferences().eqParamRef.autoq),
    responseTrace(p.getParameterReferences().eqParamRef)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

    addAndMakeVisible(filterTypeCombo);
    addAndMakeVisible(freqSlider);
    addAndMakeVisible(qSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(autoqToggle);
    addAndMakeVisible(responseTrace);
    addAndMakeVisible(parameterSmoothingSlider);

    parameterSmoothingSliderLabel.attachToComponent(&parameterSmoothingSlider, false);
    parameterSmoothingSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    parameterSmoothingSlider.setTextBoxStyle(juce::Slider::TextEntryBoxPosition::TextBoxBelow, false, 80, 30);
    parameterSmoothingSlider.setTextValueSuffix(" s");
    parameterSmoothingSlider.setRange(juce::Range<double>(0.001, 1.0), 0.0);
    parameterSmoothingSlider.setNumDecimalPlacesToDisplay(3);

    parameterSmoothingSlider.onValueChange = [&] {
        audioProcessor.getSvfProcessorRef().setRampDurationSeconds(parameterSmoothingSlider.getValue());
    };
}

GeddvasvfAudioProcessorEditor::~GeddvasvfAudioProcessorEditor()
{
}

//==============================================================================
void GeddvasvfAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void GeddvasvfAudioProcessorEditor::resized()
{
    const auto sliderWidth = 80;
    const auto comboHeight = 80;

    auto bounds = getLocalBounds();
    auto controlRegion = bounds.removeFromLeft(sliderWidth * 4);

    autoqToggle.setBounds(controlRegion.removeFromTop(comboHeight));
    filterTypeCombo.setBounds(controlRegion.removeFromTop(comboHeight));

    freqSlider.setBounds(controlRegion.removeFromLeft(sliderWidth));
    qSlider.setBounds(controlRegion.removeFromLeft(sliderWidth));
    gainSlider.setBounds(controlRegion.removeFromLeft(sliderWidth));

    parameterSmoothingSlider.setBounds(controlRegion);

    responseTrace.setBounds(bounds.reduced(30));
}
