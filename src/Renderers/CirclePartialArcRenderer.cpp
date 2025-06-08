/*
 * CirclePartialArcRenderer.cpp
 *
 *  Created on: Jun 4, 2025
 *      Author: hor
 */

#include "CirclePartialArcRenderer.h"

namespace OevGLES {

CirclePartialArcRenderer::CirclePartialArcRenderer(CirclePolygonVertexContainer& circlePolygonVertexContainer)
		:CircleBaseRenderer{circlePolygonVertexContainer}
{


}

CirclePartialArcRenderer::~CirclePartialArcRenderer() {

}

void CirclePartialArcRenderer::setArcRangeDeg(double arcRangeDeg) {
	if (this->arcRangeDeg != arcRangeDeg) {
		this->arcRangeDeg = arcRangeDeg;

		dirtyArc = true;
	}
}

void CirclePartialArcRenderer::setStartAngleDeg(double startAngleDeg) {
	if (this->startAngleDeg != startAngleDeg) {
		this->startAngleDeg = startAngleDeg;

		dirtyArc = true;
	}
}

void CirclePartialArcRenderer::draw(const OevGLES::Mat4 &modelMatrix,
		const OevGLES::Mat4 &viewMatrix, const OevGLES::Mat4 &ProjMatrix,
		const OevGLES::Mat4 &MVMatrix, const OevGLES::Mat4 &MVPMatrix,
		const OevGLES::Vec3 &lightDir, const OevGLES::Vec4 &lightColor,
		const OevGLES::Vec4 &ambientLightColor) {
}

} /* namespace OevGLES */
