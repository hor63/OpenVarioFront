/*
 * MouseEventHandlers.h
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

#ifndef LIB_GUI_EVENTS_MOUSEEVENTHANDLERS_H_
#define LIB_GUI_EVENTS_MOUSEEVENTHANDLERS_H_

#include "Events/OVFEvents.h"

namespace OevControls {

/** \brief Abstract/pure virtual mouse move handler
 *
 * This is a pure virtual class. It must be overloaded.
 */
class MouseMoveEventHandler {
public:
	MouseMoveEventHandler() {}
	virtual ~MouseMoveEventHandler();

	/** \brief An SDL mouse moved within this control
	 *
	 *
	 * \param mouseMoveEvent Event structure with the original SDL mouse move event
	 *		and coordinates converted to the GL coordinates.
	 *
	 * \return	\p true when the handler deal with the event finally.
	 * 			\p false when the handler did not handle the event.
	 *					In this case the caller needs to raise the event
	 *					with the control's parent.
	 */
	virtual bool mouseMoves (MouseMoveEvent const &mouseMoveEvent) = 0;
	
}; // class MouseMoveEventHandler

/** \brief Abstract/pure virtual handler when the mouse pointer enters a control
 * 
 * It is fired by a \ref MouseMoveEventHandler for a \ref ControlsContainer
 * when it figures that the mouse cursor entered a control 
 *
 * This is a pure virtual class. It must be overloaded.
 */
class MouseEntersControlEventHandler {
public:
	MouseEntersControlEventHandler() {}
	virtual ~MouseEntersControlEventHandler();
	
	/// \brief The mouse cursor entered this control from outside.
	virtual void mouseEntersControl () = 0;

};

/** \brief Abstract/pure virtual handler when the mouse pointer leaves a control
 * 
 * It is fired by a \ref MouseMoveEventHandler for a \ref ControlsContainer
 * when it figures that the mouse cursor entered a control 
 *
 * This is a pure virtual class. It must be overloaded.
 *
 */
class MouseLeavesControlEventHandler {
public:
	MouseLeavesControlEventHandler() {}
	virtual ~MouseLeavesControlEventHandler();
	
	/// \brief The mouse cursor left this control.
	virtual void mouseLeavesControl () = 0;

};

/** \brief Abstract/pure virtual handler when mouse button is being pressed.
 * 
 * It is fired by a \ref MouseMoveEventHandler for a \ref ControlsContainer
 * when it figures that the mouse cursor entered a control 
 *
 * This is a pure virtual class. It must be overloaded.
 *
 */
class MouseButtonDownEventHandler {
public:
	MouseButtonDownEventHandler() {}
	virtual ~MouseButtonDownEventHandler();
	
	/** \brief A mouse button was pressed.
	 *
	 * Get the button which was pressed from SDL_MouseButtonEvent::button.
	 * 
	 *  \note The button index SDL_MouseButtonEvent::button conforms to the bit mask constants
	 *  SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE and SDL_BUTTON_RIGHT
	 */
	virtual void mouseButtonDown (SDL_MouseButtonEvent &mouseButtonEvent) = 0;

};

/** \brief Abstract/pure virtual handler when mouse button is being pressed.
 * 
 * It is fired by a \ref MouseMoveEventHandler for a \ref ControlsContainer
 * when it figures that the mouse cursor entered a control 
 *
 * This is a pure virtual class. It must be overloaded.
 *
 */
class MouseButtonUpEventHandler {
public:
	MouseButtonUpEventHandler() {}
	virtual ~MouseButtonUpEventHandler();
	
	/** \brief A mouse button was released.
	 *
	 * Get the button which was released from SDL_MouseButtonEvent::button.
	 * 
	 *  \note The button index SDL_MouseButtonEvent::button conforms to the bit mask constants
	 *  SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE and SDL_BUTTON_RIGHT
	 */
	virtual void mouseButtonUp (SDL_MouseButtonEvent &mouseButtonEvent) = 0;

};

/** \brief Abstract/pure virtual handler when a single click on a control occurred.
 * 
 * A single click event is fired when 
 *	- a mouse button up event is received for the same control that received the previous mouse button down event
 *	- for the same button as the previous mouse button down event
  *	- and SDL_MouseButtonEvent::clicks == 1.
 *
 */
class MouseSingleKlickEventHandler {
public:
	MouseSingleKlickEventHandler() {}
	virtual ~MouseSingleKlickEventHandler();
	
	/** \brief The control was single-clicked.
	 *
	 * Get the button which clicked from SDL_MouseButtonEvent::button.
	 * 
	 *  \note The button index SDL_MouseButtonEvent::button conforms to the bit mask constants
	 *  SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE and SDL_BUTTON_RIGHT
	 */
	virtual void mouseSingleClick (SDL_MouseButtonEvent &mouseButtonEvent) = 0;

};

/** \brief Abstract/pure virtual handler when a double-click on a control occurred.
 * 
 * A double click event is fired when 
 *	- a mouse button up event is received for the same control that received the previous mouse button down event
 *	- for the same button as the previous mouse button down event
 *	- for the same control which received the previous single click event.
 *	- and SDL_MouseButtonEvent::clicks == 2.
 
 *	\note Only exact double-clicks will cause a double-click event. Any more clicks in quick succession will be ignored.
 *	Only a break between multi-clicks which is longer then the multi-click threshold time will reset the click count.
 *
 */
class MouseDoubleKlickEventHandler {
public:
	MouseDoubleKlickEventHandler() {}
	virtual ~MouseDoubleKlickEventHandler();
	
	/** \brief The control was double-clicked.
	 *
	 * Get the button which clicked from SDL_MouseButtonEvent::button.
	 * 
	 *  \note The button index SDL_MouseButtonEvent::button conforms to the bit mask constants
	 *  SDL_BUTTON_LEFT, SDL_BUTTON_MIDDLE and SDL_BUTTON_RIGHT
	 */
	virtual void mouseDoubleClick (SDL_MouseButtonEvent &mouseButtonEvent) = 0;

};


} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSEEVENTHANDLERS_H_ */
