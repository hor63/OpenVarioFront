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
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CirclePolygonVertexContainer");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__);
	for (int i = 0; i < maxNumSegments;++i) {
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

	// Now the final two elements which close the circle.
	maxSegmentVertexArray[maxNumSegments*2 + 1].isSecondaryCircle = 1.0f;

	// x
	maxSegmentVertexArray[maxNumSegments*2].position[0] =
			maxSegmentVertexArray[maxNumSegments*2 + 1].position[0] =
			maxSegmentVertexArray[maxNumSegments*2].normal[0] =
			maxSegmentVertexArray[maxNumSegments*2 + 1].normal[0] =
					1.0;

	// y
	maxSegmentVertexArray[maxNumSegments*2].position[1] =
			maxSegmentVertexArray[maxNumSegments*2 + 1].position[1] =
			maxSegmentVertexArray[maxNumSegments*2].normal[1] =
			maxSegmentVertexArray[maxNumSegments*2 + 1].normal[1] =
					0.0;


	LOG4CXX_DEBUG(logger,"\t angle = " << 0
			<< "deg. Array["<< maxNumSegments*2 << "].position = "
			<< maxSegmentVertexArray[maxNumSegments*2].position[0] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].position[1] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].position[2] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].position[3] << ","
			<< "; normal = "
			<< maxSegmentVertexArray[maxNumSegments*2].normal[0] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].normal[1] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].normal[2] << ","
			<< maxSegmentVertexArray[maxNumSegments*2].normal[3] << ","
			);

	LOG4CXX_DEBUG(logger,"\t angle = " << 0
			<< "deg. Array["<< maxNumSegments*2+1 << "].position = "
			<< maxSegmentVertexArray[maxNumSegments*2+1].position[0] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].position[1] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].position[2] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].position[3] << ","
			<< "; normal = "
			<< maxSegmentVertexArray[maxNumSegments*2+1].normal[0] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].normal[1] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].normal[2] << ","
			<< maxSegmentVertexArray[maxNumSegments*2+1].normal[3] << ","
			);


}

CirclePolygonVertexContainer::~CirclePolygonVertexContainer() {}

} /* namespace OevGLES */
