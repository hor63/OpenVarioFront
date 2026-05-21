/*
 * MouseEntersControlEventHandler.h
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

#ifndef LIB_GUI_EVENTS_MOUSEENTERSCONTROLEVENTHANDLER_H_
#define LIB_GUI_EVENTS_MOUSEENTERSCONTROLEVENTHANDLER_H_

#include "Events/OVFEvents.h"


namespace OevControls {

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
	virtual void mouseEntersControl (MouseMoveEvent const &mouseMoveEvent) = 0;

};


} /* namespace OevControls */

#endif /* LIB_GUI_EVENTS_MOUSEENTERSCONTROLEVENTHANDLER_H_ */
