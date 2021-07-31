/*
  ==============================================================================

    VASVFProcessor.cpp
    Created: 28 Jul 2021 10:06:34pm
    Author:  GEDD

  ==============================================================================
*/

#include "VASVFProcessor.h"

namespace gedd
{
namespace dsp
{

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setType(FilterType t) noexcept
    {
        if (t != filterType)
        {
            filterType = t;

            shouldUpdate = true;
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setFrequency(SampleType f) noexcept
    {
        jassert(juce::isPositiveAndNotGreaterThan(f, sampleRate * 0.5));

        if (f != frequency.getCurrentValue())
        {
            frequency.setTargetValue(f);

            shouldUpdate = true;
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setGain(SampleType g) noexcept
    {
        if (g != gain.getCurrentValue())
        {
            gain.setTargetValue(g);

            shouldUpdate = true;
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setQ(SampleType newq) noexcept
    {
        if (newq != q.getCurrentValue())
        {
            q.setTargetValue(newq);

            shouldUpdate = true;
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setAutoQ(bool aq) noexcept
    {
        if (aq != autoQ)
        {
            autoQ = aq;

            shouldUpdate = true;
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::setRampDurationSeconds(double newRampDurationSeconds) noexcept
    {
        if (newRampDurationSeconds != rampDurationSeconds)
        {
            rampDurationSeconds = newRampDurationSeconds;

            reset();
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::prepare(const juce::dsp::ProcessSpec& spec) noexcept
    {
        sampleRate = spec.sampleRate;

        //update();   // create state from defaults
        filterProcessor.prepare(spec);

        reset();
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::reset() noexcept
    {
        filterProcessor.reset();

        if (sampleRate != 0.0)
        {
            frequency.reset(sampleRate, rampDurationSeconds);
            gain.reset(sampleRate, rampDurationSeconds);
            q.reset(sampleRate, rampDurationSeconds);
        }
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::skip(int numSampleToSkip) noexcept
    {
        frequency.skip(numSampleToSkip);
        gain.skip(numSampleToSkip);
        q.skip(numSampleToSkip);
    }

    template<typename SampleType>
    void VASVFProcessor<SampleType>::update() noexcept
    {
        jassert(sampleRate > 0);

        if (!shouldUpdate) return;

        const auto sf = frequency.getNextValue();
        const auto sg = gain.getNextValue();
        const auto sq = q.getNextValue();
        
        jassert(sf > 0);
        jassert(sq > 0);

        VASVF::State<SampleType>::Ptr newState;

        switch (filterType)
        {
        //case FilterType::none:      newState = new VASVF::State<SampleType>(); break;
        case FilterType::lowpass:   newState = VASVF::State<SampleType>::makeLowpass(sampleRate, sf, sg, sq, autoQ);   break;
        case FilterType::bandpass:  newState = VASVF::State<SampleType>::makeBandpass(sampleRate, sf, sg, sq, autoQ);  break;
        case FilterType::highpass:  newState = VASVF::State<SampleType>::makeHighpass(sampleRate, sf, sg, sq, autoQ);  break;
        case FilterType::notch:     newState = VASVF::State<SampleType>::makeNotch(sampleRate, sf, sg, sq, autoQ);     break;
        case FilterType::allpass:   newState = VASVF::State<SampleType>::makeAllpass(sampleRate, sf, sg, sq, autoQ);   break;
        case FilterType::bell:      newState = VASVF::State<SampleType>::makeBell(sampleRate, sf, sg, sq, autoQ);      break;
        case FilterType::lowshelf:  newState = VASVF::State<SampleType>::makeLowshelf(sampleRate, sf, sg, sq, autoQ);  break;
        case FilterType::highshelf: newState = VASVF::State<SampleType>::makeHighshelf(sampleRate, sf, sg, sq, autoQ); break;
        }

        if (newState)
        {
            *filterProcessor.state = *newState;
        }

        if (!frequency.isSmoothing() &&
            !gain.isSmoothing() &&
            !q.isSmoothing())
        {
            shouldUpdate = false;
        }
    }

    /*template<typename SampleType>
    SampleType VASVFProcessor<SampleType>::processSample(SampleType v0) noexcept
    {
        return filterProcessor.processSample(v0);
    }*/


//=====================================
template class VASVFProcessor<float>;
template class VASVFProcessor<double>;

}   // namespace dsp
}   // namespace gedd