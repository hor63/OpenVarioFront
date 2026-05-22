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

		MouseMoveEvent (SDL_MouseMotionEvent const &mouseMoveEvent,SizePixel const& renderSurfaceSize) :
			mouseMoveEvent {mouseMoveEvent},
			mousePosition {
				.xPixel = static_cast<int>(std::round( mouseMoveEvent.x)),
				// Please note that Y-direction is in OpenGL notion, i.e. from bottom to top,
				// whereas most window systems count y from top to bottom.
				.yPixel = renderSurfaceSize.heightPixel - static_cast<int>(std::round(mouseMoveEvent.y))
			}
		{
			
		}

		MouseMoveEvent() = delete;
		~MouseMoveEvent() = default;
		MouseMoveEvent(const MouseMoveEvent &other) = default;
		MouseMoveEvent(MouseMoveEvent &&other) = delete;
		MouseMoveEvent& operator=(const MouseMoveEvent &other) = delete;
		MouseMoveEvent& operator=(MouseMoveEvent &&other) = delete;

		/// \brief The un-altered SDL event.
		///
		/// \note The coordinates are in Windows notation, i.e. Y goes from top to bottom.
		/// X is of course going from left to right.
		SDL_MouseMotionEvent const mouseMoveEvent;
		/// \brief The mouse position in GL coordinates.
		///
		/// \note The position is in OpenGL notation, i.e. Y goes from bottom to top.
		/// X is of course going from left to right.
		PosPixel const mousePosition;
	};

} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_OVFEVENTS_H_ */
