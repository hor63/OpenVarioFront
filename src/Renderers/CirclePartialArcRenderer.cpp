/*
 * CirclePartialArcRenderer.cpp
 *
 *  Created on: Jun 4, 2025
 *      Author: hor
 */

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

}

CirclePartialArcRenderer::~CirclePartialArcRenderer() {

}

void CirclePartialArcRenderer::setArcRangeDeg(double arcRangeDeg) {
	if (this->arcRangeDeg != arcRangeDeg) {
		this->arcRangeDeg = arcRangeDeg;

		dirtyArcRange = true;
	}
}

void CirclePartialArcRenderer::setStartAngleDeg(double startAngleDeg) {
	if (this->startAngleDeg != startAngleDeg) {
		this->startAngleDeg = startAngleDeg;

		dirtyStartArc = true;
	}
}

void CirclePartialArcRenderer::setupVertexBuffers() {
	
	// Call the base class. It does the heavy lifting
	CircleBaseRenderer::setupVertexBuffers();
	
	if (dirtyStartArc) {
		if (startAngleDeg > 0.0) {
			rotMatrixStartAngle = rotationMatrixZ (startAngleDeg);
			LOG4CXX_DEBUG(logger, __FUNCTION__
				<< ": startAngleDeg = " << startAngleDeg
				<< ", rotMatrixStartAngle = \n" << rotMatrixStartAngle);
		}
		
		dirtyStartArc = false;
	}
	
	if (dirtyArcRange) {
		if (arcRangeDeg > 0.0) {
			
			numSegmentsArc = 
				static_cast<uint32_t>( vertexArrayStruct->numSegments * arcRangeDeg / 360.0);

			LOG4CXX_DEBUG(logger, __FUNCTION__
				<< ": arcRangeDeg = " << arcRangeDeg
				<< ", numSegmentsArc = " << numSegmentsArc
				<< " of " << vertexArrayStruct->numSegments
				<< " for a full circle.");
			
		}
		dirtyArcRange = false;
	}
	
}

void CirclePartialArcRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
	
	setupVertexBuffers();
}

} /* namespace OevGLES */
