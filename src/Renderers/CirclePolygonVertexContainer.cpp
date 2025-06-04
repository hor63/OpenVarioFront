/*
 * CirclePolygonVertexContainer.cpp
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cmath>

#include "OVFCommon.h"

#include "CirclePolygonVertexContainer.h"
#include "GLES/VecMat.h"


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CirclePolygonVertexContainer::CircleVertexArrayStruct::CircleVertexArrayStruct(std::size_t numSegments)
	:numSegments{numSegments},
	 numVertexes{static_cast<GLsizei>(numSegments * 2U + 4U)},
	 angleIncrementRad{2.0*M_PI/static_cast<double>(numSegments)},
	 vertexBufferHandle{0U}

{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CirclePolygonVertexContainer");
	}
#endif

	maxRadius  = static_cast<GLfloat>(maxDeviationPixels / (1.0 - cos (angleIncrementRad / 2.0)));

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
			<< ": numSegments = " << numSegments
			<< ", numVertexes = " << numVertexes
			<< ", angleIncrementRad = " << angleIncrementRad
			<< " = " << angleIncrementRad*radToDeg << "deg."
			<< ", maxRadius = " << maxRadius);
}


CirclePolygonVertexContainer::CirclePolygonVertexContainer() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CirclePolygonVertexContainer");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);

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
	for (int i = 1; i <= maxNumSegments + 1;++i) {
		double angle = static_cast<double>(i - 1) *
				(M_PI  * 2.0 / static_cast<double>(maxNumSegments));

		// x
		maxSegmentVertexArray[i*2].position[0] =
				maxSegmentVertexArray[i*2 + 1].position[0] =
				maxSegmentVertexArray[i*2].normal[0] =
				maxSegmentVertexArray[i*2 + 1].normal[0] =
						std::cos (angle);

		// y
		maxSegmentVertexArray[i*2].position[1] =
				maxSegmentVertexArray[i*2 + 1].position[1] =
				maxSegmentVertexArray[i*2].normal[1] =
				maxSegmentVertexArray[i*2 + 1].normal[1] =
						std::sin (angle);

		// z
		// The secondary circle has the Z-offset
		maxSegmentVertexArray[i*2].position[2] = 1.0f;

		// Alternate circle is first. Assumption is that the alternate circle
		// is the inner (smaller) circle, and/or is the circle in positive
		// z-direction.
		// The direction of the triangle strip is counter-clock wise.
		// Thus the drawing direction of the triangles is counter-clock
		// wise, and in direction of the normal vector.
		maxSegmentVertexArray[i*2].isSecondaryCircle = 1.0f;


		LOG4CXX_DEBUG(logger,"\t angle = " << (angle * 180.0 / M_PI)
				<< "deg. Array["<< i*2 << "].position = "
				<< maxSegmentVertexArray[i*2].position[0] << ","
				<< maxSegmentVertexArray[i*2].position[1] << ","
				<< maxSegmentVertexArray[i*2].position[2] << ","
				<< maxSegmentVertexArray[i*2].position[3] << ","
				<< "; normal = "
				<< maxSegmentVertexArray[i*2].normal[0] << ","
				<< maxSegmentVertexArray[i*2].normal[1] << ","
				<< maxSegmentVertexArray[i*2].normal[2] << ","
				<< maxSegmentVertexArray[i*2].normal[3] << ","
				);

		LOG4CXX_DEBUG(logger,"\t angle = " << (angle * 180.0 / M_PI)
				<< "deg. Array["<< i*2+1 << "].position = "
				<< maxSegmentVertexArray[i*2+1].position[0] << ","
				<< maxSegmentVertexArray[i*2+1].position[1] << ","
				<< maxSegmentVertexArray[i*2+1].position[2] << ","
				<< maxSegmentVertexArray[i*2+1].position[3] << ","
				<< "; normal = "
				<< maxSegmentVertexArray[i*2+1].normal[0] << ","
				<< maxSegmentVertexArray[i*2+1].normal[1] << ","
				<< maxSegmentVertexArray[i*2+1].normal[2] << ","
				<< maxSegmentVertexArray[i*2+1].normal[3] << ","
				);


		// z and w are already defined by the constructor of CirclePolygonVertexStruct.

	}

	// Fill the map of vertex buffers according to max circle size.
	// The smallest circle is actual a quadrant, i.e. 4 corners.

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
			<< ": Fill map of vertex buffers for circle sizes");

	for (uint32_t numSegments = 4; numSegments <= maxNumSegments; numSegments*=2) {
		CircleVertexArrayStruct vertexArryHolder {numSegments};

		circleVertexArrayMap.insert(
				std::pair(vertexArryHolder.maxRadius,vertexArryHolder));
	}

}

CirclePolygonVertexContainer::~CirclePolygonVertexContainer() {}

const CirclePolygonVertexContainer::CircleVertexArrayStruct& CirclePolygonVertexContainer::createVertexArrayStruct(
		GLfloat radius) {

	auto rc = circleVertexArrayMap.lower_bound(radius);

	if (rc == circleVertexArrayMap.end()) {
		// The requested radius is greater than any which I provide.
		// Use the largest buffer which I can provide.
		// The number of segments is limited to \ref maxNumSegments.
		--rc;
	}

	if (rc->second.vertexBufferHandle == 0) {
		createVertexBuffer (rc->second);
	}

	return rc->second;
}

void CirclePolygonVertexContainer::createVertexBuffer(
		CircleVertexArrayStruct &vertArrayStruct) {

	CirclePolygonVertexStruct* clientBuffer;
	std::vector<CirclePolygonVertexStruct> tempBuffer;

	if (vertArrayStruct.numSegments == maxNumSegments) {
		clientBuffer = &maxSegmentVertexArray[0];

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
				<< ": numSegments  == " << maxNumSegments
				<< ". Use maxSegmentVertexArray at " <<
				reinterpret_cast<void*>(clientBuffer));

	} else {

		auto incrementSource = maxNumSegments * 2 / vertArrayStruct.numSegments;
		int sourceVertexIndex = 2;

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
				<< ": numSegments  == " << vertArrayStruct.numSegments
				<< ", numVertexes = " << vertArrayStruct.numVertexes
				<< ". Use a temporary buffer. IncrementSource = "
				<< incrementSource);

		tempBuffer.reserve(vertArrayStruct.numVertexes);

		for (int targetVertexIndex = 2 ;
				targetVertexIndex < vertArrayStruct.numVertexes ;
				targetVertexIndex += 2) {

			tempBuffer[targetVertexIndex]     = maxSegmentVertexArray[sourceVertexIndex];
			tempBuffer[targetVertexIndex + 1] = maxSegmentVertexArray[sourceVertexIndex + 1];

			LOG4CXX_DEBUG(logger,
					"\ttargetVertexIndex = " << targetVertexIndex
					<< ", sourceVertexIndex = " << sourceVertexIndex);

			sourceVertexIndex += incrementSource;
		}

		// Copy the first element manually. This is the circle center.
		tempBuffer[0] = maxSegmentVertexArray[0];
		tempBuffer[1] = maxSegmentVertexArray[1];


		clientBuffer = &tempBuffer[0];

		glGenBuffers(1, &vertArrayStruct.vertexBufferHandle);
		glBindBuffer(GL_ARRAY_BUFFER,vertArrayStruct.vertexBufferHandle);
		glBufferData(GL_ARRAY_BUFFER,
				sizeof(CirclePolygonVertexStruct)*vertArrayStruct.numVertexes,
				clientBuffer,GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER,0);

	}

	/// todo: create the vertex buffer and fill it with clientBuffer data.

}

} /* namespace OevGLES */
