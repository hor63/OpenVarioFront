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
 

RenderContext::RenderContext (
		SDLRenderSurfaceWeakPtr const& sdlRenderSurfacePtr,
		Vec4 const &foregroundColor,
		Vec4 const &backgroundColor
	) :
		sdlRenderSurfacePtr{sdlRenderSurfacePtr},
		foregroundColorPtr{std::make_shared<Vec4>(foregroundColor)},
		backgroundColorPtr{std::make_shared<Vec4>(backgroundColor)},
		textForegroundPtr{foregroundColorPtr},
		textBackgroundPtr{backgroundColorPtr},
		buttonForegroundPtr{foregroundColorPtr},
		buttonBackgroundPtr{backgroundColorPtr},
		dialogTitleForegroundPtr{foregroundColorPtr},
		dialogTitleBackgroundPtr{backgroundColorPtr},
		scrollBarForegroundPtr{foregroundColorPtr},
		scrollBarBackgroundPtr{backgroundColorPtr},
		quadVertexBufferPtr {std::make_shared<GLBufferObject>()}
		{
			/* The location of the 4 vertexes for the quad
			4   3
			|   |
			|   |
			0---1
			*/
			GLfloat vertexData[4][4] = {
				{0,0,0,1},
				{1,0,0,1},
				{1,1,0.1},
				{0,1,0,1}
			};
			
			glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
		}

} // namespace OevGLES

