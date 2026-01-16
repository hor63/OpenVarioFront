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

#include "lib/Controls/TextFieldControl.h"

namespace OevControls {

class ButtonControl: public TextFieldControl {
public:
	ButtonControl(ControlsContainerWeakPtr const &parent,
				  RenderContextSharedPtr const &renderContextPtr,
				  OevUtil::Uuid const &uuid, char const *name);
	virtual ~ButtonControl();
	ButtonControl(const ButtonControl &other) = delete;
	ButtonControl(ButtonControl &&other) = delete;
	ButtonControl& operator=(const ButtonControl &other) = delete;
	ButtonControl& operator=(ButtonControl &&other) = delete;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_BUTTONCONTROL_H_ */
