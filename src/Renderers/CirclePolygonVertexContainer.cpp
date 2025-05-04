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

#include "CirclePolygonVertexContainer.h"


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CirclePolygonVertexContainer::CirclePolygonVertexContainer() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLES.CirclePolygonVertexContainer");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);

	// Fill vertex data for the template vertex buffer on the client side.

	for (int i = 0; i <= maxNumSegments;++i) {
		double angle = static_cast<double>(i) *
				(M_PI  * 2.0 / static_cast<double>(maxNumSegments));

		maxSegmentVertexArray[i*2 + 1].isSecondaryCircle = 1.0f;

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

		LOG4CXX_DEBUG(logger,"\tnumSegments = " << numSegments
				<< ", maxRadius = " << vertexArryHolder.maxRadius
				);

		circleVertexArrayMap.insert(
				std::pair(vertexArryHolder.maxRadius,vertexArryHolder));
	}

}

CirclePolygonVertexContainer::~CirclePolygonVertexContainer() {}

} /* namespace OevGLES */
