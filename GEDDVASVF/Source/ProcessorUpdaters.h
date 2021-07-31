/*
  ==============================================================================

    ProcessorUpdaters.h
    Created: 8 Jul 2021 7:55:16pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ParameterReference.h"
#include "VASVFProcessor.h"

template<typename SampleType>
class VASVFProcessorUpdater : private juce::RangedAudioParameter::Listener
{
public:
    VASVFProcessorUpdater(EQParameterReference& ref, gedd::dsp::VASVFProcessor<SampleType>& p);
    
    ~VASVFProcessorUpdater() override;

    void updateProcessor() noexcept;

private:
    void parameterValueChanged(int parameterIndex, float newValue) override; // rangedAudioParameter::Listener

    // Unused pure virtual function
    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override; // rangedAudioParameter::Listener

    gedd::dsp::VASVFProcessor<SampleType>& processor;
    EQParameterReference& paramRef;
    std::atomic<bool> requiresUpdate{ true };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VASVFProcessorUpdater)
};
