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
#include "Events/MouseLeavesControlEventHandler.h"

namespace OevControls {

class ButtonControl: public TextFieldControl {
public:

	class MouseEntersButtonHandler :public MouseEntersControlEventHandler {
		
		friend class ButtonControl;
		
	public:
		
		// This will be the one and only object. No copies.
		MouseEntersButtonHandler(MouseEntersButtonHandler const &) = delete;
		MouseEntersButtonHandler(MouseEntersButtonHandler &&) = delete;
		MouseEntersButtonHandler& operator = (MouseEntersButtonHandler const &) = delete;
		MouseEntersButtonHandler& operator = (MouseEntersButtonHandler &&) = delete;
		
		virtual void mouseEntersControl (SDL_MouseMotionEvent& mouseMoveEvent) override;
		
	private:

		MouseEntersButtonHandler(ButtonControl & buttonObj) :
			buttonObj {buttonObj}
		{}
	
		/** \brief Reference to the owning object.
		 *
		 * Using a raw reference is allowed here: Only \ref ButtonControl can create an object which is
		 * \ref ButtonControl::mouseEntersFunctor.
		 * No copies can be created. References are only passed around as std::weak_ptr objects.
		 * When the \ref ButtonControl is destroyed the weak pointers to this are also becoming emtpy.
		 */
		ButtonControl & buttonObj;
	}; // class MouseEntersFunctor

	class MouseLeavesButtonFunctor :public MouseLeavesControlEventHandler {
		
		friend class ButtonControl;
		
	public:
		
		// This will be the one and only object. No copies.
		MouseLeavesButtonFunctor(MouseLeavesButtonFunctor const &) = delete;
		MouseLeavesButtonFunctor(MouseLeavesButtonFunctor &&) = delete;
		MouseLeavesButtonFunctor& operator = (MouseLeavesButtonFunctor const &) = delete;
		MouseLeavesButtonFunctor& operator = (MouseLeavesButtonFunctor &&) = delete;
		
		virtual void mouseLeavesControl (SDL_MouseMotionEvent& mouseMoveEvent) override;
		
	private:

		MouseLeavesButtonFunctor(ButtonControl & buttonObj) :
			buttonObj {buttonObj}
		{}

		/** \brief Reference to the owning object.
		 *
		 * Using a raw reference is allowed here: Only \ref ButtonControl can create an object which is
		 * \ref ButtonControl::mouseEntersFunctor.
		 * No copies can be created. References are only passed around as std::weak_ptr objects.
		 * When the \ref ButtonControl is destroyed the weak pointers to this are also becoming emtpy.
		 */
		ButtonControl & buttonObj;
	}; // class MouseEntersFunctor


	friend class MouseEntersButtonHandler;
	friend class MouseLeavesButtonFunctor;

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

MouseEntersButtonHandler mouseEntersHandler;
MouseLeavesButtonFunctor mouseLeavesHandler;
	
	void mouseEntersButton ();
	void mouseLeavesButton ();

};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_BUTTONCONTROL_H_ */
