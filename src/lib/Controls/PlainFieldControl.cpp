/*
 * PlainFieldControl.cpp
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "PlainFieldControl.h"

namespace OevControls {

PlainFieldControl::PlainFieldControl(ControlsContainerWeakPtr const &parent,
		RendererContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name) :
		ControlBase(parent,renderContextPtr,uuid,name)
{

}

PlainFieldControl::~PlainFieldControl() {
	// TODO Auto-generated destructor stub
}

} /* namespace OevControls */
