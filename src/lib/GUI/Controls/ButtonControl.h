/*
 * ButtonControl.h
 *
 *  Created on: Jan 16, 2026
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

#ifndef LIB_CONTROLS_BUTTONCONTROL_H_
#define LIB_CONTROLS_BUTTONCONTROL_H_

#include <memory>

#include "TextFieldControl.h"
#include "Events/MouseEntersControlEventHandler.h"

namespace OevControls {

class ButtonControl: public TextFieldControl {
public:

class MouseEntersFunctor {
public:
	MouseEntersFunctor(std::weak_ptr<ControlBase> const& weakCtrlPtr) :
	buttonWeakPtr{std::dynamic_pointer_cast<ButtonControl>(weakCtrlPtr.lock())}
	{}
	
	void operator() (SDL_MouseMotionEvent& mouseMoveEvent) {
		auto buttonPtr = buttonWeakPtr.lock();
		
		if (buttonPtr) {
			buttonPtr->mouseEntersButton();
		}
	}
	
private:

std::weak_ptr<ButtonControl> buttonWeakPtr;
}; // class MouseEntersFunctor
class MouseLeavesFunctor {
public:
	MouseLeavesFunctor(std::weak_ptr<ControlBase> const& weakCtrlPtr) :
	buttonWeakPtr{std::dynamic_pointer_cast<ButtonControl>(weakCtrlPtr.lock())}
	{}
	
	void operator() (SDL_MouseMotionEvent& mouseMoveEvent) {
		auto buttonPtr = buttonWeakPtr.lock();
		
		if (buttonPtr) {
			buttonPtr->mouseLeavesButton();
		}
	}
	
private:

std::weak_ptr<ButtonControl> buttonWeakPtr;
}; // class MouseLeavesFunctor

friend class MouseEntersFunctor;

	ButtonControl(ControlsContainerWeakPtr const &parent,
 		std::weak_ptr<ButtonControl> pointerToSelf,
		RenderContextSharedPtr const &renderContextPtr,
		OevUtil::Uuid const &uuid, char const *name);
	virtual ~ButtonControl();
	ButtonControl(const ButtonControl &other) = delete;
	ButtonControl(ButtonControl &&other) = delete;
	ButtonControl& operator=(const ButtonControl &other) = delete;
	ButtonControl& operator=(ButtonControl &&other) = delete;
	
private:

	MouseEntersControlEventHandlerProxy<MouseEntersFunctor> mouseEnterHandler;

	void mouseEntersButton ();
	void mouseLeavesButton ();

};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_BUTTONCONTROL_H_ */
