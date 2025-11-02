/*
 * RendererBase.cpp
 *
 *  Created on: May 17, 2018
 *      Author: kai_horstmann
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

#include <memory>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "Renderers/RendererBase.h"

#include "OVFCommon.h"

#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr loggerRenderStandardUniforms;
#endif


namespace OevGLES {

int32_t RenderStandardUniforms::maxMatrixChangeCounter = 1;

RenderStandardUniforms::Mat4WithChangeCounter::Mat4WithChangeCounter() 
			: matrix4{Mat4::Identity()}
			, changeCounter {1}
{
#if defined HAVE_LOG4CXX_H
		// Get the logger if necessary
		if (!loggerRenderStandardUniforms) {
			loggerRenderStandardUniforms = log4cxx::Logger::getLogger("OpenVarioFront.Renderers.RenderStandardUniforms");
		}
#endif

}


RenderStandardUniforms::RenderStandardUniforms() 
	:modelMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,viewMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,projectionMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,MVMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,MVPMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,lightDirPtr (std::make_shared<Vec3>(
		Vec3{0,0,1}))
	,lightColorPtr (std::make_shared<Vec4>(
		Vec4{0,0,0,1}))
	,ambientLightColorPtr (std::make_shared<Vec4>(
		Vec4{0,0,0,1}))
{}

Mat4 & RenderStandardUniforms::getModelMatrix () {
	maxMatrixChangeCounter ++;
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": new maxMatrixChangeCounter = " 
		<< maxMatrixChangeCounter);
	modelMatrixPtr->changeCounter = maxMatrixChangeCounter;
	return modelMatrixPtr->matrix4;
}
Mat4 & RenderStandardUniforms::getViewMatrix () {
	maxMatrixChangeCounter ++;
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": new maxMatrixChangeCounter = " 
		<< maxMatrixChangeCounter);
	viewMatrixPtr->changeCounter = maxMatrixChangeCounter;
	return viewMatrixPtr->matrix4;
}
Mat4 & RenderStandardUniforms::getProjMatrix () {
	maxMatrixChangeCounter ++;
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": new maxMatrixChangeCounter = " 
		<< maxMatrixChangeCounter);
	projectionMatrixPtr->changeCounter = maxMatrixChangeCounter;
	return projectionMatrixPtr->matrix4;
}

Mat4 const &RenderStandardUniforms::getMVMatrixC() const {
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": modelMatrixPtr->changeCounter = "
		<< modelMatrixPtr->changeCounter
		<< ", viewMatrixPtr->changeCounter = "
		<< viewMatrixPtr->changeCounter
		<< ", MVMatrixPtr->changeCounter = "
		<< MVMatrixPtr->changeCounter);
	if (std::max(modelMatrixPtr->changeCounter,
				 viewMatrixPtr->changeCounter) >
		MVMatrixPtr->changeCounter) {
		recalcMVMatrix();
	}
	return MVMatrixPtr->matrix4;
}
Mat4 const &RenderStandardUniforms::getMVPMatrixC() const {
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": modelMatrixPtr->changeCounter = "
		<< modelMatrixPtr->changeCounter
		<< ", viewMatrixPtr->changeCounter = "
		<< viewMatrixPtr->changeCounter
		<< ", MVMatrixPtr->changeCounter = "
		<< MVMatrixPtr->changeCounter);
	if (std::max(modelMatrixPtr->changeCounter,
				 viewMatrixPtr->changeCounter) >
		MVMatrixPtr->changeCounter) {
		recalcMVMatrix();
	}
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": projMatrixPtr->changeCounter = "
		<< projectionMatrixPtr->changeCounter
		<< ", MVMatrixPtr->changeCounter = "
		<< MVMatrixPtr->changeCounter
		<< ", MVPMatrixPtr->changeCounter = "
		<< MVPMatrixPtr->changeCounter);
	if (std::max(projectionMatrixPtr->changeCounter, MVMatrixPtr->changeCounter) >
		MVPMatrixPtr->changeCounter) {
		recalcMVPMatrix();
	}
	return MVPMatrixPtr->matrix4;
}

void RenderStandardUniforms::resetModelMatrixPtr() {
	modelMatrixPtr.reset(new Mat4WithChangeCounter(*modelMatrixPtr));
	resetMVMatrix();
}
void RenderStandardUniforms::setModelMatrixPtr(
		Mat4WithChangeCounterPtr const &modelMatrixPtr) {
	this->modelMatrixPtr = modelMatrixPtr;
	resetMVMatrix();
	}
void RenderStandardUniforms::resetViewMatrixPtr() {
	viewMatrixPtr.reset(new Mat4WithChangeCounter(*viewMatrixPtr));
	resetMVMatrix();
}
void RenderStandardUniforms::setViewMatrixPtr(
		Mat4WithChangeCounterPtr const &viewMatrixPtr) {
	this->viewMatrixPtr = viewMatrixPtr;	
	resetMVMatrix();
	}
void RenderStandardUniforms::resetProjectionMatrixPtr() {
	projectionMatrixPtr.reset(new Mat4WithChangeCounter(*projectionMatrixPtr));
	resetMVPMatrix();
}
void RenderStandardUniforms::setProjectionMatrixPtr(
		Mat4WithChangeCounterPtr const &projectionMatrixPtr){
	this->projectionMatrixPtr = projectionMatrixPtr;
	resetMVPMatrix();
	}
void RenderStandardUniforms::resetLightDirPtr() {
	lightDirPtr.reset(new Vec3(*lightDirPtr));
}
void RenderStandardUniforms::resetLightColorPtr() {
	lightColorPtr.reset(new Vec4(*lightColorPtr));
}
void RenderStandardUniforms::resetAmbientLightColorPtr() {
	ambientLightColorPtr.reset(new Vec4(*ambientLightColorPtr));
}

void RenderStandardUniforms::recalcMVMatrix() const {
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": old MVMatrixPtr->changeCounter = "
		<< MVMatrixPtr->changeCounter
		<< ", new Value is" << maxMatrixChangeCounter);
	MVMatrixPtr->changeCounter = maxMatrixChangeCounter;
	MVMatrixPtr->matrix4 = viewMatrixPtr->matrix4 * modelMatrixPtr->matrix4;
	recalcMVPMatrix();
}

void RenderStandardUniforms::recalcMVPMatrix() const {
	LOG4CXX_DEBUG(loggerRenderStandardUniforms,
		__PRETTY_FUNCTION__ << ": old MVPMatrixPtr->changeCounter = "
		<< MVPMatrixPtr->changeCounter
		<< ", new Value is" << maxMatrixChangeCounter);
	MVPMatrixPtr->changeCounter = maxMatrixChangeCounter;
	MVPMatrixPtr->matrix4 = projectionMatrixPtr->matrix4 * MVMatrixPtr->matrix4;
}

void RenderStandardUniforms::resetMVMatrix() {
	MVMatrixPtr.reset(new Mat4WithChangeCounter(*MVMatrixPtr));
	resetMVPMatrix();
}

void RenderStandardUniforms::resetMVPMatrix() {
	MVPMatrixPtr.reset(new Mat4WithChangeCounter(*MVPMatrixPtr));
}

RendererBase::RendererBase(RendererContextSharedPtr &context)
	:context {context}
 {}

RendererBase::~RendererBase() {}

} // namespace OevGLES
