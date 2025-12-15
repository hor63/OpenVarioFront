/*
 * CirclePolygonVertexContainer.cpp
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 */
#include "GLES/GLFramework.h"
#include "GLES/GLObjectWrappers.h"
#include <complex>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <GLES2/gl2.h>

#include <cmath>

#include "OVFCommon.h"

#include "CirclePolygonVertexContainer.h"
#include "GLPrograms/GLProgDiffLightCircle.h"
#include "VecMat.h"

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CirclePolygonVertexContainer::CircleVertexArrayStruct::CircleVertexArrayStruct(
	std::size_t numSegments) :
	numSegments{numSegments},
	numVertexes{static_cast<GLsizei>(numSegments * 2U + 4U)},
	vertexStrideInMaxVertexArrayPerSegment{maxNumSegments / numSegments * 2U},
	vertexBufferHandle{false} {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger(
			"OpenVarioFront.Renderers.CirclePolygonVertexContainer");
}
#endif

	angleIncrement = AngleRad::fullCircle() / static_cast<GLfloat>(numSegments);

	maxRadius = static_cast<GLfloat>(maxDeviationPixels /
									 (1.0 - cosf(angleIncrement / 2.0)));

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
							  << ": numSegments = " << numSegments
							  << ", numVertexes = " << numVertexes
							  << ", vertexStrideInMaxVertexArrayPerSegment = "
							  << vertexStrideInMaxVertexArrayPerSegment
							  << ", angleIncrement = " << angleIncrement
							  << ", maxRadius = " << maxRadius);
}

/*
CirclePolygonVertexContainer::CircleVertexArrayStruct::~CircleVertexArrayStruct()
{

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": vertexBufferHandle = " << vertexBufferHandle
		<< ", vertexArrayHandle" << vertexArrayHandle
		);

	if (vertexBufferHandle != 0U) {
		glDeleteBuffers(1,&vertexBufferHandle);
		vertexBufferHandle = 0;
	}

	if (vertexArrayHandle != 0U) {
		if (auto contextSharedPointer = context.lock()){
				contextSharedPointer->glDeleteVertexArraysOES(1,&vertexArrayHandle);
		} // if (auto contextSharedPointer = context.lock())
		vertexArrayHandle = 0U;
	}
}

CirclePolygonVertexContainer::CircleVertexArrayStruct&
	CirclePolygonVertexContainer::CircleVertexArrayStruct::operator = (
		CirclePolygonVertexContainer::CircleVertexArrayStruct&& source)
{
	numVertexes = source.numVertexes;
	// Free your own context
	context = RenderContextWeakPtr{};
	// ... and exchange it with the one of source.
	std::swap(context,source.context);
	vertexStrideInMaxVertexArrayPerSegment =
source.vertexStrideInMaxVertexArrayPerSegment; maxRadius   = source.maxRadius;
	if (vertexBufferHandle != 0U) {
		glDeleteBuffers(1,&vertexBufferHandle);
	}
	vertexBufferHandle = source.vertexBufferHandle;
	source.vertexBufferHandle = 0;
	if (vertexArrayHandle != 0U) {
		if (auto contextSharedPointer = context.lock()){
			contextSharedPointer->glDeleteVertexArraysOES(1,&vertexArrayHandle);
		} // if (auto contextSharedPointer = context.lock()){
	}
	vertexArrayHandle = source.vertexArrayHandle;
	source.vertexArrayHandle = 0;

	return *this;
}

CirclePolygonVertexContainer::CircleVertexArrayStruct&
	CirclePolygonVertexContainer::CircleVertexArrayStruct::operator = (
		CirclePolygonVertexContainer::CircleVertexArrayStruct const& source)
{
	numVertexes = source.numVertexes;
	vertexStrideInMaxVertexArrayPerSegment =
source.vertexStrideInMaxVertexArrayPerSegment; maxRadius   = source.maxRadius;
	context = source.context;
	if (vertexBufferHandle != 0U) {
		glDeleteBuffers(1,&vertexBufferHandle);
		vertexBufferHandle = 0;
	}
	if (vertexArrayHandle != 0U) {
		if (auto contextSharedPointer = context.lock()){
			contextSharedPointer->glDeleteVertexArraysOES(1,&vertexArrayHandle);
		} // if (auto contextSharedPointer = context.lock()){
		vertexArrayHandle = 0U;
	}

	return *this;
}
*/

CirclePolygonVertexContainer::CirclePolygonVertexContainer() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger(
			"OpenVarioFront.Renderers.CirclePolygonVertexContainer");
	}
#endif

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__);

	// Fill vertex data for the template vertex buffer on the client side.

	// First element at index is the circle center for classes which draw
	// full circles, and need a center point
	// Both vertexes of the center element are marked as secondary.
	maxSegmentVertexArray[0].isSecondaryCircle = 1.0f;
	maxSegmentVertexArray[1].isSecondaryCircle = 1.0f;

	// x and y remain zero. This is the center of the circle.
	// z position is one; can be adjusted by the z-factor
	// of the internal model matrix
	maxSegmentVertexArray[0].position[2] = 1.0f;
	maxSegmentVertexArray[1].position[2] = 1.0f;

	// The actual circle data start at segment index 1 (i.e. vertex index 2).
	// Therefore start start the loop at index 1, but the angle still at 0
	// The direction of the vertexes is counter-clock wise as usual in math.
	for (int i = 1; i <= maxNumSegments + 1; ++i) {
		double angle = static_cast<double>(i - 1) *
					   (M_PI * 2.0 / static_cast<double>(maxNumSegments));

		// x
		maxSegmentVertexArray[i * 2].position[0] =
			maxSegmentVertexArray[i * 2 + 1].position[0] =
				maxSegmentVertexArray[i * 2].normal[0] =
					maxSegmentVertexArray[i * 2 + 1].normal[0] =
						std::cos(angle);

		// y
		maxSegmentVertexArray[i * 2].position[1] =
			maxSegmentVertexArray[i * 2 + 1].position[1] =
				maxSegmentVertexArray[i * 2].normal[1] =
					maxSegmentVertexArray[i * 2 + 1].normal[1] =
						std::sin(angle);

		// z
		// The secondary circle has the Z-offset
		maxSegmentVertexArray[i * 2].position[2] = 1.0f;

		// Alternate circle is first. Assumption is that the alternate circle
		// is the inner (smaller) circle, and/or is the circle in positive
		// z-direction.
		// The direction of the triangle strip is counter-clock wise.
		// Thus the drawing direction of the triangles is counter-clock
		// wise, and in direction of the normal vector.
		maxSegmentVertexArray[i * 2].isSecondaryCircle = 1.0f;

		LOG4CXX_TRACE(logger,
					  "\t angle = "
						  << (angle * 180.0 / M_PI) << "deg. Array[" << i * 2
						  << "].position = "
						  << maxSegmentVertexArray[i * 2].position[0] << ","
						  << maxSegmentVertexArray[i * 2].position[1] << ","
						  << maxSegmentVertexArray[i * 2].position[2] << ","
						  << maxSegmentVertexArray[i * 2].position[3] << ","
						  << "; normal = "
						  << maxSegmentVertexArray[i * 2].normal[0] << ","
						  << maxSegmentVertexArray[i * 2].normal[1] << ","
						  << maxSegmentVertexArray[i * 2].normal[2] << ","
						  << maxSegmentVertexArray[i * 2].normal[3] << ",");

		LOG4CXX_TRACE(logger,
					  "\t angle = "
						  << (angle * 180.0 / M_PI) << "deg. Array["
						  << i * 2 + 1 << "].position = "
						  << maxSegmentVertexArray[i * 2 + 1].position[0] << ","
						  << maxSegmentVertexArray[i * 2 + 1].position[1] << ","
						  << maxSegmentVertexArray[i * 2 + 1].position[2] << ","
						  << maxSegmentVertexArray[i * 2 + 1].position[3] << ","
						  << "; normal = "
						  << maxSegmentVertexArray[i * 2 + 1].normal[0] << ","
						  << maxSegmentVertexArray[i * 2 + 1].normal[1] << ","
						  << maxSegmentVertexArray[i * 2 + 1].normal[2] << ","
						  << maxSegmentVertexArray[i * 2 + 1].normal[3] << ",");

		// z and w are already defined by the constructor of
		// CirclePolygonVertexStruct.
	}

	// Fill the map of vertex buffers according to max circle size.
	// The smallest circle is actual a quadrant, i.e. 4 corners.

	LOG4CXX_DEBUG(logger,
				  __PRETTY_FUNCTION__
					  << ": Fill map of vertex buffers for circle sizes");

	for (uint32_t numSegments = 4; numSegments <= maxNumSegments;
		 numSegments *= 2) {
		CircleVertexArrayStruct vertexArryHolder{numSegments};

		LOG4CXX_DEBUG(
			logger,
			"\tInsert vertexArryHolder, numSegments = "
				<< numSegments << ", vertexArryHolder.numVertexes = "
				<< vertexArryHolder.numVertexes
				<< ", vertexArryHolder.angleIncrement = "
				<< vertexArryHolder.angleIncrement
				<< ", vertexArryHolder.vertexStrideInMaxVertexArrayPerSegment "
				   "= "
				<< vertexArryHolder.vertexStrideInMaxVertexArrayPerSegment
				<< ", vertexArryHolder.maxRadius = "
				<< vertexArryHolder.maxRadius);

		circleVertexArrayMap.insert(
				CircleVertexArrayMapType::value_type(vertexArryHolder.maxRadius,std::move(vertexArryHolder)));
	}
}

CirclePolygonVertexContainer::~CirclePolygonVertexContainer() {}

const CirclePolygonVertexContainer::CircleVertexArrayStruct &
CirclePolygonVertexContainer::createVertexArrayStruct(
	RenderContextSharedPtr &context, GLfloat radius) {

	auto rc = circleVertexArrayMap.lower_bound(radius);

	if (rc == circleVertexArrayMap.end()) {
		// The requested radius is greater than any which I provide.
		// Use the largest buffer which I can provide.
		// The number of segments is limited to maxNumSegments.
		--rc;
	}

	if (!rc->second.vertexBufferHandle.valid()) {
		createVertexBuffer(context, rc->second);
	}

	return rc->second;
}

void CirclePolygonVertexContainer::createVertexBuffer(
	RenderContextSharedPtr const &context,
	CircleVertexArrayStruct &vertArrayStruct) {

	CirclePolygonVertexStruct *clientBuffer;
	std::vector<CirclePolygonVertexStruct> tempBuffer;

	if (vertArrayStruct.numSegments == maxNumSegments) {
		clientBuffer = &maxSegmentVertexArray[0];

		LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
								  << ": numSegments  == " << maxNumSegments
								  << ". Use maxSegmentVertexArray at "
								  << reinterpret_cast<void *>(clientBuffer));

	} else {

		auto incrementSource = maxNumSegments * 2 / vertArrayStruct.numSegments;
		int sourceVertexIndex = 2;

		LOG4CXX_DEBUG(logger,
					  __PRETTY_FUNCTION__
						  << ": numSegments  == " << vertArrayStruct.numSegments
						  << ", numVertexes = " << vertArrayStruct.numVertexes
						  << ". Use a temporary buffer. IncrementSource = "
						  << incrementSource);

		tempBuffer.reserve(vertArrayStruct.numVertexes);

		for (int targetVertexIndex = 2;
			 targetVertexIndex < vertArrayStruct.numVertexes;
			 targetVertexIndex += 2) {

			tempBuffer[targetVertexIndex] =
				maxSegmentVertexArray[sourceVertexIndex];
			tempBuffer[targetVertexIndex + 1] =
				maxSegmentVertexArray[sourceVertexIndex + 1];

			LOG4CXX_DEBUG(logger,
						  "\ttargetVertexIndex = " << targetVertexIndex
												   << ", sourceVertexIndex = "
												   << sourceVertexIndex);

			sourceVertexIndex += incrementSource;
		}

		// Copy the first element manually. This is the circle center.
		tempBuffer[0] = maxSegmentVertexArray[0];
		tempBuffer[1] = maxSegmentVertexArray[1];

		clientBuffer = &tempBuffer[0];

	} // if (vertArrayStruct.numSegments == maxNumSegments) {

	// remember the context for use in the destructor
	vertArrayStruct.context = context;

	if (!vertArrayStruct.vertexBufferHandle.valid()) {
		vertArrayStruct.vertexBufferHandle = GLBufferObject(true);
	}
	GlBindArrayBufferObject bindBufferObject(
		vertArrayStruct.vertexBufferHandle);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(CirclePolygonVertexStruct) *
					 vertArrayStruct.numVertexes,
				 clientBuffer, GL_STATIC_DRAW);
	if (context->vertexArrayIsUsable &&
		!vertArrayStruct.vertexArrayHandle.valid()) {
		auto glProgram = GLProgDiffLightCircle::getProgram();

		vertArrayStruct.vertexArrayHandle = GLVertexArrayObject(context);
		GLBindVertexArrayObject bindVertexArray(
			vertArrayStruct.vertexArrayHandle);

		glEnableVertexAttribArray(glProgram->getVertexPosLocation());
		glVertexAttribPointer(
			glProgram->getVertexPosLocation(),
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
					   position) /
				sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
						   position[0]),
			GL_FLOAT, GL_FALSE,
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
			reinterpret_cast<void *>(offsetof(
				CirclePolygonVertexContainer::CirclePolygonVertexStruct,
				position)));

		glEnableVertexAttribArray(glProgram->getVertexNormalLocation());
		glVertexAttribPointer(
			glProgram->getVertexNormalLocation(),
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
					   normal) /
				sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
						   normal[0]),
			GL_FLOAT, GL_FALSE,
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
			reinterpret_cast<void *>(offsetof(
				CirclePolygonVertexContainer::CirclePolygonVertexStruct,
				normal)));

		glEnableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
		glVertexAttribPointer(
			glProgram->getIsSecondaryVertexLocation(), 1, GL_FLOAT, GL_FALSE,
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct),
			reinterpret_cast<void *>(offsetof(
				CirclePolygonVertexContainer::CirclePolygonVertexStruct,
				isSecondaryCircle)));
	}
}

} /* namespace OevGLES */
