/*
 * PlainFieldControl.cpp
 *
 *  Created on: Oct 22, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2018  Kai Horstmann
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

#include "PlainFieldControl.h"
#include "GLPrograms/GLProgControlSimpleFill.h"

#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr logger;
#endif

namespace OevControls {


PlainFieldControl::PlainFieldControl(ControlsContainerWeakPtr const &parent,
		RenderContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name) :
		ControlBase(parent,renderContextPtr,uuid,name)
{
	#if defined HAVE_LOG4CXX_H
			// Get the logger if necessary
			if (!logger) {
				logger = log4cxx::Logger::getLogger("OpenVarioFront.Controls.PlainFieldControl");
			}
	#endif

}

PlainFieldControl::~PlainFieldControl() { }

void PlainFieldControl::setupVertexBuffers () {}

void PlainFieldControl::draw () {

	if (posOrSizeDirty) {
		recalcSizePositionMatrix();
	}
	auto simpleFillProg = OevGLES::GLProgControlSimpleFill::getProgram();
	OevGLES::GlProgUse useSimpleFilleProg(*simpleFillProg);

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": MVP Matrix location" << simpleFillProg->getMvpMatrixLocation()
		<< ", MVP Matrix = \n" << renderUniforms.getMVPMatrixC());

	LOG4CXX_DEBUG(logger, 
		"\tmodel Matrix = \n" << renderUniforms.getModelMatrixC());
	
	// Set the uniforms
	glUniformMatrix4fv(simpleFillProg->getMvpMatrixLocation(), 1, GL_FALSE,
					   &(renderUniforms.getMVPMatrixC()(0, 0)));

	auto & foregroundColor = *renderContextPtr->foregroundColorPtr.get();

	LOG4CXX_DEBUG(logger, 
		"\t Fill color location = " << simpleFillProg->getFillColorLocation()
		<< ", forgroundColor = " << foregroundColor.transpose());

	glUniform4fv(simpleFillProg->getFillColorLocation(), 1,
				 &foregroundColor(0));

	OevGLES::GLBindVertexArrayObject bindVertexArrayObject;
	OevGLES::GLBindVertexArrayObject bindVertexArray;

	if (renderContextPtr->quadVertexArray.valid()) {
		bindVertexArrayObject =
			OevGLES::GLBindVertexArrayObject(renderContextPtr->quadVertexArray);
	} else {
		bindVertexArray = OevGLES::GLBindVertexArrayObject (renderContextPtr->quadVertexArray);
		// setup the vertex coordinates
		OevGLES::GLVertexArrayAttribObject enableVertexPosAttr(
			true, simpleFillProg->getVertexPosLocation());
		glVertexAttribPointer(simpleFillProg->getVertexPosLocation(), 4,
							  GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	}
	
	glDrawArrays(GL_TRIANGLE_FAN, 0, OevGLES::RenderContext::quadVertexBufferNumVertexes);

}

} /* namespace OevControls */
