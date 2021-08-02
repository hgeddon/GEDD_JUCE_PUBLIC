/*
  ==============================================================================

    VASVFTraceComponent.h
    Created: 31 Jul 2021 4:18:47am
    Author:  GEDD

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "VASVFProcessor.h"
#include "ParameterReference.h"
#include "CommonFunctions.h"

//==============================================================================
/*
*/
class VASVFTraceComponent  : public juce::Component,
                             private juce::RangedAudioParameter::Listener,
                             private juce::AsyncUpdater
{
public:
    enum ColourIDs
    {
        magnitudeTraceColourID  = 0x8800100,
        phaseTraceColourID      = 0x8800101
    };

    VASVFTraceComponent(EQParameterReference& ref);

    ~VASVFTraceComponent() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void update();

    // setters
    void setShowMagnitudeTrace(bool shouldShow) 
    { 
        showMagnitudes = shouldShow; 
        repaint();
    }

    void setShowPhaseTrace(bool shouldShow) 
    { 
        showPhases = shouldShow;
        repaint();
    }

    void setNumPoints(int newNumPoints);

    void setFrequencyRange(double start, double end);

    void setFrequencyRange(juce::Range<double> r);

    void setFrequencyNormalisableRange(juce::NormalisableRange<double> r);

    void setDecibelRange(double bottom, double top);

    void setDecibelRange(juce::Range<double> r);

    void setDecibelNormalisableRange(juce::NormalisableRange<double> r);

    void setSampleRate(double newSampleRate);

    // getters
    bool getShowMagnitudeTrace() const { return showMagnitudes; }

    bool getShowPhaseTrace() const { return showPhases; }

    int getNumPoints() const { return numPoints; }

    juce::Range<double> getFrequencyRange() const { return frequencyRange.getRange(); }

    juce::Range<double> getDecibelRange() const { return decibelRange.getRange(); }

    juce::NormalisableRange<double> getFrequencyNormalisableRange() const { return frequencyRange; }

    juce::NormalisableRange<double> getDecibelNormalisableRange() const { return decibelRange; }

private:
    void redraw() noexcept;

    void handleUpdate() noexcept;

    void parameterValueChanged(int parameterIndex, float newValue);

    void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) {}

    void handleAsyncUpdate() override;

    void fillFrequencyVector();

    void updateMagnitudes();

    void updatePhases();

    void createMagnitudePlot();

    void createPhasePlot();

    juce::NormalisableRange<double> frequencyRange{ gedd::createFrequencyRange(20.0, 18000.0) };
    juce::NormalisableRange<double> decibelRange{ -24.0, 24.0 };

    double  sampleRate{ 44100.0 };
    int     numPoints{ 256 };
    bool    showMagnitudes{ true };
    bool    showPhases{ true };

    juce::RangedAudioParameter& freq;
    juce::RangedAudioParameter& gain;
    juce::RangedAudioParameter& q;
    juce::RangedAudioParameter& autoq;
    juce::RangedAudioParameter& filterType;

    gedd::dsp::VASVF::State<double> displayState;

    std::vector<double> frequencies;
    std::vector<double> magnitudes;
    std::vector<double> phases;

    juce::Path frequencyPath;
    juce::Path phasePath;

    //============================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VASVFTraceComponent)
};
