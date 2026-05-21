/*
 * OVFEvents.h
 *
 *  Created on: May 19, 2026
 *      Author: hor
 */

#ifndef LIB_GUI_EVENTS_OVFEVENTS_H_
#define LIB_GUI_EVENTS_OVFEVENTS_H_

#include <cmath>
#include <SDL3/SDL_events.h>
#include "PosSizePixel.h"

namespace OevControls {

	class MouseMoveEvent final {
	public:

		MouseMoveEvent (SDL_MouseMotionEvent const &mouseMoveEvent) :
			mouseMoveEvent {mouseMoveEvent},
			mousePosition {
				.xPixel = static_cast<int>(std::round( mouseMoveEvent.x)),
				.yPixel = static_cast<int>(std::round(mouseMoveEvent.y))
			}
		{
			
		}

		MouseMoveEvent() = delete;
		~MouseMoveEvent() = default;
		MouseMoveEvent(const MouseMoveEvent &other) = default;
		MouseMoveEvent(MouseMoveEvent &&other) = delete;
		MouseMoveEvent& operator=(const MouseMoveEvent &other) = delete;
		MouseMoveEvent& operator=(MouseMoveEvent &&other) = delete;
		
		SDL_MouseMotionEvent const mouseMoveEvent;
		PosPixel const mousePosition;
	};

} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_OVFEVENTS_H_ */
