
#include "AudioFormHandle.h"
#include "AudioFormEvents.h"

AudioformHandle::AudioformHandle(AudioformEvents* par) : interval(50)
{
    this->par = par;
    constrains.setMinimumOnscreenAmounts(1000, 1000, 1000, 1000);
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}
void AudioformHandle::mouseDown(const juce::MouseEvent& e)
{
    dragger.startDraggingComponent(this, e);
}
void AudioformHandle::mouseDrag(const juce::MouseEvent& e)
{
    dragger.dragComponent(this, e, &constrains);
    const float x = e.originalComponent->getX();
    const float w = this->getParentComponent()->getWidth();
    auto r = x / w;
    //valueTreeState.getParameter("readpos")->setValueNotifyingHost(x / w);
}
void AudioformHandle::paint(juce::Graphics& g)
{
    float start = 0;
    float end = getLocalBounds().getWidth();
    float duration = end - start;
    if (start >= 0 && start != end) {
        g.setColour(juce::Colour(255, 105, juce::uint8(128), juce::uint8(128)));
        g.fillRect((int)start, getLocalBounds().getHeight() - (int)interval + 2, (int)duration, getLocalBounds().getHeight());
        g.setColour(juce::Colour(51, 51, juce::uint8(51), juce::uint8(128)));
        g.fillRect((int)start, getLocalBounds().getHeight() - (int)interval - 2, (int)duration, 4);
        g.setColour(juce::Colour(51, 51, 51));
        juce::Point<float> handleP(end - duration * 0.5f, getLocalBounds().getHeight() - interval);
        juce::Rectangle<float> handle(0, 0, 50, 10);
        g.fillRoundedRectangle(handle.withCentre(handleP), 3.0f);
        g.setColour(juce::Colour(51, 51, juce::uint8(51)));
        g.fillRoundedRectangle(0 - 2, 0, 4, getLocalBounds().getHeight(), 1.5f);
        g.fillRoundedRectangle(getLocalBounds().getWidth() - 2, 0, 4, getLocalBounds().getHeight(), 1.5f);
    }
}
void AudioformHandle::setInterval(const float& interval)
{
    this->interval = interval;
}