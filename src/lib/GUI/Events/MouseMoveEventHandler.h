/*
 * MouseMoveEventHandler.h
 *
 *  Created on: Apr 3, 2026
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

#ifndef LIB_GUI_EVENTS_MOUSEMOVEEVENTHANDLER_H_
#define LIB_GUI_EVENTS_MOUSEMOVEEVENTHANDLER_H_

#include <SDL3/SDL_events.h>
#include <memory>

namespace OevControls {

/** \brief Abstract/pure virtual mouse move handler
 *
 * Must be overloaded. This is usually done with an instance of 
 * template class \ref MouseMoveEventHandlerProxy
 */
class MouseMoveEventHandler {
public:
	MouseMoveEventHandler() {}
	virtual ~MouseMoveEventHandler();

	/** \brief An SDL mouse moved within this control
	 *
	 * \return	\p true when the handler deal with the event finally.
	 * 			\p false when the handler did not handle the event.
	 *					In this case the caller needs to raise the event
	 *					with the control's parent.
	 */
	virtual bool mouseMoves (SDL_MouseMotionEvent &mouseMoveEvent) = 0;
	
}; // class MouseMoveEventHandler

/** \brief Template class for an implementation of the class specific event handler
 *
 * The type \p Functor is either a lambda capturing \p this of the object which creates the lambda,
 * or a functor class whose operator () handles the event.
 *
 * Either way the operator () must return \p bool, and have a parameter \p SDL_MouseMotionEvent&.
 */
template <typename Lambda>
class MouseMoveEventHandlerProxy : public MouseMoveEventHandler {
public:

	MouseMoveEventHandlerProxy (Lambda &eventProcessor) :
		eventProcessor {eventProcessor}
	{}
	
	/// \brief An SDL mouse moved within this control
	virtual bool mouseMoves (SDL_MouseMotionEvent &mouseMoveEvent) override {
		return eventProcessor(mouseMoveEvent);
	}	
		
private:

	Lambda eventProcessor;	

}; // template  class MouseMoveProxy


template <typename T>
auto getMouseMoveHandler (std::shared_ptr<T>& controlShPtr) {
	std::weak_ptr<T> controlWeakPtr = controlShPtr;
	auto mouseMoveHandler =
		[controlWeakPtr] (SDL_MouseMotionEvent &mouseMoveEvent) -> bool {
			auto controlShPtr = controlWeakPtr.lock();
			if (controlShPtr) {
				return controlShPtr->handleMouseMove(mouseMoveEvent);
			} else {
				return false;
			}
		};

	MouseMoveEventHandlerProxy mouseMoveHandlerProxy (mouseMoveHandler);
		
	return mouseMoveHandlerProxy;
}

template <typename T>
auto getMouseMoveHandler (T & control) {
	auto shPointerToSelf = control.getPointerToSelf().lock();
	auto controlShPtr = std::dynamic_pointer_cast<T>(shPointerToSelf);
	
	return getMouseMoveHandler (controlShPtr);
}


} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSEMOVEEVENTHANDLER_H_ */
