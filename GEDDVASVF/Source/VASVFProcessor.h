/*
  ==============================================================================

    VASVFProcessor.h
    Created: 28 Jul 2021 10:06:34pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "VASVF.h"
#include "CommonFunctions.h"

namespace gedd
{
namespace dsp
{
    // Multi-mono container class for VASVF::Filter with smoothed values
    template<typename SampleType = float>
    class VASVFProcessor
    {
    public:
        using FilterType = VASVF::FilterType;

        VASVFProcessor() noexcept {};

        // setters
        void setType(FilterType t) noexcept;

        void setFrequency(SampleType f) noexcept;

        void setGain(SampleType g) noexcept;

        void setQ(SampleType q) noexcept;

        void setAutoQ(bool aq) noexcept;

        void setRampDurationSeconds(double newRampDurationSeconds) noexcept;

        // getters
        FilterType getType() const { return filterType; }

        SampleType getFrequency() const { return frequency.getCurrentValue(); }

        SampleType getGain() const { return gain.getCurrentValue(); }

        SampleType getQ() const { return q.getCurrentValue(); }

        bool getAutoQ() const { return autoQ; }

        // Dsp methods
        void prepare(const juce::dsp::ProcessSpec& spec) noexcept;

        void reset() noexcept;

        void skip(int numSampleToSkip) noexcept;

        void update() noexcept;

        template<typename ProcessContext = juce::dsp::ProcessContextReplacing<float>>
        void process(const ProcessContext& context) noexcept
        {
            const auto& inputBlock = context.getInputBlock();
            auto& outputBlock = context.getOutputBlock();
            
            jassert(inputBlock.getNumChannels() == outputBlock.getNumChannels());
            jassert(inputBlock.getNumSamples() == outputBlock.getNumSamples());

            const auto numSamples = outputBlock.getNumSamples();

            if (context.isBypassed || filterType == FilterType::none)
            {
                skip(numSamples);

                if (context.usesSeparateInputAndOutputBlocks)
                    outputBlock.copyFrom(inputBlock);

                return;
            }

            update();

            filterProcessor.process(context);
        }

    private:
        double sampleRate{ 0.0 }, rampDurationSeconds{ 0.05 };

        juce::dsp::ProcessorDuplicator<VASVF::Filter<SampleType>, VASVF::State<SampleType>> filterProcessor;
 
        bool shouldUpdate{ true };

        //=====================================================================
        VASVF::FilterType                       filterType  { FilterType::lowpass };
        bool                                    autoQ       { false };
        juce::LinearSmoothedValue<SampleType>   frequency   { 1000 };
        juce::LinearSmoothedValue<SampleType>   gain        { 0 };
        juce::LinearSmoothedValue<SampleType>   q           { gedd::MathConstants<SampleType>::reciprocalSqrt2 };

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VASVFProcessor)
    };

}   // namespace dsp
}   // namespace gedd
