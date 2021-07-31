/*
  ==============================================================================

    AudioProcessorBase.cpp
    Created: 31 Jul 2021 4:25:44pm
    Author:  GEDD

  ==============================================================================
*/

#include "AudioProcessorBase.h"

//==============================================================================
gedd::AudioProcessorBase::AudioProcessorBase(const BusesProperties& ioLayouts, ParameterLayout pl, juce::UndoManager* um)
	: AudioProcessor(ioLayouts),
    apvts(*this, um, "state", std::move(pl))
{
}

gedd::AudioProcessorBase::~AudioProcessorBase()
{
}

//==============================================================================
const juce::String gedd::AudioProcessorBase::getName() const
{
    return JucePlugin_Name;
}

bool gedd::AudioProcessorBase::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool gedd::AudioProcessorBase::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool gedd::AudioProcessorBase::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double gedd::AudioProcessorBase::getTailLengthSeconds() const
{
    return 0.0;
}

int gedd::AudioProcessorBase::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int gedd::AudioProcessorBase::getCurrentProgram()
{
    return 0;
}

void gedd::AudioProcessorBase::setCurrentProgram(int index)
{
}

const juce::String gedd::AudioProcessorBase::getProgramName(int index)
{
    return {};
}

void gedd::AudioProcessorBase::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void gedd::AudioProcessorBase::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    copyXmlToBinary(*apvts.copyState().createXml(), destData);
}

void gedd::AudioProcessorBase::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    apvts.replaceState(juce::ValueTree::fromXml(*getXmlFromBinary(data, sizeInBytes)));
}

//==============================================================================
void gedd::AudioProcessorBase::reset()
{
}

void gedd::AudioProcessorBase::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void gedd::AudioProcessorBase::releaseResources()
{
}

juce::AudioProcessor::BusesProperties gedd::AudioProcessorBase::getDefaultProperties()
{
    return BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
        //.withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
        ;
}

juce::AudioProcessor::BusesProperties gedd::AudioProcessorBase::getDefaultPropertiesWithSidechain()
{
    return BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
        .withInput("Sidechain", juce::AudioChannelSet::stereo(), true)
        ;
}

bool gedd::AudioProcessorBase::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}

void gedd::AudioProcessorBase::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(buffer, midiMessages);
}

//==============================================================================
bool gedd::AudioProcessorBase::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* gedd::AudioProcessorBase::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}

