/*
 * MouseMoveEvent.h
 *
 *  Created on: Apr 27, 2026
 *      Author: hor
 */

#ifndef LIB_GUI_EVENTS_MOUSEMOVEEVENT_H_
#define LIB_GUI_EVENTS_MOUSEMOVEEVENT_H_

#include "Controls/PosPixel.h"
#include <SDL3/SDL_events.h>
#include <cmath>

namespace OevControls {

class MouseMoveEvent {
public:

	MouseMoveEvent (SDL_MouseMotionEvent const &mouseMoveEvent) :
		mouseMoveEvent {mouseMoveEvent},
		mousePosition {.xPixel = static_cast<int>(std::round( mouseMoveEvent.x)),
			.yPixel = static_cast<int>(std::round(mouseMoveEvent.y))
		}
	{
		
	}

	MouseMoveEvent() = delete;
	virtual ~MouseMoveEvent() = default;
	MouseMoveEvent(const MouseMoveEvent &other) = default;
	MouseMoveEvent(MouseMoveEvent &&other) = delete;
	MouseMoveEvent& operator=(const MouseMoveEvent &other) = delete;
	MouseMoveEvent& operator=(MouseMoveEvent &&other) = delete;
	
	SDL_MouseMotionEvent const mouseMoveEvent;
	PosPixel const mousePosition;
};

} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSEMOVEEVENT_H_ */
