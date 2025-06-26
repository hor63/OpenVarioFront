/*
 * CircleBaseRenderer.cpp
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 *
 *	Base class for all things circular.
 *	It is based on triangle meshes forming different types of circular forms.
 *	Derived classes will render full or partial arcs, with an inner and outer diameter
 *	or tire-like forms
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

#include "CircleBaseRenderer.h"

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

CircleBaseRenderer::CircleBaseRenderer(
		CirclePolygonVertexContainer& circlePolygonVertexContainer)
	: circlePolygonVertexContainer {circlePolygonVertexContainer}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.CircleBaseRenderer");
	}
#endif

}

CircleBaseRenderer::~CircleBaseRenderer() {
	
}

void CircleBaseRenderer::setPrimaryRadius(double primaryRadius) {
	if (this->primaryRadius != primaryRadius) {
		this->primaryRadius = primaryRadius;

		dirty = true;
	}
}

void CircleBaseRenderer::setPrimarySecondaryZOffset(
		double primarySecondaryZOffset) {

	if (this->primarySecondaryZOffset != primarySecondaryZOffset) {
		this->primarySecondaryZOffset = primarySecondaryZOffset;

		dirty = true;
	}
}

void CircleBaseRenderer::setSecondaryRadius(double secondaryRadius) {
	if (this->secondaryRadius != secondaryRadius) {
		this->secondaryRadius = secondaryRadius;

		dirty = true;
	}
}

void CircleBaseRenderer::setupVertexBuffers() {

	if (glProgram == nullptr) {
		glProgram = GLProgDiffLightCircle::getProgram();
	}

	if (dirty) {

		vertexArrayStruct =
			&circlePolygonVertexContainer.createVertexArrayStruct(primaryRadius);

		vecFactorPrimaryVertex [0] = vecFactorPrimaryVertex [1] = primaryRadius;
		vecFactorSecondVertex [0] = vecFactorSecondVertex [1] = secondaryRadius;
		vecFactorSecondVertex [2] = primarySecondaryZOffset;
#if defined HAVE_LOG4CXX_H
		Eigen::Map<Vec4> vecFactorPrimaryVertexMap(vecFactorPrimaryVertex);
		Eigen::Map<Vec4> vecFactorSecondVertexMap(vecFactorSecondVertex);
#endif

		// The normal vector as the cross product of vectors {0,1,0}
		// (dummy to form a plane rotating around the y-axis)
		// and {x,0,z} degrades to {z,0,-x}
		vecFactorNormalVector [0] = vecFactorNormalVector [1] = primarySecondaryZOffset;
		vecFactorNormalVector [2] = primaryRadius - secondaryRadius;

		Eigen::Map<Vec3> vecFactorNormalVectorMap (vecFactorNormalVector);

		dirty = false;

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
				<< ": primaryRadius = " << primaryRadius
				<< ", secondaryRadius = " << secondaryRadius
				<< ", primarySecondaryZOffset = " << primarySecondaryZOffset
				<< "; use a polygon with " << vertexArrayStruct->numSegments
				<< " segments, " << vertexArrayStruct->numVertexes << " vertexes"
				<< "\nvecFactorPrimaryVertexMap = \n" << vecFactorPrimaryVertexMap
				<< "\nvecFactorSecondVertexMap = \n" << vecFactorSecondVertexMap
				<< "\nvecFactorNormalVectorMap = \n" << vecFactorNormalVectorMap);

		vecFactorNormalVectorMap.normalize();
		LOG4CXX_DEBUG(logger,"\tvecFactorNormalVectorMap normalized = \n"
				<< vecFactorNormalVectorMap
				);

	}
}

void CircleBaseRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {

	if (dirty) {
		setupVertexBuffers();
	}

	// First activate the program
	glProgram->useProgram();

	// Set up the uniforms
	glUniform4fv(glProgram->getVecFactorPrimaryVertexLocation(),1,vecFactorPrimaryVertex);
	glUniform4fv(glProgram->getVecFactorSecondVertexLocation(),1,vecFactorSecondVertex);
	glUniform4fv(glProgram->getVecFactorNormalVectorLocation(),1,vecFactorNormalVector);

	glUniformMatrix4fv(glProgram->getMvpMatrixLocation(),1,GL_FALSE,&(MVPMatrix(0,0)));
	glUniformMatrix4fv(glProgram->getMvMatrixLocation(),1,GL_FALSE,&(MVMatrix(0,0)));

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
	glDrawArrays( GL_TRIANGLE_STRIP, 2, vertexArrayStruct->numVertexes - 2);

	glDisableVertexAttribArray(glProgram->getIsSecondaryVertexLocation());
	glDisableVertexAttribArray(glProgram->getVertexNormalLocation());
	glDisableVertexAttribArray(glProgram->getVertexPosLocation());

	glBindBuffer(GL_ARRAY_BUFFER,0);
	glUseProgram(0);

}

} /* namespace OevGLES */
