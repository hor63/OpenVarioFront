/*
 * GLTextFontTexture.h
 *
 *  Created on: Oct 16, 2024
 *      Author: hor
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

#ifndef GLTEXTRENDER_GLTEXTFONTTEXTURE_H_
#define GLTEXTRENDER_GLTEXTFONTTEXTURE_H_

#include <cstdint>
#include <list>

#include "GLTextFontCache.h"
#include "GLES/GLTexture.h"

namespace OevGLES {

class GLTextFontCacheItem;

struct GLTextGlyphBBox;

/** \brief Manages a GL texture which contains the images of a number of glyphs of a font.
 *
 *   The glyphs are arranged in lines with 1 pixel space to the edges and between the glyphs
 *   Besides the actual texture which is managed by a GLTexture member the class stores the glyph
 *   positions and sizes of the current line as well as the previous line.
 *   This allows precise positioning of new glyhps above the glyphs of the previous line below.
 *
 */
class GLTextFontTexture {
public:

	static int32_t constexpr textureDimension = 128;

	using GlyphBBoxList = std::list<GLTextGlyphBBox>;

	GLTextFontTexture(GLTextFontCacheItem& cacheItem,int32_t sizeXY);
	GLTextFontTexture(const GLTextFontTexture &other) = delete;
	GLTextFontTexture(GLTextFontTexture &&other);
	virtual ~GLTextFontTexture();
	GLTextFontTexture& operator=(const GLTextFontTexture &other) = delete;
	GLTextFontTexture& operator=(GLTextFontTexture &&other) = delete;

	GLTextGlyphBBox addGlyphToTexture(FT_GlyphSlot glyphSlot);
	GLTextGlyphBBox addFallbackTofuGlyphToTexture(int32_t width,int32_t height);

	bool isFull() const {
		return full;
	}

	void exportTextureBitmap(int bitmapNumber);

private:


	GLTextFontCacheItem& fontCacheItem;

	GlyphBBoxList previousGlyphLine;
	GlyphBBoxList currentGlyphLine;

	GLTexture texture;
	TextureData textureData;

	bool full = false;
	/// Determines if the local texture buffer is not synchronized with the GL texture data.
	bool dirty = true;

	/** \brief Determine how low a glyph can be positioned when \ref previousGlyphLine does not determine a lower limit.
	 *
	 * It can happen that there is a gap left between the rightmost glyph in \ref previousGlyphLine and the right edge of the texture.
	 * When a narrow glyph is placed very close to the right edge it may be right of the rightmost glyph in \ref previousGlyphLine.
	 * Therefore this glyph would be placed at the very bottom of the texture thus overwriting a glyph lying there.
	 * This value here ensures that a glyph which "falls though the cracks" does not drop to the bottom overwriting a random glyph at
	 * the bottom right of the texture.
	 *
	 */
	int32_t topPosPreviousLines = 1;

	// count the rows being filled for diagnostics and debugging purposes.
	int rowNum = 0;

	/** \brief Copy the rendered glyph image from the freetype bitmap to the texture
	 *
	 * Sets \ref dirty true.
	 *
	 * \param glyphCoord Destination coordinates where the glyph image is to be drawn.
	 * \param glyphSlot The Freetype glyph slot with the loaded glyph and the rendered glyph image
	 */
	void copyGlyphImageToTexture(GLTextGlyphBBox const glyphCoord, FT_GlyphSlot glyphSlot );

	/** \brief Draws a rectangle with a line one pixel wide, i.e. the Tofu.
	 *
	 * Sets \ref dirty true.
	 *
	 * \param glyphCoord Corners of
	 */
	void drawTofuGlyphImageToTexture(GLTextGlyphBBox const glyphCoord);

	/** \brief Moves the content of \ref currentGlyphLine to \ref previousGlyphLine.
	 *
	 * First re-calculates \ref topPosPreviousLines for \ref previousGlyphLine before its content is being overwritten.
	 * Then moves the content of \ref currentGlyphLine to \ref previousGlyphLine.
	 * Finally clears \ref currentGlyphLine.
	 */
	void startNewGlyphLine();

	/** \brief Add a new rectangle in the texture and return its coordinates
	 *
	 *	If the rectangle does not fit into the texture \ref full is being set \p true.
	 *
	 * \param width Width in pixel
	 * \param height Height in pixel
	 * \return The coordinates (bounding box) of the rectangle in the texture.
	 *   Check validity of the return value. When it is invalid no space could be
	 *   found in this texture for a rectangle of the given size.
	 */
	GLTextGlyphBBox addNewRectangle(int32_t width, int32_t height);

};



} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTTEXTURE_H_ */
