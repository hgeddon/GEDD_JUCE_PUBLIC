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
    
}   // namespace gedd