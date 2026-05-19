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

#include "Events/OVFEvents.h"

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
	virtual bool mouseMoves (MouseMoveEvent const &mouseMoveEvent) = 0;
	
}; // class MouseMoveEventHandler

} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSEMOVEEVENTHANDLER_H_ */
