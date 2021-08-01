/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "AudioProcessorBase.h"
#include "ParameterReference.h"
#include "VASVF.h"
#include "ProcessorUpdaters.h"

//==============================================================================
/**
*/
class GeddvasvfAudioProcessor  : public gedd::AudioProcessorBase
{
public:
    //==============================================================================
    GeddvasvfAudioProcessor();
    ~GeddvasvfAudioProcessor() override;

    //==============================================================================
    void reset() override;
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;

    ParameterReferences& getParameterReferences() noexcept { return paramRef; }

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

private:
    ParameterReferences paramRef;

    ParameterLayout createLayout() override;

    gedd::dsp::VASVFProcessor<float> svfProcessor;
    VASVFProcessorUpdater<float> svfProcessorUpdater;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GeddvasvfAudioProcessor)
};
