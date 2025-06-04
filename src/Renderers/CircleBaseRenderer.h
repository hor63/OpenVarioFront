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
#include "CirclePolygonVertexContainer.h"
#include "GLPrograms/GLProgDiffLightCircle.h"

namespace OevGLES {

/** \brief Base class for all things circular
 *
 * This class can draw these types of objects:
 *
 *   - A ring with an inner and outer radius (like a washer).
 *     The y-offset is 0.0.\n
 *     - A full circle is just a degenerated ring with an inner radius = 0.
 *       However, an optimized circle or cone is available with \ref CircleFilledRenderer.
 *   - A cylinder with primary and secondary radius equal, and z-offset > 0.0.
 *   - A conical frustum. The intermediary between the cylinder and the ring.
 *     The secondary radius is smaller than the primary one. Simultaneously
 *     the z-offset is > 0.0.
 *     - A pointy cone with the secondary radius=0.
 *       However, an optimized circle or cone is available with \ref CircleFilledRenderer.
 *
 * The normal at the polygon segment borders is the average between the normals of the two
 * adjacent polygon segments. Thus the appearance of such a body in directional light is smooth,
 * hiding the segments of the polygon.
 */
class CircleBaseRenderer: public RendererBase {
public:

	/// Obtain the circlePolygonVertexContainer reference from your
	/// \ref GLFramework object.
	CircleBaseRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer);
	virtual ~CircleBaseRenderer();

	double getPrimaryRadius() const {
		return primaryRadius;
	}

	double getPrimarySecondaryZOffset() const {
		return primarySecondaryZOffset;
	}

	CirclePolygonVertexContainer& getCirclePolygonVertexContainer() {
		return circlePolygonVertexContainer;
	}

	double getSecondaryRadius() const {
		return secondaryRadius;
	}

	/// Color format is RGBA.
	/// Values are normalized, i.e. the range is 0.0 to 1.0.
	Vec4 getBodyColor() {
		return bodyColor;
	}

	void setPrimaryRadius(double primaryRadius);
	void setPrimarySecondaryZOffset(double primarySecondaryZOffset);
	void setSecondaryRadius(double secondaryRadius);
	/// Color format is RGBA.
	/// Values are normalized, i.e. the range is 0.0 to 1.0.
	void setBodyColor(const Vec4& colorRGBA) {
		bodyColor = colorRGBA;
	}
	virtual void setupVertexBuffers() override;
	virtual void draw(const OevGLES::Mat4 &modelMatrix,
			const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
			const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
			const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
			const OevGLES::Vec4 &ambientLightColor) override;

protected:

	/// \brief Any rendering parameter changed.
	bool dirty = true;

	/// \brief The outer radius, and/or the radius around z=0
	double primaryRadius = 1.0;
	/// \brief Inner and/or radius of the circle offset with z>0
	double secondaryRadius = 1.0;
	/// \brief The z-offset of the secondary circle from the primary circle
	/// which is always on z=0.0.
	double primarySecondaryZOffset = 1.0;

	// Client side uniforms
	GLfloat vecFactorPrimaryVertex [4] = {1,1,0,1};
	GLfloat vecFactorSecondVertex [4] = {1,1,1,1};
	GLfloat vecFactorNormalVector [4] = {1,0,1,0};

	// Client side fixed attributes
	Vec4 bodyColor = {1.0f,1.0f,1.0f,1.0f};

	/// The cache object to provide the vertex arrays
	CirclePolygonVertexContainer& circlePolygonVertexContainer;

	/// The carrier of the suitable vertex buffer when \ref dirty is \p false.
	CirclePolygonVertexContainer::CircleVertexArrayStruct const *
		vertexArrayStruct = nullptr;

	GLProgDiffLightCircle *glProgram = nullptr;

	Vec4 normalVectorFactors = {1,1,1,1};
}; // class CircleBaseRenderer

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEBASERENDERER_H_ */
