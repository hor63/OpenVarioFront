/*
 * CircleFilledRenderer.cpp
 *
 *  Created on: Jun 1, 2025
 *      Author: hor
 *
  *
 *	Sub-class of CircleBaseRenderer to render completely filled circles
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "CircleFilledRenderer.h"

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CircleFilledRenderer::CircleFilledRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer):
		CircleBaseRenderer {circlePolygonVertexContainer}
	{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CircleBaseRenderer");
	}
#endif

}

CircleFilledRenderer::~CircleFilledRenderer() {
}

void CircleFilledRenderer::setupVertexBuffers() {
}

void CircleFilledRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
}

} /* namespace OevGLES */
