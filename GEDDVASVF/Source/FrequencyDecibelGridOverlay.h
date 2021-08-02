/*
  ==============================================================================

    FrequencyDecibelGridOverlay.h
    Created: 31 Jul 2021 4:19:06am
    Author:  GEDD

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CommonFunctions.h"

//==============================================================================
/*
* Bugs: - db lines should always centre around 0
*/
class FrequencyDecibelGridOverlay : public juce::Component
{
public:
    // creates and stores an array of decade 'markers' - depending on range and component size
    class FrequencyMarkers
    {
    public:
        // Constructors
        FrequencyMarkers() {}

        explicit FrequencyMarkers(juce::NormalisableRange<double> freqRange)
        {
            makeDecades(freqRange.getRange());
        }

        explicit FrequencyMarkers(juce::Range<double> freqRange)
        {
            makeDecades(freqRange);
        }

        FrequencyMarkers(const FrequencyMarkers&) = default;    // copy constructor
        FrequencyMarkers(FrequencyMarkers&&) = default;         // move constructor
        FrequencyMarkers& operator=(const FrequencyMarkers&) = default; // copy assignment operator
        FrequencyMarkers& operator=(FrequencyMarkers&&) = default;  // move assignment operator

        // update state
        void makeDecades(juce::NormalisableRange<double> freqRange)
        {
            makeDecades(freqRange.getRange());
        }

        void makeDecades(juce::Range<double> freqRange)
        {
            makeDecades(freqRange.getStart(), freqRange.getEnd());
        }

        void makeDecades(double freqMin, double freqMax)
        {
            jassert(freqMin < freqMax);

            markers.clear();

            auto freq = roundUpToNearestDecade(freqMin);

            do
            {
                markers.push_back(freq);
                freq += getIncrement(freq);
            } while (freq < freqMax);
        }

        // Storage
        std::vector<int> markers;

    private:
        // Returns whith decade 'x' is in ie. 1 = 1, 10 = 2, 100 = 3 etc..
        int getDecade(const float x) { return std::floor(std::log10(x)); }

        // Returns increment from 'x' ie. 0 to 99 = 10, 100 to 999 = 100, 1000 to 9999 = 1000
        int getIncrement(const int x) { return std::pow(10, getDecade(x)); }

        // Round up to nearest decade ie. 22 = 30, 48 = 50, 112 = 120
        int roundUpToNearestDecade(const float x)
        {
            const auto inc = getIncrement(x);
            return std::ceil(x / inc) * inc;
        }

        JUCE_LEAK_DETECTOR(FrequencyMarkers)
    };

    enum ColourIDs
    {
        gridColourID = 0x8800000,
        textColourID = 0x8800001
    };

    FrequencyDecibelGridOverlay();

    FrequencyDecibelGridOverlay(juce::NormalisableRange<double> freqRange, juce::NormalisableRange<double> dbRange);

    // setters
    void setFrequencyRange(double start, double end);

    void setFrequencyRange(juce::Range<double> r);

    void setFrequencyNormalisableRange(juce::NormalisableRange<double> r);

    void setDecibelRange(double bottom, double top);

    void setDecibelRange(juce::Range<double> r);

    void setDecibelNormalisableRange(juce::NormalisableRange<double> r);

    void setFont(juce::Font f) { textFont = f; }

    void setGridThicknessPx(int t);

    // getters
    juce::Font getFont() const { return textFont; }

    int getGridThicknessPx() const { return gridThicknessPx; }

    juce::NormalisableRange<double> getFrequencyNormalisableRange() const { return frequencyRange; }

    juce::NormalisableRange<double> getDecibelNormalisableRange() const { return decibelRange; }

    juce::Range<double> getFrequencyRange() const { return frequencyRange.getRange(); }

    juce::Range<double> getDecibelRange() const { return decibelRange.getRange(); }

    void paint(juce::Graphics& g) override;

    void resized() override;

private:
    void paintFrequencyGrid(juce::Graphics& g);

    void drawFrequencyLabels(juce::Graphics& g, juce::StringRef str, const double y, const double textBase);

    void drawDecibelLine(juce::Graphics& g, const int db, const int textStartX);

    void paintDecibelGrid(juce::Graphics& g);

    static constexpr auto decadeStrings = { "10", "100", "1k", "10k" };

    FrequencyMarkers frequencyMarkers;

    juce::Font textFont;
    int gridThicknessPx{ 1 };
    int paddingPx{ 4 };

    juce::NormalisableRange<double> frequencyRange;
    juce::NormalisableRange<double> decibelRange;

    //========================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FrequencyDecibelGridOverlay)
};

