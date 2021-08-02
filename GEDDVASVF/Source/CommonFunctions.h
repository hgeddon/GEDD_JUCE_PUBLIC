/*
  ==============================================================================

    CommonFunctions.h
    Created: 31 Jul 2021 4:25:36pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

namespace gedd
{
    // Beginning index for each custom component's ColourIDs
    namespace ComponentColourID
    {
        constexpr auto FrequencyDecibelGridOverlay  = 0x8800000;
        constexpr auto VASVFTraceComponent          = 0x8800100;
    }

    // Create a NormalisableRange of template type with frequency scaling
    template <typename Type = float>
    static juce::NormalisableRange<Type> createFrequencyRange(Type min, Type max, Type interval = static_cast<Type>(0))
    {
        // have to create it and move it if we want to set the 'interval' parameter
        auto x = juce::NormalisableRange<Type>(min, max,
            [](Type start, Type end, Type value) mutable    // from 0 to 1
            {
                const auto startLog = std::log(start);
                const auto endLog = std::log(end);
                return std::exp((value) * (endLog - startLog) + startLog);
            },
            [](Type start, Type end, Type value) mutable    // to 0 to 1
            {
                const auto startLog = std::log(start);
                const auto endLog = std::log(end);
                return (std::log(value) - startLog) / (endLog - startLog);
            });

        x.interval = interval;

        return std::move(x);
    }

    // Basic float to text / text to float conversion methods
    static juce::String floatValueToTextFunction(float x)           { return juce::String(x, 2); }
    static float floatTextToValueFunction(const juce::String& str)  { return str.getFloatValue(); }

    // Math Constants
    template <typename FloatType>
    struct MathConstants
    {
        /** A predefined value for 1 / pi */
        static constexpr FloatType reciprocalPi = static_cast<FloatType>(1 / 3.141592653589793238L);

        /** A predefined value for 1 / twopi */
        static constexpr FloatType reciprocalTwopi = static_cast<FloatType>(1 / (2 * 3.141592653589793238L));

        /** A predefined value for 1 / halfpi */
        static constexpr FloatType reciprocalHalfpi = static_cast<FloatType>(1 / (3.141592653589793238L / 2));

        /** A predefined value for 1 / euler's number */
        static constexpr FloatType reciprocalEuler = static_cast<FloatType>(1 / (2.71828182845904523536L));

        /** A predefined value for 1 / sqrt(2) */
        static constexpr FloatType reciprocalSqrt2 = static_cast<FloatType> (0.70710678118654752440L);
    };

    // gets the previous power of 2 to compliment juce::nextPowerOfTwo
    inline int lastPowerOfTwo(int n) noexcept
    {
        n |= (n >> 1);
        n |= (n >> 2);
        n |= (n >> 4);
        n |= (n >> 8);
        n |= (n >> 16);
        return n - (n >> 1);
    }

    /* Integer modulo for negative values */
    static int reverseModulo(int x, int y)
    {
        jassert(x < 0); // why use it if not negative?

        auto z = y - ((0 - x) % y);
        if (z == y) z = 0;
        return z;
    }

}   // namespace gedd