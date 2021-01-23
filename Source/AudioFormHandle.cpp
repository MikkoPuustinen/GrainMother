
#include "AudioFormHandle.h"
#include "AudioFormEvents.h"

AudioformHandle::AudioformHandle(AudioformEvents* par) : interval(50), intervalHandle(), draggingComponent(false), cursor(), state(STATE_NONE)
{
    this->par = par;
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}
void AudioformHandle::mouseDown(const juce::MouseEvent& e)
{
    draggingComponent = !intervalHandle.contains(e.getPosition().getX(), e.getPosition().getY());
    if (intervalHandle.contains(e.getPosition().getX(), e.getPosition().getY()))
    {
        state = STATE_INTERVAL;
    }
    else if (e.getPosition().getX() > par->getReadpos() * getLocalBounds().getWidth() + 4 && e.getPosition().getX() < (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() - 4)
    {
        state = STATE_MOVE;
        interval = e.getPosition().getX() - par->getReadpos() * getLocalBounds().getWidth();
    }
    else if (e.getPosition().getX() < par->getReadpos() * getLocalBounds().getWidth() + 4 &&
             e.getPosition().getX() > par->getReadpos() * getLocalBounds().getWidth() - 4)
    {
        state = STATE_LEFT;
    }
    else if (e.getPosition().getX() < (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() + 4 &&
             e.getPosition().getX() > (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() - 4)
    {
        state = STATE_RIGHT;
    }
    else {
        state = STATE_NONE;
    }
}
void AudioformHandle::mouseDrag(const juce::MouseEvent& e)
{
    switch (state) 
    {
    case STATE_NONE:
    {
        break;
    }
    case STATE_MOVE:
    {
        auto ev = e.getEventRelativeTo(par);
        auto temp = ev.getPosition().getX() - e.getPosition().getX();
        par->changeReadpos(ev.getPosition().getX() - interval);
        break;
    }
    case STATE_INTERVAL:
    {
        auto ev = e.getEventRelativeTo(this);
        par->changeInterval(ev.getPosition().getY());
        break;
    }
    case STATE_LEFT:
    {
        par->changeStart(e.getPosition().getX());
        break;
    }
    case STATE_RIGHT:
    {
        par->changeEnd(e.getPosition().getX());
        break;
    }

    }
}
void AudioformHandle::mouseMove(const juce::MouseEvent& e)
{
    if (intervalHandle.contains(e.getPosition().getX(), e.getPosition().getY()))
    {
        cursor = juce::MouseCursor::UpDownResizeCursor;
    }
    else if (e.getPosition().getX() > par->getReadpos() * getLocalBounds().getWidth() + 4 && e.getPosition().getX() < (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() - 4)
    {
        cursor = juce::MouseCursor::PointingHandCursor;
    }
    else if (e.getPosition().getX() < par->getReadpos()  * getLocalBounds().getWidth() + 4 &&
             e.getPosition().getX() > par->getReadpos()  * getLocalBounds().getWidth() - 4 ||
             e.getPosition().getX() < (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() + 4 &&
             e.getPosition().getX() > (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth() - 4
            )
    {
        cursor = juce::MouseCursor::LeftRightResizeCursor;
    }
    else {
        cursor = juce::MouseCursor();
    }
}
juce::MouseCursor AudioformHandle::getMouseCursor()
{
    return cursor;
}
void AudioformHandle::resized()
{
    intervalHandle.setBounds(0, getLocalBounds().getHeight() - par->getInterval() - 4, getLocalBounds().getWidth(), 8);
}
void AudioformHandle::paint(juce::Graphics& g)
{
    float start = par->getReadpos() * getLocalBounds().getWidth();
    float end = (par->getReadpos() + par->getDuration()) * getLocalBounds().getWidth();
    float duration = end - start;
    if (start >= 0 && start != end) {
        g.setColour(juce::Colour(255, 105, juce::uint8(128), juce::uint8(128)));
        g.fillRect((int)start, getLocalBounds().getHeight() - (int)par->getInterval() + 2, (int)duration, getLocalBounds().getHeight());
        g.setColour(juce::Colour(51, 51, juce::uint8(51), juce::uint8(128)));
        g.fillRect((int)start, getLocalBounds().getHeight() - (int)par->getInterval() - 2, (int)duration, 4);
        g.setColour(juce::Colour(51, 51, 51));
        juce::Point<float> handleP(end - duration * 0.5f, getLocalBounds().getHeight() - par->getInterval());
        juce::Rectangle<float> handle(0, 0, 50, 10);
        g.fillRoundedRectangle(handle.withCentre(handleP), 3.0f);
        g.setColour(juce::Colour(51, 51, juce::uint8(51)));
        g.fillRoundedRectangle(start, 0, 4, getLocalBounds().getHeight(), 1.5f);
        g.fillRoundedRectangle(end - 4, 0, 4, getLocalBounds().getHeight(), 1.5f);
    }
}

void AudioformHandle::setInterval()
{
    intervalHandle.setBounds(par->getReadpos() * getLocalBounds().getWidth(), getLocalBounds().getHeight() - par->getInterval() - 5, par->getReadpos() + par->getDuration() * getLocalBounds().getWidth(), 10);
}
