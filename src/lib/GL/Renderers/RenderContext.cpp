/*
 * ControlsContext.cpp
 *
 *  Created on: Oct 25, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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
#include <GLES2/gl2.h>
#include <memory>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include "RenderContext.h"

namespace OevGLES {

RenderContext::RenderContext(SDLRenderSurfaceWeakPtr const sdlRenderSurfacePtr,
							 Vec4 const &foregroundColor,
							 Vec4 const &backgroundColor) :
	sdlRenderSurfacePtr{sdlRenderSurfacePtr},
	foregroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
	backgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	inactiveForegroundColorPtr{
		std::make_shared<Vec4>(Vec4{0.5f, 0.5f, 0.5f, 1.0f})},
	inactiveBackgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	textForegroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
	textBackgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	buttonForegroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
	buttonBackgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	dialogTitleForegroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
	dialogTitleBackgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	scrollBarForegroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
	scrollBarBackgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
	quadVertexBuffer{false} {}

} // namespace OevGLES

