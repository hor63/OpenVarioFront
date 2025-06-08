/*
 * CirclePartialArcRenderer.h
 *
 *  Created on: Jun 4, 2025
 *      Author: hor
*
 *	Sub-class of CircleBaseRenderer to render partial arcs
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

#ifndef RENDERERS_CIRCLEPARTIALARCRENDERER_H_
#define RENDERERS_CIRCLEPARTIALARCRENDERER_H_

#include "CircleBaseRenderer.h"

namespace OevGLES {

/** \brief Draws a partial arc of circular bodies
 *
 * This class draws all kinds of circular shapes which are possible with \ref CircleBaseRenderer.
 * However, this class draws a partial arc.\n
 * You can define the range which the arc covers from 0-360 deg.
 * You can also define the start angle where the arc starts.
 * Both angles *must* be positive, and they *always* count counter-clock wise.
 * If you want the angles run clock-wise, or negative, do the math yourself to match
 * the abovementioned conditions.
 * The start angle 0 is the x-axis (to the right).
 *
 * Default start angle is 0 deg; default arc range is 360 deg.
 *
 * All other settings of the circle properties, like radiuses and z-offset of
 * base class \ref CircleBaseRenderer apply here too.
 *
 */
class CirclePartialArcRenderer: public CircleBaseRenderer {
public:
	CirclePartialArcRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer);
	virtual ~CirclePartialArcRenderer();

	virtual void draw(const OevGLES::Mat4 &modelMatrix,
			const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
			const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
			const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
			const OevGLES::Vec4 &ambientLightColor) override;

	double getArcRangeDeg() const {
		return arcRangeDeg;
	}

	void setArcRangeDeg(double arcRangeDeg);

	double getStartAngleDeg() const {
		return startAngleDeg;
	}

	void setStartAngleDeg(double startAngleDeg);

protected:

	/// Local dirty flag \n
	/// It is kept distinct from the dirty flag of the base class
	/// \ref CircleBaseRenderer
	bool dirtyArc = true;

	double startAngleDeg = 0.0;
	double arcRangeDeg = 360.0;
};

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEPARTIALARCRENDERER_H_ */
