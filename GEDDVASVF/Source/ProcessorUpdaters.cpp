/*
  ==============================================================================

    ProcessorUpdaters.cpp
    Created: 8 Jul 2021 7:55:16pm
    Author:  GEDD

  ==============================================================================
*/

#include "ProcessorUpdaters.h"

template<typename SampleType>
VASVFProcessorUpdater<SampleType>::VASVFProcessorUpdater(EQParameterReference& ref, gedd::dsp::VASVFProcessor<SampleType>& p)
    : paramRef(ref), processor(p)
{
    paramRef.type .addListener(this);
    paramRef.freq .addListener(this);
    paramRef.gain .addListener(this);
    paramRef.q    .addListener(this);
    paramRef.autoq.addListener(this);
}

template<typename SampleType>
VASVFProcessorUpdater<SampleType>::~VASVFProcessorUpdater()
{
    paramRef.type .removeListener(this);
    paramRef.freq .removeListener(this);
    paramRef.gain .removeListener(this);
    paramRef.q    .removeListener(this);
    paramRef.autoq.removeListener(this);
}

template<typename SampleType>
void VASVFProcessorUpdater<SampleType>::updateProcessor() noexcept
{
    if (requiresUpdate.load())
    {
        processor.setType(static_cast<gedd::dsp::VASVF::FilterType>(paramRef.type.getIndex()));
        processor.setFrequency(paramRef.freq.get());
        processor.setGain(paramRef.gain.get());
        processor.setQ(paramRef.q.get());
        processor.setAutoQ(paramRef.autoq.get());

        requiresUpdate.store(false);
    }
}

template<typename SampleType>
void VASVFProcessorUpdater<SampleType>::parameterValueChanged(int parameterIndex, float newValue)
{
    juce::ignoreUnused(parameterIndex, newValue);
    requiresUpdate.store(true);
}

template<typename SampleType>
void VASVFProcessorUpdater<SampleType>::parameterGestureChanged(int parameterIndex, bool gestureIsStarting)
{
    juce::ignoreUnused(parameterIndex, gestureIsStarting);
}

//==================
template class VASVFProcessorUpdater<float>;
template class VASVFProcessorUpdater<double>;
