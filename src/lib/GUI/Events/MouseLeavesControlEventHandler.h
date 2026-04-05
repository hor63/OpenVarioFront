/*
 * MouseLeavesControlEventHandler.h
 *
 *  Created on: Apr 5, 2026
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2026  Kai Horstmann
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License along
 *   with this program; if not, write to the Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef LIB_GUI_EVENTS_MOUSELEAVESCONTROLEVENTHANDLER_H_
#define LIB_GUI_EVENTS_MOUSELEAVESCONTROLEVENTHANDLER_H_

#include <SDL3/SDL_events.h>

namespace OevControls {

/** \brief Abstract/pure virtual handler when the mouse pointer Leaves a control
 * 
 * It is fired by a \ref MouseMoveEventHandler for a \ref ControlsContainer
 * when it figures that the mouse cursor entered a control 
 *
 * Must be overloaded. This is usually done with an instance of 
 * template class \ref MouseMoveEventHandlerProxy
 */
class MouseLeavesControlEventHandler {
public:
	MouseLeavesControlEventHandler() {}
	virtual ~MouseLeavesControlEventHandler();
	
	/// \brief The mouse cursor entered this control from outside.
	virtual void mouseLeavesControl (SDL_MouseMotionEvent &mouseMoveEvent) = 0;

};

/** \brief Template class for an implementation of the class specific event handler
 *
 * The type \p Lambda is usually a lambda capturing \p this of the object which creates the lambda.
 * The lambda must be of type void and have a parameter \p SDL_MouseMotionEvent&.
 */
template <typename Lambda>
class MouseLeavesControlEventHandlerProxy : public MouseLeavesControlEventHandler {
public:

	MouseLeavesControlEventHandlerProxy (Lambda &eventProcessor) :
		eventProcessor {eventProcessor}
	{}
	
	/// \brief An SDL mouse moved within this control
	virtual void mouseLeavesControl (SDL_MouseMotionEvent &mouseMoveEvent) override {
		eventProcessor(mouseMoveEvent);
	}	
		
private:

	Lambda eventProcessor;	

}; // template  class MouseLeavesControlEventHandlerProxy

} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSELEAVESCONTROLEVENTHANDLER_H_ */
