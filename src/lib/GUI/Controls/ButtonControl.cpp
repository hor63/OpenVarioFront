/*
 * ButtonControl.cpp
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
#include "OVFCommon.h"


#include "ButtonControl.h"

namespace OevControls {

ButtonControl::ButtonControl(ControlsContainerWeakPtr const &parent,
							 std::weak_ptr<ButtonControl> pointerToSelf,
							 RenderContextSharedPtr const &renderContextPtr,
							 OevUtil::Uuid const &uuid, char const *name) :
		TextFieldControl(parent, pointerToSelf, renderContextPtr, uuid, name),
		mouseEnterHandler {
			MouseEntersFunctor (pointerToSelf)}
	{
		// look&feel is hardcoded flat style.
		hasFrame_ = true;
		mouseEntersEventHandler = &mouseEnterHandler;
	}

ButtonControl::~ButtonControl() {
	// Nothing to to here myself.
}

} /* namespace OevControls */
