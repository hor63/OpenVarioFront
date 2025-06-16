/*
 * CirclePartialArcRenderer.cpp
 *
 *  Created on: Jun 4, 2025
 *      Author: hor
 */

#include <cmath>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "CirclePartialArcRenderer.h"
#include "CircleBaseRenderer.h"
#include "GLES/VecMat.h"


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CirclePartialArcRenderer::CirclePartialArcRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer)
		:CircleBaseRenderer{circlePolygonVertexContainer}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CirclePartialArcRenderer");
	}
#endif
	
	
	// Initialize endArcVertexes with default values.
	// Please note that x and y of element 2 and 3 are being adjusted
	// later at runtime
	// x
	endArcVertexes[0].position[0] =
			endArcVertexes[1].position[0] =
			endArcVertexes[0].normal[0] =
			endArcVertexes[1].normal[0] =
			endArcVertexes[2].position[0] =
			endArcVertexes[3].position[0] =
			endArcVertexes[2].normal[0] =
			endArcVertexes[3].normal[0] =
					1.0f;

	// y
	endArcVertexes[0].position[1] =
			endArcVertexes[1].position[1] =
			endArcVertexes[0].normal[1] =
			endArcVertexes[1].normal[1] =
			endArcVertexes[2].position[1] =
			endArcVertexes[3].position[1] =
			endArcVertexes[2].normal[1] =
			endArcVertexes[3].normal[1] =
					0.0f;

	// z
	// The secondary circle has the Z-offset
	endArcVertexes[0].position[2] = 1.0f;
	endArcVertexes[2].position[2] = 1.0f;

	// Alternate circle is first. Assumption is that the alternate circle
	// is the inner (smaller) circle, and/or is the circle in positive
	// z-direction.
	// The direction of the triangle strip is counter-clock wise.
	// Thus the drawing direction of the triangles is counter-clock
	// wise, and in direction of the normal vector.
	endArcVertexes[0].isSecondaryCircle = 1.0f;
	endArcVertexes[2].isSecondaryCircle = 1.0f;
}

CirclePartialArcRenderer::~CirclePartialArcRenderer() {

}

void CirclePartialArcRenderer::setArcRange(AngleDeg arcRange) {
	
	if (this->arcRange != arcRange) {
		this->arcRange = arcRange;

		dirtyArcAngles = true;
	}
}

void CirclePartialArcRenderer::setStartAngle(AngleDeg startAngle) {
	
	if (this->startAngle != startAngle) {
		this->startAngle = startAngle;

		dirtyArcAngles = true;
	}
}

AngleDeg CirclePartialArcRenderer::normalizeAngle(AngleDeg angle) {
	if (angle >= 360.0_deg || (angle <= (360.0_deg*-1.0))) {
		angle = AngleDeg::makeAngle(std::fmod(angle.getAngleValue(), 360.0));
	}
	
	if (angle < 0.0_deg) {
		angle = 360.0_deg - angle;
	}
	
	return angle;
}

void CirclePartialArcRenderer::setupVertexBuffers() {
	
	// Call the base class. It does the heavy lifting
	CircleBaseRenderer::setupVertexBuffers();
	
	if (dirtyArcAngles) {
		normalizeAngles();
		dirtyArcAngles = false;
	}
}

void CirclePartialArcRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
	
	setupVertexBuffers();
}

void CirclePartialArcRenderer::normalizeAngles () {

	LOG4CXX_DEBUG(logger, __FUNCTION__
		<< ": arcRangeDeg = " << arcRange.getAngleValue()
		<< ", startAngleDeg = " << startAngle.getAngleValue()
		);

	if (arcRange >=360.0_deg || arcRange <= (360.0_deg * -1.0f)) {
		// Other considerations are moot since now
		// the full circle draw method of the base classs is being called.
		isFullCircle = true;
		
		LOG4CXX_DEBUG(logger, "\tPaint a full circle");

	} else {
		isFullCircle = false;
		
		if (arcRange < 0.0_deg) {
			// let the arc start at the end but draw the arc now counter-clock wise.
			startAngleNormalized = normalizeAngle(startAngle + arcRange);
			arcRangeNormalized = arcRange * -1.0f;
		} else {
			startAngleNormalized = normalizeAngle(startAngle);
			arcRangeNormalized = arcRange;
		}

		rotMatrixStartAngle = rotationMatrixZ (startAngleNormalized);
		LOG4CXX_DEBUG(logger, 
			"\t startAngleDegNormalized = " << startAngle.getAngleValue()
			<< ", rotMatrixStartAngle = \n" << rotMatrixStartAngle);
	
		numSegmentsArc = 
			static_cast<uint32_t>( vertexArrayStruct->numSegments * (arcRangeNormalized / AngleRad::fullCircle()));
	
		LOG4CXX_DEBUG(logger, __FUNCTION__
			<< ": arcRangeDeg = " << arcRangeNormalized.getAngleValue()
			<< ", numSegmentsArc = " << numSegmentsArc
			<< " of " << vertexArrayStruct->numSegments
			<< " for a full circle.");
			
		
	}


}
} /* namespace OevGLES */
