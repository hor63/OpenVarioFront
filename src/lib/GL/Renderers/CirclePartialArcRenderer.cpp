/*
 * CirclePartialArcRenderer.cpp
 *
 *  Created on: Jun 4, 2025
 *      Author: hor
 */
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <GLES2/gl2.h>
#include <cmath>

#include "OVFCommon.h"

#include <GLES/GLFramework.h>
#include "CirclePartialArcRenderer.h"
#include "CircleBaseRenderer.h"
#include "VecMat.h"


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
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": vertexBufferHandleArcEnd = " << vertexBufferHandleArcEnd
		<< ", vertexArrayHandleArcEnd  = " << vertexArrayHandleArcEnd
		);

	if (vertexBufferHandleArcEnd != 0U) {
		glDeleteBuffers(1, &vertexBufferHandleArcEnd);
		vertexBufferHandleArcEnd = 0U;
	}

	if (vertexArrayHandleArcEnd != 0U) {
		GLFramework::glDeleteVertexArraysOES(1,&vertexArrayHandleArcEnd);
		vertexArrayHandleArcEnd = 0U;
	}


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

template <int32_t Numerator,int32_t Denominator>
Angle<Numerator,Denominator> CirclePartialArcRenderer::normalizeAngle(
		Angle<Numerator,Denominator> angle) {	
	// Use the fmod function only when absolutely necessary,
	// i.e. the |angle| is >= 720deg (circle 2 times)
	if (angle >= (Angle<Numerator,Denominator>::fullCircle() * 2.0f) || 
		(angle <= (Angle<Numerator,Denominator>::fullCircle() * 2.0f))) {
		angle = Angle<Numerator,Denominator>::makeAngle(std::fmod(angle.getAngleValue(),
			Angle<Numerator,Denominator>::fullCircle().getAngleValue()));
	}
	
	// The much more likely scenario in angle conversions is that the angle to normalize
	// crosses 360 deg limit only once
	while (angle >= Angle<Numerator,Denominator>::fullCircle()) {
		angle = angle - Angle<Numerator,Denominator>::fullCircle();
	}
	
	while (angle <= -Angle<Numerator,Denominator>::fullCircle()) {
		angle = angle + Angle<Numerator,Denominator>::fullCircle();
	}
	
	if (angle < Angle<Numerator,Denominator>::makeAngle(0.0f)){
		angle = Angle<Numerator,Denominator>::fullCircle() + angle;
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
				<< ": startAngleNormalized = " << startAngleNormalized
				<< ", rotMatrixStartAngle = \n" << rotMatrixStartAngle);
		
			numSegmentsArc = 
				static_cast<uint32_t>( vertexArrayStruct->numSegments * 
					(arcRangeNormalized / AngleRad::fullCircle()));
					
			numVertexesArc = numSegmentsArc * 2 + 2;
		
			LOG4CXX_DEBUG(logger, 
				"\tarcRangeNormalized = " << arcRangeNormalized
				<< ", numSegmentsArc = " << numSegmentsArc
				<< " of " << vertexArrayStruct->numSegments
				<< " for a full circle.");
				
	
			LOG4CXX_DEBUG (logger,"\tstartAngleNormalized:" << startAngleNormalized
				<< " + arcRangeNormalized:" << arcRangeNormalized
				<< " - vertexArrayStruct->angleIncrement:" << vertexArrayStruct->angleIncrement);
			
	
			auto indexMaxVertexArrayEndSegmentStart = 
				numSegmentsArc * vertexArrayStruct->vertexStrideInMaxVertexArrayPerSegment + 2;
	
			LOG4CXX_DEBUG (logger,"\tindexMaxVertexArrayEndSegmentStart:" 
				<< indexMaxVertexArrayEndSegmentStart
				<< ", vertexStrideInMaxVertexArrayPerSegment = "
				<< vertexArrayStruct->vertexStrideInMaxVertexArrayPerSegment);

			// Copy the data from the template arry into the end segment, and calculate
			// the end points of the end segment yourself.
			// The vertexes are the end of the coarse polygon. This way I achieve a
			// seamless connection from the coarse polygon to the end segment.
			endArcVertexes[0] = endArcVertexes[2] = 
				circlePolygonVertexContainer.getMaxSegmentVertexArray()[
					indexMaxVertexArrayEndSegmentStart
				];
			endArcVertexes[1] = endArcVertexes[3] = 
				circlePolygonVertexContainer.getMaxSegmentVertexArray()[
					indexMaxVertexArrayEndSegmentStart + 1
				];

			// x
			endArcVertexes[2].position[0] =
				endArcVertexes[3].position[0] =
				endArcVertexes[2].normal[0] =
				endArcVertexes[3].normal[0] =
						cosf (arcRangeNormalized);

			// y
			endArcVertexes[2].position[1] =
				endArcVertexes[3].position[1] =
				endArcVertexes[2].normal[1] =
				endArcVertexes[3].normal[1] =
						sinf (arcRangeNormalized);

			if (vertexBufferHandleArcEnd == 0U) {
				glGenBuffers(1, &vertexBufferHandleArcEnd);
			}
			
			glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandleArcEnd);
			glBufferData(GL_ARRAY_BUFFER,
				sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct)
					* endArcVertexes.max_size(),
				endArcVertexes.data(),
				GL_DYNAMIC_DRAW);
				
			if (GLFramework::isVertexArrayUsable() && vertexArrayHandleArcEnd == 0U) {
				GLFramework::glGenVertexArraysOES(1,&vertexArrayHandleArcEnd);
				GLFramework::glBindVertexArrayOES(vertexArrayHandleArcEnd);

				// Set up the attributes
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
				
				GLFramework::glBindVertexArrayOES(0U);

			}
				
			glBindBuffer(GL_ARRAY_BUFFER,0);
		} // if (!isFullCircle) {
	} // if (dirtyArcAngles) {
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
	
		glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,
			GL_FALSE,&(MVMatrixStartAngle(0,0)));
		glUniformMatrix4fv(glProgram->getMvpMatrixLocation(),1,
			GL_FALSE,&(MVPMatrixStartAngle(0,0)));
	
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
	
		// draw the end array
		if (vertexArrayHandleArcEnd != 0U) {
				GLFramework::glBindVertexArrayOES(vertexArrayHandleArcEnd);
		} else {
			glBindBuffer(GL_ARRAY_BUFFER,vertexBufferHandleArcEnd);
			
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
		}

		glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);
		
		if (vertexArrayHandleArcEnd != 0U) {
			GLFramework::glBindVertexArrayOES(0U);
		} else {
			glDisableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
			glDisableVertexAttribArray(glProgram->getVertexNormalLocation());
			glDisableVertexAttribArray(glProgram->getVertexPosLocation());
		
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
		
		glUseProgram(0);
	} // else { // if (isFullCircle) {

}

void CirclePartialArcRenderer::normalizeAngles () {

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": arcRangeDeg = " << arcRange
		<< ", startAngleDeg = " << startAngle
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
