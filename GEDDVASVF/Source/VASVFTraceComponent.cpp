/*
  ==============================================================================

    VASVFTraceComponent.cpp
    Created: 31 Jul 2021 4:18:47am
    Author:  GEDD

  ==============================================================================
*/

#include "VASVFTraceComponent.h"

//==============================================================================
VASVFTraceComponent::VASVFTraceComponent(EQParameterReference& ref)
    : freq(ref.freq),
    gain(ref.gain),
    q(ref.q),
    autoq(ref.autoq),
    filterType(ref.type)
{
    setOpaque(false);
    setBufferedToImage(true);
    setInterceptsMouseClicks(false, false);
    setPaintingIsUnclipped(true);
    setRepaintsOnMouseActivity(false);

    setColour(magnitudeTraceColourID, juce::Colours::azure);
    setColour(phaseTraceColourID, juce::Colours::magenta);

    freq.addListener(this);
    gain.addListener(this);
    q.addListener(this);
    autoq.addListener(this);
    filterType.addListener(this);

    frequencies.resize(numPoints);
    magnitudes.resize(numPoints);
    phases.resize(numPoints);

    fillFrequencyVector();

    update();
}

VASVFTraceComponent::~VASVFTraceComponent()
{
    freq.removeListener(this);
    gain.removeListener(this);
    q.removeListener(this);
    autoq.removeListener(this);
    filterType.removeListener(this);
}

void VASVFTraceComponent::paint (juce::Graphics& g)
{
    if (showMagnitudes && !frequencyPath.isEmpty())
    {
        g.setColour(findColour(magnitudeTraceColourID));
        g.strokePath(frequencyPath, juce::PathStrokeType(1.5f));
    }

    if (showPhases && !phasePath.isEmpty())
    {
        g.setColour(findColour(phaseTraceColourID));
        g.strokePath(phasePath, juce::PathStrokeType(1.5f));
    }
}

void VASVFTraceComponent::resized()
{
    update();
}

void VASVFTraceComponent::update()
{
    using FilterType = gedd::dsp::VASVF::FilterType;
    using State = gedd::dsp::VASVF::State<double>;

    // create state from params
    const auto sr = sampleRate;
    const auto sf = freq.getNormalisableRange().convertFrom0to1(freq.getValue());
    const auto sg = gain.getNormalisableRange().convertFrom0to1(gain.getValue());
    const auto sq = q.getNormalisableRange().convertFrom0to1(q.getValue());
    const auto aq = autoq.getNormalisableRange().convertFrom0to1(autoq.getValue());
    const auto t = static_cast<FilterType>(filterType.getNormalisableRange().convertFrom0to1(filterType.getValue()));

    jassert(sr > 0);    // sample rate in range
    jassert(sf > 0);    // frequency in range
    jassert(sq > 0);    // q in range

    State::Ptr newState;

    switch (t)
    {
    case FilterType::none:      newState = new State(); break;
    case FilterType::lowpass:   newState = State::makeLowpass(sr, sf, sg, sq, aq);   break;
    case FilterType::bandpass:  newState = State::makeBandpass(sr, sf, sg, sq, aq);  break;
    case FilterType::highpass:  newState = State::makeHighpass(sr, sf, sg, sq, aq);  break;
    case FilterType::notch:     newState = State::makeNotch(sr, sf, sg, sq, aq);     break;
    case FilterType::allpass:   newState = State::makeAllpass(sr, sf, sg, sq, aq);   break;
    case FilterType::bell:      newState = State::makeBell(sr, sf, sg, sq, aq);      break;
    case FilterType::lowshelf:  newState = State::makeLowshelf(sr, sf, sg, sq, aq);  break;
    case FilterType::highshelf: newState = State::makeHighshelf(sr, sf, sg, sq, aq); break;
    }

    // if successful then update trace
    if (newState)
    {
        displayState = *newState;
        redraw();
    }
}

void VASVFTraceComponent::setNumPoints(int newNumPoints)
{
    if (newNumPoints != numPoints)
    {
        numPoints = newNumPoints;

        fillFrequencyVector();

        update();
    }
}

void VASVFTraceComponent::setFrequencyRange(double start, double end)
{
    setFrequencyRange(juce::Range<double>(start, end));
}

void VASVFTraceComponent::setFrequencyRange(juce::Range<double> r)
{
    jassert(r.getStart() < r.getEnd());

    if (r != frequencyRange.getRange())
    {
        frequencyRange.start = r.getStart();
        frequencyRange.end = r.getEnd();

        fillFrequencyVector();
        update();
    }
}

void VASVFTraceComponent::setDecibelRange(double bottom, double top)
{
    setDecibelRange(juce::Range<double>(bottom, top));
}

void VASVFTraceComponent::setDecibelRange(juce::Range<double> r)
{
    jassert(r.getStart() < r.getEnd());

    if (r != decibelRange.getRange())
    {
        decibelRange.start = r.getStart();
        decibelRange.end = r.getEnd();

        update();
    }
}

void VASVFTraceComponent::setSampleRate(double newSampleRate)
{
    jassert(newSampleRate != 0.0);

    if (newSampleRate != sampleRate)
    {
        sampleRate = newSampleRate;

        update();
    }
}

void VASVFTraceComponent::redraw() noexcept
{
    updateMagnitudes();
    updatePhases();
    createMagnitudePlot();
    createPhasePlot();
    repaint();
}

void VASVFTraceComponent::handleUpdate() noexcept
{
    if (juce::MessageManager::getInstance()->isThisTheMessageThread())
    {
        cancelPendingUpdate();
        handleAsyncUpdate();
    }
    else
    {
        triggerAsyncUpdate();
    }
}

void VASVFTraceComponent::parameterValueChanged(int parameterIndex, float newValue)
{
    handleUpdate();
}

void VASVFTraceComponent::handleAsyncUpdate()
{
    update();
}

void VASVFTraceComponent::fillFrequencyVector()
{
    jassert(frequencies.size() == numPoints);

    auto resolution = 1.0 / static_cast<double>(numPoints - 1);

    for (auto i = 0; i != numPoints; ++i)
        frequencies[i] = frequencyRange.convertFrom0to1(static_cast<double>(i) * resolution);
}

void VASVFTraceComponent::updateMagnitudes()
{
    displayState.getMagnitudeForFrequencyArray(frequencies.data(), magnitudes.data(), frequencies.size(), sampleRate);
}

void VASVFTraceComponent::updatePhases()
{
    displayState.getPhaseForFrequencyArray(frequencies.data(), phases.data(), frequencies.size(), sampleRate);
}

void VASVFTraceComponent::createMagnitudePlot()
{
    const auto bounds = getLocalBounds();
    const auto pixelsPerValue = 4.0 * bounds.getHeight() / juce::Decibels::decibelsToGain(decibelRange.end);
    const auto xScale = static_cast<double>(bounds.getWidth()) / static_cast<double>(numPoints - 1);
    const auto gainFloor = static_cast<double>(juce::Decibels::decibelsToGain(decibelRange.start));
    const auto gainCeiling = static_cast<double>(juce::Decibels::decibelsToGain(decibelRange.end));

    frequencyPath.clear();
    frequencyPath.preallocateSpace(3 * numPoints);

    for (auto i = 0; i != numPoints; ++i)
    {
        const auto xPos = static_cast<double>(bounds.getX() + i) * xScale;
        const auto db = decibelRange.convertTo0to1(decibelRange.snapToLegalValue(juce::Decibels::gainToDecibels(magnitudes[i])));
        const auto yPos = bounds.getHeight() - ((db * bounds.getHeight()) + bounds.getY());

        // plot
        if (i == 0)
            frequencyPath.startNewSubPath(xPos, yPos);
        else
            frequencyPath.lineTo(xPos, yPos);
    }
}

void VASVFTraceComponent::createPhasePlot()  // rather than db shouldn't it be in phases?
{
    auto bounds = getLocalBounds();
    const auto pixelsPerValue = 4.0 * bounds.getHeight() / juce::Decibels::decibelsToGain(decibelRange.end);
    const auto xScale = static_cast<double>(bounds.getWidth()) / static_cast<double>(numPoints - 1);
    const auto phaseFloor = -juce::MathConstants<double>::pi;
    const auto phaseCeiling = juce::MathConstants<double>::pi;

    phasePath.clear();
    phasePath.preallocateSpace(3 * numPoints);

    for (auto i = 0; i != numPoints; ++i)
    {
        auto xPos = static_cast<double>(bounds.getX() + i) * xScale;
        auto yPos = 0.0;

        // clamp yPos
        if (phases[i] < phaseFloor)
            yPos = bounds.getBottom();
        else if (phases[i] > phaseCeiling)
            yPos = bounds.getY();
        else
            yPos = static_cast<double>(bounds.getCentreY()) - pixelsPerValue * (phases[i] / juce::MathConstants<double>::halfPi);

        // plot
        if (i == 0)
            phasePath.startNewSubPath(xPos, yPos);
        else
            phasePath.lineTo(xPos, yPos);
    }
}
