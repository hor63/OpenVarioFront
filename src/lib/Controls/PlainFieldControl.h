/*
 * PlainFieldControl.h
 *
 *  Created on: Oct 22, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2018  Kai Horstmann
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

#ifndef LIB_CONTROLS_PLAINFIELDCONTROL_H_
#define LIB_CONTROLS_PLAINFIELDCONTROL_H_

#include <ControlBase.h>

namespace OevControls {

class PlainFieldControl :public ControlBase {
public:
	PlainFieldControl(ControlsContainerWeakPtr const &parent,
		OevUtil::Uuid const & uuid,
		char const* name = "");
	virtual ~PlainFieldControl();
	PlainFieldControl(const PlainFieldControl &other) = delete;
	PlainFieldControl(PlainFieldControl &&other) = delete;
	PlainFieldControl& operator=(const PlainFieldControl &other) = delete;
	PlainFieldControl& operator=(PlainFieldControl &&other) = delete;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_PLAINFIELDCONTROL_H_ */
