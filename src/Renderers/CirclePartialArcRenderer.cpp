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

void CirclePartialArcRenderer::setArcRangeDeg(double arcRangeDeg) {
	
	if (this->arcRangeDeg != arcRangeDeg) {
		this->arcRangeDeg = arcRangeDeg;

		dirtyArcAngles = true;
	}
}

void CirclePartialArcRenderer::setStartAngleDeg(double startAngleDeg) {
	
	if (this->startAngleDeg != startAngleDeg) {
		this->startAngleDeg = startAngleDeg;

		dirtyArcAngles = true;
	}
}

double CirclePartialArcRenderer::normalizeAngleDeg(double angleDeg) {
	if (angleDeg >= 360.0 || angleDeg <= -360.0) {
		angleDeg = std::fmod(angleDeg, 360.0);
	}
	
	if (angleDeg < 0.0) {
		angleDeg = 360.0 - angleDeg;
	}
	
	return angleDeg;
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

#error Actually normalize the angles here :)	

	rotMatrixStartAngle = rotationMatrixZ (startAngleDeg);
	LOG4CXX_DEBUG(logger, __FUNCTION__
		<< ": startAngleDeg = " << startAngleDeg
		<< ", rotMatrixStartAngle = \n" << rotMatrixStartAngle);

	numSegmentsArc = 
		static_cast<uint32_t>( vertexArrayStruct->numSegments * arcRangeDeg / 360.0);

	LOG4CXX_DEBUG(logger, __FUNCTION__
		<< ": arcRangeDeg = " << arcRangeDeg
		<< ", numSegmentsArc = " << numSegmentsArc
		<< " of " << vertexArrayStruct->numSegments
		<< " for a full circle.");
		
	

}
} /* namespace OevGLES */
