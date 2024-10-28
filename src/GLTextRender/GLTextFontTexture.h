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
 *   The glyphs are arranged in lines with 1 pixel space to the edges and 2 pixels between the glyphs
 *   Besides the actual texture which is managed by a GLTexture member the class stores the glyph
 *   positions and sizes of the current line as well as the previous line.
 *   This allows precise positioning of new glyhps above the glyphs of the previous line below.
 *
 */
class GLTextFontTexture {
public:

	static uint32_t constexpr textureDimension = 256;

	using GlyphBBoxList = std::list<GLTextGlyphBBox>;

	GLTextFontTexture(GLTextFontCacheItem* cacheItem,int32_t sizeXY);
	GLTextFontTexture(const GLTextFontTexture &other) = delete;
	GLTextFontTexture(GLTextFontTexture &&other);
	virtual ~GLTextFontTexture();
	GLTextFontTexture& operator=(const GLTextFontTexture &other) = delete;
	GLTextFontTexture& operator=(GLTextFontTexture &&other);

	GLTextGlyphBBox addGlyphToTexture(FT_Bitmap& glyphBitmap );

	bool isFull() {
		return full;
	}

private:


	GLTextFontCacheItem* fontCacheItem;

	GlyphBBoxList previousGlyphLine;
	GlyphBBoxList currentGlyphLine;

	GLTexture texture;
	TextureData textureData;

	bool full = false;
};



} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTTEXTURE_H_ */
