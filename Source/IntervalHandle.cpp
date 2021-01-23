#include "IntervalHandle.h"
#include "AudioFormEvents.h"

IntervalHandle::IntervalHandle(AudioformEvents* par)
{
	this->par = par;
	setMouseCursor(juce::MouseCursor::UpDownResizeCursor);
	constrains.setMinimumOnscreenAmounts(1000, 1000, 1000, 1000);
}

void IntervalHandle::mouseDown(const juce::MouseEvent& e)
{
	//dragger.startDraggingComponent(this, e);
	par->startDragging();
}

void IntervalHandle::mouseDrag(const juce::MouseEvent& e)
{
	//dragger.dragComponent(this, e, &constrains);
	//const float y = e.originalComponent->getY();
	//DBG(y);
	//par->changeInterval(y);
}

void IntervalHandle::mouseUp(const juce::MouseEvent& e)
{
	par->stopDragging();
}

void IntervalHandle::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colour(0, 0, 255));
}
