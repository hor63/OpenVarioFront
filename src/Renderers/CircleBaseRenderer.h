/*
 * CircleBaseRenderer.h
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

#ifndef RENDERERS_CIRCLEBASERENDERER_H_
#define RENDERERS_CIRCLEBASERENDERER_H_

#include "RendererBase.h"

namespace OevGLES {

/** \brief Base class for all things circular
 *
 * This class can draw these types of objects:
 *
 *   - A ring with an inner and outer radius (like a washer).
 *     The y-offset is 0.0.\n
 *     - A full circle is just a degenerated ring with an inner radius = 0.
 *   - A cylinder with primary and secondary radius equal, and z-offset > 0.0.
 *   - A cone. The intermediary between the cylinder and the ring.
 *     The secondary radius is smaller than the primary one. Simultaneously
 *     the z-offset is > 0.0.
 *
 */
class CircleBaseRenderer: public RendererBase {
public:

	CircleBaseRenderer();
	virtual ~CircleBaseRenderer();

	double getPrimaryRadius() const {
		return primaryRadius;
	}

	double getPrimarySecondaryZOffset() const {
		return primarySecondaryZOffset;
	}

	double getSecondaryRadius() const {
		return secondaryRadius;
	}

	void setPrimaryRadius(double primaryRadius = 1.0);
	void setPrimarySecondaryZOffset(double primarySecondaryZOffset = 1.0);
	void setSecondaryRadius(double secondaryRadius = 1.0);
	virtual void setupVertexBuffers() override;
	virtual void draw(const OevGLES::Mat4 &modelMatrix,
			const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
			const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
			const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
			const OevGLES::Vec4 &ambientLightColor) override;

private:

	/// \brief Any rendering parameter changed.
	bool dirty = true;

	/// \brief The outer radius, and/or the radius around z=0
	double primaryRadius = 1.0;
	/// \brief Inner and/or radius of the circle offset with z>0
	double secondaryRadius = 1.0;
	/// \brief The z-offset of the secondary circle from the primary circle
	/// which is always on z=0.0.
	double primarySecondaryZOffset = 1.0;

	;

	Vec4 normalVectorFactors;
}; // class CircleBaseRenderer

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEBASERENDERER_H_ */
