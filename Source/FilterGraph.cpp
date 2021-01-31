#include "FilterGraph.h"

FilterGraph::FilterGraph(GrainMotherAudioProcessor& p, juce::AudioProcessorValueTreeState& vts) : audioProcessor(p), valueTreeState(vts), freq(0), resonance(0)
{
	startTimer(50);
}

void FilterGraph::paint(juce::Graphics& g)
{
	//g.fillAll(juce::Colour(0, 255, 0));
	g.setColour(juce::Colour(51, 51, juce::uint8(51), juce::uint8(128)));
	g.fillRect(getWidth() / 2,  0, 1, getHeight());
	g.fillRect(0, getHeight() / 2, getLocalBounds().getWidth(), 1);
	g.fillEllipse(freq * getWidth(), getHeight() * 0.5f - (resonance * getHeight() * 0.5f), 8, 8);

	g.strokePath(frequencyResponse, juce::PathStrokeType(2.0f));

	juce::Path p1, p2;
	p1.addPath(frequencyResponse);
	auto endX = p1.getCurrentPosition().getX();
	p1.lineTo(p1.getCurrentPosition().getX(), getBottom());
	p1.lineTo(0, getBottom());
	p1.closeSubPath();
	juce::FillType fillType = juce::FillType();
	fillType.setColour(juce::Colour(51, 51, 51));
	g.setFillType(fillType);
	//g.fillPath(uiPath);
	g.setColour(juce::Colour(51, 51, 51));
	g.strokePath(p1, juce::PathStrokeType(2));

	fillType.setColour(juce::Colour(255, 105, juce::uint8(128), juce::uint8(128)));
	g.setFillType(fillType);
	g.fillPath(p1);

}

void FilterGraph::mouseDown(const juce::MouseEvent& e)
{

}

void FilterGraph::mouseDrag(const juce::MouseEvent& e)
{
	float resonance, freq;
	freq = static_cast<float>(e.getPosition().getX()) / getLocalBounds().getWidth();
	auto h = (float)getHeight() * 0.5f;
	auto p = (float)e.getPosition().getY();
	auto h2 = (float)getHeight() * 0.5f;
	resonance = (h - p) / h2;
	valueTreeState.getParameter("filterFreq")->setValueNotifyingHost(freq);
	valueTreeState.getParameter("resonance")->setValueNotifyingHost(resonance);
}

void FilterGraph::timerCallback()
{
	this->freq = valueTreeState.getParameter("filterFreq")->getValue();
	this->resonance = valueTreeState.getParameter("resonance")->getValue();
	auto pixels = 2.0f * getLocalBounds().getHeight() / juce::Decibels::decibelsToGain(24.0f);
	frequencyResponse.clear();
	//audioProcessor.updateFilterGraph();
	audioProcessor.updateFilterPath(frequencyResponse, getLocalBounds(), pixels);
	repaint();
}
