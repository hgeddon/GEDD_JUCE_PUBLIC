/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GeddvasvfAudioProcessor::GeddvasvfAudioProcessor()
    : AudioProcessorBase(getDefaultProperties(), createLayout()),
    paramRef(apvts),
    svfProcessorUpdater(paramRef.eqParamRef, svfProcessor)
{
}

GeddvasvfAudioProcessor::~GeddvasvfAudioProcessor()
{
}

//==============================================================================
void GeddvasvfAudioProcessor::reset()
{
    svfProcessor.reset();
}

//==============================================================================
void GeddvasvfAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    const auto channels = juce::jmax(getTotalNumInputChannels(), getTotalNumOutputChannels());
    if (channels == 0) return;

    juce::dsp::ProcessSpec spec{ sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(channels) };

    // prepare processors here
    svfProcessor.prepare(spec);

    reset();
}

void GeddvasvfAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void GeddvasvfAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels  = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();
    const auto numChannels = juce::jmax(totalNumInputChannels, totalNumOutputChannels);

    // update processor state
    svfProcessorUpdater.updateProcessor();

    // clear extra channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // make context
    auto inOutBlock = juce::dsp::AudioBlock<float>(buffer).getSubsetChannelBlock(0, numChannels);
    auto context = juce::dsp::ProcessContextReplacing<float>(inOutBlock);

    // process
    svfProcessor.process(context);
}

//==============================================================================

juce::AudioProcessorEditor* GeddvasvfAudioProcessor::createEditor()
{
    //return new juce::GenericAudioProcessorEditor (*this);
    return new VasvftestAudioProcessorEditor(*this);
}

juce::AudioProcessorValueTreeState::ParameterLayout GeddvasvfAudioProcessor::createLayout()
{
    ParameterLayout layout;

    {
        auto params = EQParameterReference::createParamGroup(ID::group::EQ);
        layout.add(params.begin(), params.end());
    }

    return layout;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GeddvasvfAudioProcessor();
}
