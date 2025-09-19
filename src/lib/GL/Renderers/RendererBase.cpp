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

namespace OevGLES {

int32_t RenderStandardUniforms::maxMatrixChangeCounter = 1;


RenderStandardUniforms::RenderStandardUniforms() 
	:modelMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,viewMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
		Mat4WithChangeCounter()))
	,projMatrixPtr (std::make_shared<Mat4WithChangeCounter>(
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



RendererBase::RendererBase()
{ }

RendererBase::~RendererBase() {
}

} // namespace OevGLES
