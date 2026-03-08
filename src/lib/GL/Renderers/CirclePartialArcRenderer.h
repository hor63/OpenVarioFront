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
	CirclePartialArcRenderer(
		RenderContextSharedPtr const &context);
	virtual ~CirclePartialArcRenderer();

	virtual void setupVertexBuffers() override;

	virtual void draw(RenderStandardUniforms const &stdUniformData) override;

	AngleDeg getArcRange() const {
		return arcRange;
	}

	void setArcRange(AngleDeg arcRange);

	AngleDeg getStartAngle() const {
		return startAngle;
	}

	void setStartAngle(AngleDeg startAngle);
	
	/** \brief Normalize angles to be positive between 0 and 360 deg.
	*/
	template <int32_t Numerator,int32_t Denominator>
	static Angle<Numerator,Denominator> normalizeAngle(Angle<Numerator,Denominator> angle);

protected:

	/// Dirty flag for \ref startAngle and \ref arcRange
	bool dirtyArcAngles= true;

	/** \brief The angle where the arc (visible part of the circle starts)
	 *
	 * The angle can be positive or negative, and exceed +360.0 or -360.0.
	 *
	 * Internally the angle is normalized as a positive angle between 0 and 360.0
	 * to \ref startAngleNormalized
	 */
	AngleDeg startAngle = AngleDeg::makeAngle(0.0f);

	/** \brief Normalized start angle of the arc; 
	 * 0.0 <= \p startAngleDegNormalized < 360.0
	 *
	 * Negative angles or angles >= 360.0 are transformed into a positive angle
	 * < 360.0.
	 */
	AngleRad startAngleNormalized = AngleRad::makeAngle(0.0f);
	
	/// Calculated from \ref startAngleNormalized
	Mat4 rotMatrixStartAngle;
	
	/** \brief The visible part of the circle in degrees starting from \ref startAngle
	 *
	 * You can define the range positive (counter-clock wise) or negative (clock-wise).
	 *
	 * If \p arcRangeDeg is <= -360.0 or is >= 360.0 the arc degrades to a full circle.
	 
	 * If the range angle is negative it is made positive, and \ref startAngleNormalized
	 * now starts at the previous end. Thus the arc can progress in positive direction.
	 */
	AngleDeg arcRange = 360.0_deg;
	
	/** \brief Normalized arc range.
	 *
	 * The normalized arc range is
	 *   - positive
	 *   - 0.0 <= \p arcRangeNormalized <= 360.0
	 *   - | \ref arcRange | > 360.0 is cropped to 360.0. More than a full circle
	 *     is not a thing.
	 *
	 * A negative \ref arcRange also affects \ref startAngleNormalized because
	 * I need to start at the original end of the arc and draw the arc other way around.
	 */
	AngleRad arcRangeNormalized = 2.0_rad * M_PI;
	
	/// \brief \p true when | \ref arcRange | >= 360.0 
	bool isFullCircle = true;
	
	/// Number of segments of the \ref vertexArrayStruct object
	/// to approximate the \ref arcRange angle.
	uint32_t numSegmentsArc = 0U;
	
	/// Number of vertexes to draw to form the coarse part of the arc 
	GLsizei numVertexesArc = 0;
	
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
	std::array<CirclePolygonVertexContainer::CirclePolygonVertexStruct,4> endArcVertexes;
	
	/** \brief Handle to the GL ES vertex buffer for \ref endArcVertexes
	 *
	 * The vertex buffer is created only on demand. The handle is initialized
	 * to 0 to indicate that the vertex buffer must still be created.
	 *
	*/
	GLBufferObject vertexBufferHandleArcEnd;
	GLVertexArrayObject vertexArrayHandleArcEnd;

	
	/** \brief Take \ref startAngle and \ref arcRange and normalize them
	 * into \ref startAngleNormalized and \ref arcRangeNormalized
	 *
	 \see \ref startAngleNormalized and \ref arcRangeNormalized
	 */
	void normalizeAngles ();
};

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEPARTIALARCRENDERER_H_ */
