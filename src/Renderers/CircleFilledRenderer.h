/*
 * CircleFilledRenderer.h
 *
 *  Created on: Jun 1, 2025
 *      Author: hor
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

#ifndef RENDERERS_CIRCLEFILLEDRENDERER_H_
#define RENDERERS_CIRCLEFILLEDRENDERER_H_

#include "CircleBaseRenderer.h"

namespace OevGLES {

class CircleFilledRenderer: public CircleBaseRenderer {
public:
	CircleFilledRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer);
	virtual ~CircleFilledRenderer();

	virtual void setupVertexBuffers() override;
	virtual void draw(const OevGLES::Mat4 &modelMatrix,
			const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
			const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
			const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
			const OevGLES::Vec4 &ambientLightColor) override;

private:

	// Hide unused methods from public usage because they are useless.
	// They are only declared here but not implemented because I do not need them too.
	double getPrimarySecondaryZOffset();
	double getSecondaryRadius();
	void setPrimarySecondaryZOffset(double primarySecondaryZOffset);
	void setSecondaryRadius(double secondaryRadius);

};

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEFILLEDRENDERER_H_ */
