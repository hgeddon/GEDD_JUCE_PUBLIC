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
    
    // Create a NormalisableRange of template type with frequency scaling
    template <typename Type = float>
    static juce::NormalisableRange<Type> createFrequencyRange(Type min, Type max)
    {
        return juce::NormalisableRange<Type>(min, max,
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
    }

    // Basic float to text / text to float conversion methods
    static juce::String floatValueToTextFunction(float x) { return juce::String(x, 2); }
    static float floatTextToValueFunction(const juce::String& str) { return str.getFloatValue(); }

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

}   // namespace gedd