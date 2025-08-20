/*
 * ControlsContainer.cpp
 *
 *  Created on: Aug 14, 2025
 *      Author: hor
 *
 *	Base class for any control which can host other controls, and a tab group,
 *  like dialogs, canvases, and the root control
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
#include "ControlBase.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include <utility>

#include "ControlsContainer.h"

namespace OevControls {


ControlsContainer::~ControlsContainer() {
	// TODO Auto-generated destructor stub
}

void ControlsContainer::addControl(ControlBasePtr const &controlPtr) {
	controlsMap.insert(std::pair(controlPtr->getUuid(),controlPtr));
}

void ControlsContainer::appendControlToTabGroup (ControlBaseWeakPtr const &controlWeakPtr) {
	tabGroup.push_back(controlWeakPtr);
}

void ControlsContainer::insertControlInTabGroupBefore (ControlsWeakListT::iterator ref,
		ControlBaseWeakPtr const &controlWeakPtr) {
	tabGroup.insert (ref,controlWeakPtr);
}

} /* namespace OevControls */
