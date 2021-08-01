/*
  ==============================================================================

    FrequencyDecibelGridOverlay.cpp
    Created: 31 Jul 2021 4:19:06am
    Author:  GEDD

  ==============================================================================
*/

#include "FrequencyDecibelGridOverlay.h"

//==============================================================================
FrequencyDecibelGridOverlay::FrequencyDecibelGridOverlay()
    : frequencyRange(gedd::createFrequencyRange(20.0, 18000.0)),
    decibelRange(-24.0, 24.0),
    textFont(juce::Font(juce::String("Microsoft YaHei UI"), 12.0f, juce::Font::bold))
{
    setOpaque(false);   // needs to have transparent background
    setBufferedToImage(true);
    setInterceptsMouseClicks(false, false);
    setPaintingIsUnclipped(true);
    setRepaintsOnMouseActivity(false);

    setColour(gridColourID, juce::Colours::darkslategrey);
    setColour(textColourID, juce::Colours::slategrey);

    frequencyMarkers.makeDecades(frequencyRange);
}

FrequencyDecibelGridOverlay::FrequencyDecibelGridOverlay(juce::NormalisableRange<double> freqRange, juce::NormalisableRange<double> dbRange)
    : frequencyRange(freqRange),
    decibelRange(dbRange),
    textFont(juce::Font(juce::String("Microsoft YaHei UI"), 12.0f, juce::Font::bold))
{
    setOpaque(false);   // needs to have transparent background
    setBufferedToImage(true);
    setInterceptsMouseClicks(false, false);
    setPaintingIsUnclipped(true);
    setRepaintsOnMouseActivity(false);

    setColour(gridColourID, juce::Colours::darkslategrey);
    setColour(textColourID, juce::Colours::slategrey);

    frequencyMarkers.makeDecades(frequencyRange);
}

void FrequencyDecibelGridOverlay::setFrequencyRange(double start, double end)
{
    setFrequencyRange(juce::Range<double>(start, end));
}

void FrequencyDecibelGridOverlay::setFrequencyRange(juce::Range<double> r)
{
    jassert(r.getStart() < r.getEnd());

    if (r != frequencyRange.getRange())
    {
        frequencyRange.start = r.getStart();
        frequencyRange.end = r.getEnd();

        frequencyMarkers.makeDecades(r);
    }
}

void FrequencyDecibelGridOverlay::setFrequencyNormalisableRange(juce::NormalisableRange<double> r)
{
    frequencyRange = r;

    frequencyMarkers.makeDecades(r.getRange());

    repaint();
}

void FrequencyDecibelGridOverlay::setDecibelRange(double bottom, double top)
{
    setDecibelRange(juce::Range<double>(bottom, top));
}

void FrequencyDecibelGridOverlay::setDecibelRange(juce::Range<double> r)
{
    jassert(r.getStart() < r.getEnd());

    if (r != decibelRange.getRange())
    {
        decibelRange.start = r.getStart();
        decibelRange.end = r.getEnd();
    }
}

void FrequencyDecibelGridOverlay::paint(juce::Graphics& g)
{
    paintFrequencyGrid(g);
    paintDecibelGrid(g);
}

void FrequencyDecibelGridOverlay::resized()
{
    repaint();
}

void FrequencyDecibelGridOverlay::paintFrequencyGrid(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();
    const auto textBaseLine = bounds.getBottom() - paddingPx;

    // draw grid
    const auto num = frequencyMarkers.markers.size();

    for (auto i = 0; i != num; ++i)
    {
        const auto marker = frequencyMarkers.markers[i];
        const auto xPos = (frequencyRange.convertTo0to1(marker) * bounds.getWidth()) + bounds.getX();

        // draw line
        g.setColour(findColour(gridColourID));
        g.drawVerticalLine(xPos, bounds.getY(), bounds.getBottom());

        // draw text
        g.setColour(findColour(textColourID));
        g.setFont(textFont);

        switch (marker)
        {
        case 100:   drawFrequencyLabels(g, juce::StringArray(decadeStrings)[0], xPos, textBaseLine); break;
        case 1000:  drawFrequencyLabels(g, juce::StringArray(decadeStrings)[1], xPos, textBaseLine); break;
        case 10000: drawFrequencyLabels(g, juce::StringArray(decadeStrings)[2], xPos, textBaseLine); break;
        }
    }
}

void FrequencyDecibelGridOverlay::drawFrequencyLabels(juce::Graphics& g, juce::StringRef str, const double y, const double textBase)
{
    const auto textWidth = textFont.getStringWidth(str);
    g.drawSingleLineText(str, y - textWidth, textBase);
}

void FrequencyDecibelGridOverlay::drawDecibelLine(juce::Graphics& g, const int db, const int textStartX)
{
    const auto bounds = getLocalBounds();
    const auto str = juce::String(db);
    const auto yPos = bounds.getBottom() - ((decibelRange.convertTo0to1(static_cast<double>(db)) * bounds.getHeight()) + bounds.getX());

    g.setColour(findColour(gridColourID));
    g.drawHorizontalLine(yPos, bounds.getX(), bounds.getRight());

    g.setColour(findColour(textColourID));
    g.drawSingleLineText(str, textStartX, yPos - paddingPx);
}

void FrequencyDecibelGridOverlay::paintDecibelGrid(juce::Graphics& g)
{
    const auto textStartX = getLocalBounds().getX() + paddingPx;
    const auto dbLength = decibelRange.getRange().getLength();
    const auto start = decibelRange.getRange().getStart();
    const auto end = decibelRange.getRange().getEnd();
    const auto dbStep = (dbLength > 24) ? 6 : (dbLength > 12) ? 3 : 1;

    for (auto db = start; db < end; db += dbStep)
        drawDecibelLine(g, db, textStartX);
}
