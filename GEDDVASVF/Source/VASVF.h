/*
  ==============================================================================

    VAVCF.h
    Created: 6 Jul 2021 7:08:30pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace gedd {
namespace dsp {
namespace VASVF {

    // enum
    enum class FilterType
    {
        none = 0,
        lowpass,
        bandpass,
        highpass,
        notch,
        allpass,
        bell,
        lowshelf,
        highshelf,
        numTypes
    };

    // static string array
    static constexpr auto filterTypeStr = {
        "none",
        "lowpass",
        "bandpass",
        "highpass",
        "notch",
        "allpass",
        "bell",
        "lowshelf",
        "highshelf"
    };

    template<typename NumericType>
    struct State;

    template <typename SampleType>
    class Filter
    {
    public:
        using NumericType = typename juce::dsp::SampleTypeHelpers::ElementType<SampleType>::Type;

        using StatePtr = typename State<NumericType>::Ptr;

        // Constructor
        Filter();

        // Create filter with given coeffs
        Filter(StatePtr stateToUse);

        Filter(const Filter&) = default;    // copy constructor
        Filter(Filter&&) = default;         // move constructor
        Filter& operator=(const Filter&) = default; // copy assignment operator
        Filter& operator=(Filter&&) = default;  // move assignment operator

        StatePtr coeffs;

        // Dsp methods
        void prepare(const juce::dsp::ProcessSpec& spec) noexcept;

        void reset() noexcept { reset(static_cast<SampleType>(0)); }

        void reset(SampleType resetToValue);

        void snapToZero() noexcept;

        // if only using this then need to also call updateStateIfRequired(), skip() and snapToZero()
        SampleType JUCE_VECTOR_CALLTYPE processSample(SampleType v0) noexcept;

        template<typename ProcessContext>
        void process(const ProcessContext& context) noexcept
        {
            static_assert(std::is_same<typename ProcessContext::SampleType, SampleType>::value,
                "The sample-type of the VASVF filter must match the sample-type supplied to this process callback");

            jassert(coeffs != nullptr);

            auto&& inputBlock = context.getInputBlock();
            auto&& outputBlock = context.getOutputBlock();

            jassert(inputBlock.getNumChannels() == 1);
            jassert(inputBlock.getNumChannels() == outputBlock.getNumChannels());
            jassert(inputBlock.getNumSamples() == outputBlock.getNumSamples());

            if (context.isBypassed)
            {
                if (context.usesSeparateInputAndOutputBlocks)
                    outputBlock.copyFrom(inputBlock);

                return;
            }

            const auto numSamples = inputBlock.getNumSamples();
            const auto* src = inputBlock.getChannelPointer(0);
            auto* dst = outputBlock.getChannelPointer(0);

            for (auto sample = 0; sample != numSamples; ++sample)
            {
                dst[sample] = processSample(src[sample]);
            }
#if JUCE_SNAP_TO_ZERO
            snapToZero();
#endif
        }

    private:
        std::array<SampleType, 2> iceq;

        JUCE_LEAK_DETECTOR(Filter)
    };

    template<typename NumericType>
    struct State : public juce::dsp::ProcessorState
    {
        // create null set of coeffs
        State();

        // create with coeffs
        State(NumericType a, NumericType g, NumericType k, 
              NumericType m0, NumericType m1, NumericType m2);

        State(const State&) = default;
        State(State&&) = default;
        State& operator=(const State&) = default;
        State& operator=(State&&) = default;

        using Ptr = juce::ReferenceCountedObjectPtr<State>;

        static Ptr makeLowpass(double sampleRate, NumericType frequency, NumericType q = inverseRootTwo);

        static Ptr makeLowpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ);

        static Ptr makeBandpass(double sampleRate, NumericType frequency, NumericType q = inverseRootTwo);

        static Ptr makeBandpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ);

        static Ptr makeHighpass(double sampleRate, NumericType frequency, NumericType q = inverseRootTwo);

        static Ptr makeHighpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ);

        static Ptr makeNotch(double sampleRate, NumericType frequency, NumericType q = inverseRootTwo);

        static Ptr makeNotch(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ);

        static Ptr makeAllpass(double sampleRate, NumericType frequency, NumericType q = inverseRootTwo);

        static Ptr makeAllpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ);

        static Ptr makeBell(double sampleRate, NumericType frequency, NumericType gain, NumericType q = inverseRootTwo, bool autoQ = false);

        static Ptr makeLowshelf(double sampleRate, NumericType frequency, NumericType gain, NumericType q = inverseRootTwo, bool autoQ = false);

        static Ptr makeHighshelf(double sampleRate, NumericType frequency, NumericType gain, NumericType q = inverseRootTwo, bool autoQ = false);

        double getMagnitudeForFrequency(double frequency, double sampleRate) const noexcept;

        void getMagnitudeForFrequencyArray(const double* frequencies, double* magnitudes, size_t numSamples, double sampleRate) const noexcept;

        double getPhaseForFrequency(double frequency, double sampleRate) const noexcept;

        void getPhaseForFrequencyArray(const double* frequencies, double* phases, size_t numSamples, double sampleRate) const noexcept;

        static NumericType calculateAutoQ(NumericType q, NumericType gain, bool aq) noexcept;

        std::array<NumericType, 9> data;    // a, g, k, m0, m1, m2, a1, a2, a3

        static constexpr NumericType inverseRootTwo = static_cast<NumericType> (0.70710678118654752440L);
    };

}   // namespace VASVF
}   // namespace dsp
}   // namespace gedd
