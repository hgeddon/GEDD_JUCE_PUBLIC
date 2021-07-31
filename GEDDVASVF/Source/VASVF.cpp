/*
  ==============================================================================

    VAVCF.cpp
    Created: 6 Jul 2021 7:08:30pm
    Author:  GEDD

  ==============================================================================
*/

#include "VASVF.h"

namespace gedd {
namespace dsp {
namespace VASVF {

//====================================================
template<typename NumericType>
State<NumericType>::State()
    : data({NumericType(), NumericType(), NumericType(),
            NumericType(), NumericType(), NumericType(), 
            NumericType(), NumericType(), NumericType()})
{
    DBG("null state constructor");
}

template<typename NumericType>
State<NumericType>::State(NumericType a, NumericType g, NumericType k,
                          NumericType m0, NumericType m1, NumericType m2)
{
    jassert(a != 0);
    jassert(g != 0);
    jassert(k != 0);

    const auto a1 = static_cast<NumericType>(1) / (static_cast<NumericType>(1) + g * (g + k));
    const auto a2 = g * a1;
    const auto a3 = g * a2;

    data[static_cast<size_t>(0)] = a;
    data[static_cast<size_t>(1)] = g;
    data[static_cast<size_t>(2)] = k;
    data[static_cast<size_t>(3)] = m0;
    data[static_cast<size_t>(4)] = m1;
    data[static_cast<size_t>(5)] = m2;
    data[static_cast<size_t>(6)] = a1;
    data[static_cast<size_t>(7)] = a2;
    data[static_cast<size_t>(8)] = a3;
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeLowpass(double sampleRate, NumericType frequency, NumericType q)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = static_cast<NumericType>(1); // not used
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / q;

    return *new State(a, g, k, 0, 0, 1);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeLowpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    return makeLowpass(sampleRate, frequency, calculateAutoQ(q, gain, autoQ));
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeBandpass(double sampleRate, NumericType frequency, NumericType q)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = static_cast<NumericType>(1); // not used
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / q;

    return *new State(a, g, k, 0, 1, 0);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeBandpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    return makeBandpass(sampleRate, frequency, calculateAutoQ(q, gain, autoQ));
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeHighpass(double sampleRate, NumericType frequency, NumericType q)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = static_cast<NumericType>(1); // not used
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / q;

    return *new State(a, g, k, 1, -k, -1);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeHighpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    return makeHighpass(sampleRate, frequency, calculateAutoQ(q, gain, autoQ));
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeNotch(double sampleRate, NumericType frequency, NumericType q)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = static_cast<NumericType>(1); // not used
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / q;

    return *new State(a, g, k, 1, -k, 0);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeNotch(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    return makeNotch(sampleRate, frequency, calculateAutoQ(q, gain, autoQ));
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeAllpass(double sampleRate, NumericType frequency, NumericType q)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = static_cast<NumericType>(1); // not used
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / q;

    return *new State(a, g, k, 1, -2 * k, 0);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeAllpass(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    return makeAllpass(sampleRate, frequency, calculateAutoQ(q, gain, autoQ));
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeBell(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = std::pow(static_cast<NumericType>(10), gain * static_cast<NumericType>(0.025));
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi);
    const auto k = static_cast<NumericType>(1) / (calculateAutoQ(q, gain, autoQ) * a);

    return *new State(a, g, k, 1, k * (a * a - 1), 0);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeLowshelf(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = std::pow(static_cast<NumericType>(10), gain * static_cast<NumericType>(0.025));
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi) / std::sqrt(a);
    const auto k = static_cast<NumericType>(1) / calculateAutoQ(q, gain, autoQ);

    return *new State(a, g, k, 1, k * (a - 1), a * a - 1);
}

template<typename NumericType>
typename State<NumericType>::Ptr State<NumericType>::makeHighshelf(double sampleRate, NumericType frequency, NumericType gain, NumericType q, bool autoQ)
{
    jassert(sampleRate > 0.0);
    jassert(juce::isPositiveAndNotGreaterThan(frequency, sampleRate * 0.5));
    jassert(q > static_cast<NumericType>(0));

    const auto a = std::pow(static_cast<NumericType>(10), gain * static_cast<NumericType>(0.025));
    const auto g = std::tan(frequency / sampleRate * juce::MathConstants<double>::pi) * std::sqrt(a);
    const auto k = static_cast<NumericType>(1) / calculateAutoQ(q, gain, autoQ);

    return *new State(a, g, k, a * a, k * (1 - a) * a, 1 - a * a);
}

template<typename NumericType>
double State<NumericType>::getMagnitudeForFrequency(double frequency, double sampleRate) const noexcept
{
    auto z = std::exp(juce::dsp::Complex<double>(0.0, -2.0 * juce::MathConstants<double>::pi) * frequency / sampleRate);

    const auto g  = static_cast<double>(data[static_cast<size_t>(1)]);
    const auto k  = static_cast<double>(data[static_cast<size_t>(2)]);
    const auto m0 = static_cast<double>(data[static_cast<size_t>(3)]);
    const auto m1 = static_cast<double>(data[static_cast<size_t>(4)]);
    const auto m2 = static_cast<double>(data[static_cast<size_t>(5)]);

    const auto gsq    = g * g;
    const auto zsq    = z * z;
    const auto twoz   = z * 2.0;
    const auto gm1    = g * m1;
    const auto gk     = g * k;
    const auto twogsq = gsq * 2.0;

    auto numerator   = gsq * m2 * (zsq + twoz + 1.0) - gm1 * (zsq - 1.0);
    auto denominator = gsq + gk + zsq * (gsq - gk + 1.0) + z * (twogsq - 2.0) + 1.0;

    return std::abs(m0 + (numerator / denominator));
}

template<typename NumericType>
void State<NumericType>::getMagnitudeForFrequencyArray(const double* frequencies, double* magnitudes, size_t numSamples, double sampleRate) const noexcept
{
    for (auto sample = 0; sample != numSamples; ++sample)
        magnitudes[sample] = getMagnitudeForFrequency(frequencies[sample], sampleRate);
}

template<typename NumericType>
double State<NumericType>::getPhaseForFrequency(double frequency, double sampleRate) const noexcept
{
    auto z = std::exp(juce::dsp::Complex<double>(0.0, -2.0 * juce::MathConstants<double>::pi) * frequency / sampleRate);

    const auto g  = static_cast<double>(data[static_cast<size_t>(1)]);
    const auto k  = static_cast<double>(data[static_cast<size_t>(2)]);
    const auto m0 = static_cast<double>(data[static_cast<size_t>(3)]);
    const auto m1 = static_cast<double>(data[static_cast<size_t>(4)]);
    const auto m2 = static_cast<double>(data[static_cast<size_t>(5)]);

    const auto gsq    = g * g;
    const auto zsq    = z * z;
    const auto twoz   = z * 2.0;
    const auto gm1    = g * m1;
    const auto gk     = g * k;
    const auto twogsq = gsq * 2.0;

    auto numerator   = gsq * m2 * (zsq + twoz + 1.0) - gm1 * (zsq - 1.0);
    auto denominator = gsq + gk + zsq * (gsq - gk + 1.0) + z * (twogsq - 2.0) + 1.0;

    return std::arg(m0 + (numerator / denominator));
}

template<typename NumericType>
void State<NumericType>::getPhaseForFrequencyArray(const double* frequencies, double* phases, size_t numSamples, double sampleRate) const noexcept
{
    for (auto sample = 0; sample != numSamples; ++sample)
        phases[sample] = getPhaseForFrequency(frequencies[sample], sampleRate);
}

template<typename NumericType>
NumericType State<NumericType>::calculateAutoQ(NumericType q, NumericType gain, bool aq) noexcept
{   
    if (aq)
        return juce::jmin((q * static_cast<NumericType>(0.5)) * (std::pow(static_cast<NumericType>(10), std::abs(gain) * static_cast<NumericType>(0.05))), static_cast<NumericType>(10));
    else
        return q;
}

//=====
template struct State<float>;
template struct State<double>;

//======================
template<typename SampleType>
Filter<SampleType>::Filter()
    : coeffs(new State<typename Filter<SampleType>::NumericType>(1, 1, 1, 0, 0, 0))
{
    reset();
}

template<typename SampleType>
Filter<SampleType>::Filter(StatePtr s)
    : coeffs(std::move(s))
{
    reset();
}

template<typename SampleType>
void Filter<SampleType>::reset(SampleType resetToValue)
{
    std::fill(iceq.begin(), iceq.end(), resetToValue);
}

template<typename SampleType>
void Filter<SampleType>::prepare(const juce::dsp::ProcessSpec& spec) noexcept
{
    jassert(spec.sampleRate > 0);
    jassert(spec.numChannels > 0);

    reset();
}

template<typename SampleType>
void Filter<SampleType>::snapToZero() noexcept
{
    const auto num = iceq.size();

    for (auto i = 0; i != num; ++i)
        juce::dsp::util::snapToZero(iceq[i]);
}

template<typename SampleType>
SampleType JUCE_VECTOR_CALLTYPE Filter<SampleType>::processSample(SampleType v0) noexcept
{
    jassert(coeffs != nullptr);

    const auto& s = coeffs->data;

    auto& ic1 = iceq[static_cast<size_t>(0)];
    auto& ic2 = iceq[static_cast<size_t>(1)];

    const auto v3 = v0 - ic2;
    const auto v1 = s[static_cast<size_t>(6)] * ic1 + s[static_cast<size_t>(7)] * v3;
    const auto v2 = ic2 + s[static_cast<size_t>(7)] * ic1 + s[static_cast<size_t>(8)] * v3;

    // try clipping? - nope, doesn't do much good here...
    //ic1 = std::tanh(static_cast<SampleType>(2) * v1 - ic1);
    //ic2 = std::tanh(static_cast<SampleType>(2) * v2 - ic2);

    ic1 = static_cast<SampleType>(2) * v1 - ic1;
    ic2 = static_cast<SampleType>(2) * v2 - ic2;

    return s[static_cast<size_t>(3)] * v0 + s[static_cast<size_t>(4)] * v1 + s[static_cast<size_t>(5)] * v2;
}

template class Filter<float>;
template class Filter<double>;

}   // namespace VASVF
}   // namespace dsp
}   // namespace gedd
