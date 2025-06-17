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
	if (angle >= 360.0_deg || (angle <= (-360.0_deg))) {
		angle = AngleDeg::makeAngle(std::fmod(angle.getAngleValue(), 360.0f));
	}
	
	if (angle < 0.0_deg) {
		angle = 360.0_deg + angle;
	}
	
	return angle;
}

void CirclePartialArcRenderer::setupVertexBuffers() {
	
	// Call the base class. It does the heavy lifting
	CircleBaseRenderer::setupVertexBuffers();
	
	if (dirtyArcAngles) {
		normalizeAngles();
		dirtyArcAngles = false;
		
		if (!isFullCircle) {

		rotMatrixStartAngle = rotationMatrixZ (startAngleNormalized);
		LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
			<< "startAngleNormalized = " 
			<< static_cast<AngleDeg>(startAngleNormalized).getAngleValue()
			<< ", rotMatrixStartAngle = \n" << rotMatrixStartAngle);
	
		numSegmentsArc = 
			static_cast<uint32_t>( vertexArrayStruct->numSegments * 
				(arcRangeNormalized / AngleRad::fullCircle()));
				
		numVertexesArc = numSegmentsArc * 2 + 2;
	
		LOG4CXX_DEBUG(logger, 
			"\tarcRangeNormalized = "
			<< static_cast<AngleDeg>(arcRangeNormalized).getAngleValue()
			<< ", numSegmentsArc = " << numSegmentsArc
			<< " of " << vertexArrayStruct->numSegments
			<< " for a full circle.");
			
		

		}
	}
}

void CirclePartialArcRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
	
	setupVertexBuffers();
	
	if (isFullCircle) {
		CircleBaseRenderer::draw(modelMatrix,viewMatrix,ProjMatrix,
			MVMatrix,MVPMatrix,lightDir,lightColor,ambientLightColor);
	} else { // if (isFullCircle) {
		Mat4 MVMatrixStartAngle = MVMatrix * rotMatrixStartAngle;
		Mat4 MVPMatrixStartAngle = MVPMatrix * rotMatrixStartAngle;
		// First activate the program
		glProgram->useProgram();
	
		// Set up the uniforms
		glUniform4fv(glProgram->getVecFactorPrimaryVertexLocation(),1,vecFactorPrimaryVertex);
		glUniform4fv(glProgram->getVecFactorSecondVertexLocation(),1,vecFactorSecondVertex);
		glUniform4fv(glProgram->getVecFactorNormalVectorLocation(),1,vecFactorNormalVector);
	
		glUniformMatrix4fv(glProgram->getMvpMatrixLocation(),1,
			GL_FALSE,&(MVPMatrixStartAngle(0,0)));
		glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,
			GL_FALSE,&(MVMatrixStartAngle(0,0)));
	
		glUniform3fv(glProgram->getLightDirLocation(),1,&(lightDir(0)));
		glUniform4fv(glProgram->getLightColorLocation(),1,&(lightColor(0)));
		glUniform4fv(glProgram->getAmbientLightColorLocation(),1,&(ambientLightColor(0)));
	
		// Set up the attributes
		glBindBuffer(GL_ARRAY_BUFFER,vertexArrayStruct->vertexBufferHandle);
	
		glEnableVertexAttribArray(glProgram->getVertexPosLocation());
		glVertexAttribPointer(glProgram->getVertexPosLocation(),
				sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::position) /
					sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::position[0]),
				GL_FLOAT,
				GL_FALSE,sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
				reinterpret_cast<void*>(offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,position)));
	
		glEnableVertexAttribArray(glProgram->getVertexNormalLocation());
		glVertexAttribPointer(glProgram->getVertexNormalLocation(),
				sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::normal) /
					sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::normal[0]),
				GL_FLOAT,
				GL_FALSE,sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
				reinterpret_cast<void*>(offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,normal)));
	
		glEnableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
		glVertexAttribPointer(glProgram->getIsSecondaryVertexLocation(),
				1,
				GL_FLOAT,
				GL_FALSE,sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
				reinterpret_cast<void*>(offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,isSecondaryCircle)));
	
		glDisableVertexAttribArray(glProgram->getVertexColorLocation());
		glVertexAttrib4fv(glProgram->getVertexColorLocation(),&bodyColor(0));
	
		std::unique_ptr<BlendAttributeSetRestoreStd> blendAttrs;
	
		// Draw in transparent mode when the Alpha value is not totally opaque.
		if (bodyColor(3) < 1.0f) {
			blendAttrs = std::unique_ptr<BlendAttributeSetRestoreStd>(new BlendAttributeSetRestoreStd);
		}
	
		// I am omitting the circle center at the start of the vertex array.
		// Therefore I am starting at position 2, and the number of vertexes
		// is 2 less that the number of vertexes in the buffer.
		glDrawArrays( GL_TRIANGLE_STRIP, 2, numVertexesArc);
	
		glDisableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
		glDisableVertexAttribArray(glProgram->getVertexNormalLocation());
		glDisableVertexAttribArray(glProgram->getVertexPosLocation());
	
		glBindBuffer(GL_ARRAY_BUFFER,0);
		glUseProgram(0);
	} // else { // if (isFullCircle) {

}

void CirclePartialArcRenderer::normalizeAngles () {

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": arcRangeDeg = " << arcRange.getAngleValue()
		<< ", startAngleDeg = " << startAngle.getAngleValue()
		);

	if (arcRange >=360.0_deg || arcRange <= (-360.0_deg)) {
		// Other considerations are moot since now
		// the full circle draw method of the base classs is being called.
		isFullCircle = true;
		
		LOG4CXX_DEBUG(logger, "\tPaint a full circle");

	} else {
		isFullCircle = false;
		
		if (arcRange < 0.0_deg) {
			LOG4CXX_DEBUG(logger,"\tarcRange is < 0.0");
			// let the arc start at the end but draw the arc now counter-clock wise.
			startAngleNormalized = normalizeAngle(startAngle + (AngleDeg::fullCircle() + arcRange));
			arcRangeNormalized = -arcRange;
		} else {
			startAngleNormalized = normalizeAngle(startAngle);
			arcRangeNormalized = arcRange;
		}

	}


}
} /* namespace OevGLES */
