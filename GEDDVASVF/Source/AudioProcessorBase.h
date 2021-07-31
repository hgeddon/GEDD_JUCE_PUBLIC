/*
  ==============================================================================

    AudioProcessorBase.h
    Created: 31 Jul 2021 4:25:44pm
    Author:  GEDD

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

namespace gedd {

class AudioProcessorBase : public juce::AudioProcessor
{
public:
    //==============================================================================
    using ParameterLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
    using ParameterGroup = juce::AudioProcessorParameterGroup;
    using ParameterFloat = juce::AudioProcessorValueTreeState::Parameter;
    using ParameterBool = juce::AudioParameterBool;
    using ParameterChoice = juce::AudioParameterChoice;
    using RangeFloat = juce::NormalisableRange<float>;

    //==============================================================================
    AudioProcessorBase() = default;
    explicit AudioProcessorBase(const BusesProperties& ioLayouts, ParameterLayout pl, juce::UndoManager* um = nullptr);
    ~AudioProcessorBase() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void reset() override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    static BusesProperties getDefaultProperties();
    static BusesProperties getDefaultPropertiesWithSidechain();

    juce::AudioProcessorValueTreeState& getApvts() noexcept { return apvts; }

    /*
        Have to make a 'createLayout()' function to pass to the apvts through the constructor
    */
    virtual ParameterLayout createLayout() = 0;

protected:
    //==============================================================================
    juce::AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioProcessorBase)
};

} // namespace gedd
