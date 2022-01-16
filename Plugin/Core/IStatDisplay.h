#pragma once

#include <bakkesmod/wrappers/canvaswrapper.h>

/** The public interface of classes which are responsible for displaying stats to the user. */
class IStatDisplay
{
protected:
	IStatDisplay() = default;

public:
	virtual ~IStatDisplay() = default;

	/** This is called each time a single frame shall be rendered. */
	virtual void renderOneFrame(CanvasWrapper& canvas) const = 0;
};