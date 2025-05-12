/*
 * CircleBaseRenderer.cpp
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 *
 *	Base class for all things circular.
 *	It is based on triangle meshes forming different types of circular forms.
 *	Derived classes will render full or partial arcs, with an inner and outer diameter
 *	or tire-like forms
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

#include "CircleBaseRenderer.h"

namespace OevGLES {

CircleBaseRenderer::CircleBaseRenderer() {
	// TODO Auto-generated constructor stub

}

CircleBaseRenderer::~CircleBaseRenderer() {
	// TODO Auto-generated destructor stub
}

void CircleBaseRenderer::setPrimaryRadius(double primaryRadius = 1.0) {
	this->primaryRadius = primaryRadius;

	dirty = true;
}

void CircleBaseRenderer::setPrimarySecondaryZOffset(
		double primarySecondaryZOffset = 1.0) {
	this->primarySecondaryZOffset = primarySecondaryZOffset;

	dirty = true;
}

void CircleBaseRenderer::setSecondaryRadius(double secondaryRadius = 1.0) {
	this->secondaryRadius = secondaryRadius;

	dirty = true;
}

void CircleBaseRenderer::setupVertexBuffers() {
}

void CircleBaseRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
}

} /* namespace OevGLES */
