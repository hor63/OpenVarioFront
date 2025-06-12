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
#include "GLES/VecMat.h"
#include <cstdint>

namespace OevGLES {

/** \brief Draws a partial arc of circular bodies
 *
 * This class draws all kinds of circular shapes which are possible with \ref CircleBaseRenderer.
 * However, this class draws a partial arc.\n
 * You can define the range which the arc covers from 0-360 deg.
 * You can also define the start angle where the arc starts.
 * Both angles *always* count counter-clock wise. Negative values, and/or values 
 * >= 360.0 or =< 360.0 are accepted.
 * If you want the angles run clock-wise you can use negative angle values.
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

	virtual void setupVertexBuffers() override;

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
	
	/** \brief Normalize angles to be positive between 0 and 360 deg.
	*/
	static double normalizeAngleDeg(double angleDeg);

protected:

	/// Dirty flag for \ref startAngleDeg and \ref arcRangeDeg
	bool dirtyArcAngles= true;

	/** \brief The angle where the arc (visible part of the circle starts)
	 *
	 * The angle can be positive or negative, and exceed +360.0 or -360.0.
	 *
	 * Internally the angle is normalized as a positive angle between 0 and 360.0
	 * to \ref startAngleDegNormalized
	 */
	double startAngleDeg = 0.0;

	/** \brief Normalized start angle of the arc; 
	 * 0.0 <= \p startAngleDegNormalized < 360.0
	 *
	 * Negative angles or angles >= 360.0 are transformed into a positive angle
	 * < 360.0.
	 */
	double startAngleDegNormalized = 0.0;
	
	/** \brief The visible part of the circle in degrees starting from \ref startAngleDeg
	 *
	 * You can define the range positive (counter-clock wise) or negative (clock-wise).
	 *
	 * If \p arcRangeDeg is <= -360.0 or is >= 360.0 the arc degrades to a full circle.
	 
	 * If the range angle is negative it is made positive, and \ref startAngleDegNormalized
	 * now starts at the previous end. Thus the arc can progress in positive direction.
	 */
	double arcRangeDeg = 360.0;
	
	/** \brief Normalized arc range.
	 *
	 * The normalized arc range is
	 *   - positive
	 *   - 0.0 <= \p arcRangeDegNormalized <= 360.0
	 *   - | \ref arcRangeDeg | > 360.0 is cropped to 360.0. More than a full circle
	 *     is not a thing.
	 *
	 * A negative \ref arcRangeDeg also affects \ref startAngleDegNormalized because
	 * I need to start at the original end of the arc and draw the arc other way around.
	 */
	double arcRangeDegNormalized = 360.0;
	
	/// \brief \p true when | \ref arcRangeDeg | >= 360.0 
	bool isFullCircle = true;
	
	/// Number of segments of the \ref vertexArrayStruct object
	/// to approximate the \ref arcRangeDeg angle.
	uint32_t numSegmentsArc = 0U;
	
	/// Calculated from \ref startAngleDeg
	Mat4 rotMatrixStartAngle;
	
	/// The MVPMatrix from the MVP matrix parameter in \ref draw() and
	/// \ref rotMatrixStartAngle
	Mat4 effMVPMatrix;

	/** \brief Rotation matrix for the last partial segment to ending exactly on
	 * the target range angle
	 */
	Mat4 rotMatrixRangeEnd;
	
	/// The MVPMatrix from the MVP matrix parameter in \ref draw() and
	/// \ref rotMatrixRangeEnd
	Mat4 effMVPMatrixRangeEnd;
	
	/// Vertexes for the end segment for an arc with an arbitrary angle
	CirclePolygonVertexContainer::CirclePolygonVertexStruct endArcVertexes [4];
	
	/** \brief Take \ref startAngleDeg and \ref arcRangeDeg and normalize them
	 * into \ref startAngleDegNormalized and \ref arcRangeDegNormalized
	 *
	 \see \ref startAngleDegNormalized and \ref arcRangeDegNormalized
	 */
	void normalizeAngles ();
};

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEPARTIALARCRENDERER_H_ */
