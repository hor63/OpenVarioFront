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

/** \brief Draw a full circle.
 *
 * Draws a full circle with the x-y center at 0,0.
 *
 * The z-offset of the circle center can be offset from 0;
 * default is y=0.
 *
 * Default radius is 1. Instead of scaling the radius up with the
 * model matrix set the radius explicitly here with \ref setPrimaryRadius().
 * This is necessary because the class can correctly determine how many polygon segments
 * are required to render a smooth circle without visible edges.\n
 * The class optimizes the number of polygon segments based on the given radius of the circle,
 * instead of always draw the maximum possible number of segments for very small circles.
 *
 * The normal at the center is always {0,0,1}, i.e. always pointing to z.
 * When used for directional lighting the cone body gets a dome like appearance in light.\n
 * If you really want the cone in lighting use a \ref CircleBaseRenderer object with scondary radius=0.
 * However, this class uses double the triangles than this class.
 *
 */
class CircleFilledRenderer: public CircleBaseRenderer {
public:
	CircleFilledRenderer(
		RenderContextSharedPtr &context,
		CirclePolygonVertexContainer& circlePolygonVertexContainer);
	virtual ~CircleFilledRenderer();

	virtual void setupVertexBuffers() override;
	virtual void draw(RenderStandardUniforms const &stdUniformData) override;

	/// \brief Set the z-offset of the center of the circle.
	///
	/// The perimeter of the circle is always drawn at z=0.\n
	/// You can offset the center from z. This results in a cone.
	void setCenterZOffset (double centerZOffset) {
		CircleBaseRenderer::setPrimarySecondaryZOffset(centerZOffset);
	}

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
