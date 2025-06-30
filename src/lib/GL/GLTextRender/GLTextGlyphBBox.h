/*
 * GLTextGlyphBBox.h
 *
 *  Created on: Dec 27, 2024
 *      Author: hor
 *
 *	Template class for bounding boxes.
 *	Implementation aliases for int32_t and float are being provided.
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2024 Kai Horstmann
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

#ifndef GLTEXTRENDER_GLTEXTGLYPHBBOX_H_
#define GLTEXTRENDER_GLTEXTGLYPHBBOX_H_

#include <cstdint>

namespace OevGLES {

/**
 *  \brief Bounding box in pixel for a glyph image in a texture
 *
 *  Coordinates follow OpenGL convention
 *  X-coordinates go left->right.\n
 *  Y-coordinates go bottom->up.
 *
 *  An invalid BBox is indicated by negative coordinates of the upper right corner.
 */
template <typename T>
class GLTextGlyphBBoxT  {
public:

	T xLeft; //< left edge
	T yBottom; // << bottom edge
	T xRight; // right edge
	T yTop; // << top edge

	GLTextGlyphBBoxT (
			T xLeft,
			T yBottom,
			T xRight,
			T yTop
			) :
				xLeft{xLeft},
				yBottom{yBottom},
				xRight{xRight},
				yTop{yTop}
	{}

	/// \brief Creates an invalid BBox
	GLTextGlyphBBoxT() :
		xLeft{-1},
		yBottom{-1},
		xRight{-1},
		yTop{-1}
	{}

	/**
	 * @brief A BBox is valid when the coordinates of the top right corner are positive.
	 *
	 * @return true when the x and y coordinates of the top-left corner are both >= 0
	 */
	bool isValid() const {
		return yTop >= 0 && xRight >= 0;
	}

	T height() const {
		return yTop - yBottom;
	}
	T width() const {
		return xRight - xLeft;
	}

};

using GLTextGlyphBBoxI = GLTextGlyphBBoxT<int32_t>;
using GLTextGlyphBBoxF = GLTextGlyphBBoxT<float>;

} // namespace OevGLES {

#endif /* GLTEXTRENDER_GLTEXTGLYPHBBOX_H_ */
