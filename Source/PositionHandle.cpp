
#include "PositionHandle.h"
#include "AudioFormEvents.h"

PositionHandle::PositionHandle(AudioformEvents* par, bool left) : left(left)
{
    this->par = par;
    setMouseCursor(juce::MouseCursor::LeftRightResizeCursor);
    constrains.setMinimumOnscreenAmounts(1000, 1000, 1000, 1000);
}
void PositionHandle::mouseDown(const juce::MouseEvent& e)
{
    dragger.startDraggingComponent(this, e);
}
void PositionHandle::mouseDrag(const juce::MouseEvent& e)
{

    dragger.dragComponent(this, e, &constrains);
    if (left)
    {
        const float x = e.originalComponent->getX();
        par->changeStart(x);
    }
    else {
        const float x = e.originalComponent->getX();
        par->changeEnd(x);
    }

    }
void PositionHandle::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0, 0, 255));
}
