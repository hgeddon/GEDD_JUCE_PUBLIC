/*
  ==============================================================================

    ParameterReference.h
    Created: 8 Jul 2021 7:54:33pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CommonFunctions.h"
#include "VASVF.h"

//=================================================================
namespace ID
{
    namespace group
    {
        static constexpr auto EQ = "EQ";;
    }
}

//=================================================================
namespace ID
{
    namespace EQ
    {
        static constexpr auto type  = "type";
        static constexpr auto freq  = "freq";
        static constexpr auto gain  = "gain";
        static constexpr auto q     = "q";
        static constexpr auto autoq = "autoq";
    }
}

//=================================================================
using Apvts = juce::AudioProcessorValueTreeState;
using ParameterGroup = juce::AudioProcessorParameterGroup;
using ParameterFloat = juce::AudioProcessorValueTreeState::Parameter;
using ParameterChoice = juce::AudioParameterChoice;
using ParameterBool = juce::AudioParameterBool;

struct EQParameterReference
{
    explicit EQParameterReference(Apvts& apvts, juce::StringRef name)
        :
        typeID  (name + ID::EQ::type),
        freqID  (name + ID::EQ::freq),
        gainID  (name + ID::EQ::gain),
        qID     (name + ID::EQ::q),
        autoqID (name + ID::EQ::autoq),
        type    (static_cast<ParameterChoice&>(*apvts.getParameter(typeID))),
        freq    (static_cast<ParameterFloat&> (*apvts.getParameter(freqID))),
        gain    (static_cast<ParameterFloat&> (*apvts.getParameter(gainID))),
        q       (static_cast<ParameterFloat&> (*apvts.getParameter(qID))),
        autoq   (static_cast<ParameterBool&>(*apvts.getParameter(autoqID)))
    {}

    // id
    juce::Identifier autoqID;
    juce::Identifier typeID;
    juce::Identifier freqID;
    juce::Identifier gainID;
    juce::Identifier qID;

    // params
    ParameterBool& autoq;
    ParameterChoice& type;
    ParameterFloat& freq;
    ParameterFloat& gain;
    ParameterFloat& q;

    // parameter group builder
    static std::vector<std::unique_ptr<ParameterGroup>> createParamGroup(juce::StringRef name)
    {
        auto params = std::vector<std::unique_ptr<ParameterGroup>>();

        auto type = std::make_unique<ParameterChoice>(
            name + ID::EQ::type,
            ID::EQ::type,
            gedd::dsp::VASVF::filterTypeStr,
            0);

        auto freq = std::make_unique<ParameterFloat>(
            name + ID::EQ::freq,
            ID::EQ::freq,
            "hz",
            gedd::createFrequencyRange(20.0f, 22000.0f),
            1000.0f,
            gedd::floatValueToTextFunction,
            gedd::floatTextToValueFunction);

        auto gain = std::make_unique<ParameterFloat>(
            name + ID::EQ::gain,
            ID::EQ::gain,
            "dB",
            juce::NormalisableRange<float>(-36.0f, 36.0f, 0.0f, 0.666f, true),
            0.0f,
            gedd::floatValueToTextFunction,
            gedd::floatTextToValueFunction);

        auto q = std::make_unique<ParameterFloat>(
            name + ID::EQ::q,
            ID::EQ::q,
            "",
            juce::NormalisableRange<float>(0.1f, 8.0f, 0.0f, 0.333f),
            0.707f,
            gedd::floatValueToTextFunction,
            gedd::floatTextToValueFunction);

        auto autoq = std::make_unique<ParameterBool>(
            name + ID::EQ::autoq,
            ID::EQ::autoq,
            false);

        params.push_back(std::make_unique<ParameterGroup>(
            name,
            name,
            "|",
            std::move(type),
            std::move(freq),
            std::move(gain),
            std::move(q),
            std::move(autoq)
            ));

        return params;
    }
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EQParameterReference)
};

struct ParameterReferences
{
    explicit ParameterReferences(Apvts& apvts)
        : eqParamRef(apvts, ID::group::EQ)
    {}

    EQParameterReference eqParamRef;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ParameterReferences)
};
