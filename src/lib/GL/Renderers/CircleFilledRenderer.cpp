/*
 * CircleFilledRenderer.cpp
 *
 *  Created on: Jun 1, 2025
 *      Author: hor
 *
  *
 *	Sub-class of CircleBaseRenderer to render completely filled circles
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "CircleFilledRenderer.h"

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CircleFilledRenderer::CircleFilledRenderer(
		RenderContextSharedPtr const &context):
	CircleBaseRenderer {context}
	{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CircleFilledRenderer");
	}
#endif

}

CircleFilledRenderer::~CircleFilledRenderer() {
}

void CircleFilledRenderer::setupVertexBuffers() {

	if (dirty) {
		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
				<< ": Radius = " << primaryRadius
				<< ", z-offset center = " << primarySecondaryZOffset);

		// Used in CircleBaseRenderer::setupVertexBuffers().
		// For a full circle the inner radius is of course 0.
		secondaryRadius = 0;

		// I am re-using most of the stuff of the base class.
		CircleBaseRenderer::setupVertexBuffers();

		// ... with some modifications:

		// I can offset the center of the circle in z-direction.
		vecFactorPrimaryVertex [2] = primarySecondaryZOffset;

		dirty = false;

	} // if (dirty)
}

void CircleFilledRenderer::draw(RenderStandardUniforms const &stdUniformData) {
	if (dirty) {
		setupVertexBuffers();
	}

	// First activate the program
	glProgram->useProgram();

	// Set up the uniforms
	glUniform4fv(glProgram->getVecFactorPrimaryVertexLocation(), 1,
				 vecFactorPrimaryVertex);
	glUniform4fv(glProgram->getVecFactorSecondVertexLocation(), 1,
				 vecFactorSecondVertex);
	glUniform4fv(glProgram->getVecFactorNormalVectorLocation(), 1,
				 vecFactorNormalVector);

	glUniformMatrix4fv(glProgram->getMvpMatrixLocation(), 1, GL_FALSE,
					   &(stdUniformData.getMVPMatrixC()(0, 0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(), 1, GL_FALSE,
					   &(stdUniformData.getMVMatrixC()(0, 0)));

	glUniform3fv(glProgram->getLightDirLocation(), 1,
				 &(stdUniformData.getLightDirC()(0)));
	glUniform4fv(glProgram->getLightColorLocation(), 1,
				 &(stdUniformData.getLightColorC()(0)));
	glUniform4fv(glProgram->getAmbientLightColorLocation(), 1,
				 &(stdUniformData.getAmbientLightColorC()(0)));

	// Set up the attributes
	glBindBuffer(GL_ARRAY_BUFFER, vertexArrayStruct->vertexBufferHandle);

	glEnableVertexAttribArray(glProgram->getVertexPosLocation());
	glVertexAttribPointer(
		glProgram->getVertexPosLocation(),
		sizeof(
			CirclePolygonVertexContainer::CirclePolygonVertexStruct::position) /
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
					   position[0]),
		GL_FLOAT, GL_FALSE,
		sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct) *
			2, // Only every 2 vertexes
		reinterpret_cast<void *>(
			sizeof(CirclePolygonVertexContainer::
					   CirclePolygonVertexStruct) + // The primary element is
													// the second of the tuple.
			offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,
					 position)));

	glEnableVertexAttribArray(glProgram->getVertexNormalLocation());
	glVertexAttribPointer(
		glProgram->getVertexNormalLocation(),
		sizeof(
			CirclePolygonVertexContainer::CirclePolygonVertexStruct::normal) /
			sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct::
					   normal[0]),
		GL_FLOAT, GL_FALSE,
		sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct) *
			2, // Only every 2 vertexes
		reinterpret_cast<void *>(
			sizeof(CirclePolygonVertexContainer::
					   CirclePolygonVertexStruct) + // The primary element is
													// the second of the tuple.
			offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,
					 normal)));

	glEnableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
	glVertexAttribPointer(
		glProgram->getIsSecondaryVertexLocation(), 1, GL_FLOAT, GL_FALSE,
		sizeof(CirclePolygonVertexContainer::CirclePolygonVertexStruct) *
			2, // Only every 2 vertexes
		reinterpret_cast<void *>(
			sizeof(CirclePolygonVertexContainer::
					   CirclePolygonVertexStruct) + // The primary element is
													// the second of the tuple.
			offsetof(CirclePolygonVertexContainer::CirclePolygonVertexStruct,
					 isSecondaryCircle)));

	glDisableVertexAttribArray(glProgram->getVertexColorLocation());
	glVertexAttrib4fv(glProgram->getVertexColorLocation(), &bodyColor(0));

	std::unique_ptr<BlendAttributeSetRestoreStd> blendAttrs;

	// Draw in transparent mode when the Alpha value is not totally opaque.
	if (bodyColor(3) < 1.0f) {
		blendAttrs = std::unique_ptr<BlendAttributeSetRestoreStd>(
			new BlendAttributeSetRestoreStd);
	}

	// I am starting with vertex #0, i.e. the circle center.
	// But I am omitting every second vertex in the buffer.
	// Thus I am hitting the only the primary circle vertexes.
	glDrawArrays(GL_TRIANGLE_FAN, 0, (vertexArrayStruct->numVertexes / 2));

	glDisableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
	glDisableVertexAttribArray(glProgram->getVertexNormalLocation());
	glDisableVertexAttribArray(glProgram->getVertexPosLocation());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glUseProgram(0);
}

} /* namespace OevGLES */
