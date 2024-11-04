/*
 * GLTextFontCache.h
 *
 *  Created on: Oct 3, 2024
 *      Author: hor
 *
 *   Cache and cache item classes for the font and font bitmap cache.
 *   The key for the cache item is the PangoFont hash value.
 *   The PangoFont is the specialization of a font face at a specific size and various attributes (style, weight ...)
 *   The cache items also maintains the list the font textures for the rendered glyphs as well as the map for the
 *   glyph descriptions for this font.
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

#ifndef GLTEXTRENDER_GLTEXTFONTCACHE_H_
#define GLTEXTRENDER_GLTEXTFONTCACHE_H_

#include <unordered_map>

#include "GLTextPangoCPPWrappers.h"
#include "GLTextFontTexture.h"

namespace OevGLES {

class GLTextFontCache;
class GLTextFontTexture;
class GLTextFontCacheItem;

/// X-Coordinates go left->right in x-direction
/// and bottom->up in y-direction
struct GLTextGlyphBBox {
	int32_t xLeft; //< left edge
	int32_t yBottom; // << bottom edge
	int32_t xRight; // right edge
	int32_t yTop; // << top edge

	GLTextGlyphBBox (
			int32_t xLeft,
			int32_t yBottom,
			int32_t xRight,
			int32_t yTop
			) :
				xLeft{xLeft},
				yBottom{yBottom},
				xRight{xRight},
				yTop{yTop}
	{}

	/// Create an invalid BBox
	GLTextGlyphBBox() :
		xLeft{-1},
		yBottom{-1},
		xRight{-1},
		yTop{-1}
		{}

	bool isValid(){
		return yTop >= 0 && xRight >= 0;
	}

	int32_t height() {
		return yTop - yBottom;
	}
	int32_t width() {
		return xRight - xLeft;
	}

};


struct GLTextFontCacheGlyphItem {

	GLTextFontCacheGlyphItem(
			PangoGlyph glyphIndex,
			GLTextFontCacheItem& cacheItem,
			GLTextFontTexture& texture,
			GLTextGlyphBBox texturePosition,
			FT_Glyph_Metrics glyphMetrics
			) :
				glyphIndex {glyphIndex},
				cacheItem {cacheItem},
				texture {texture},
				texturePosition {texturePosition},
				glyphMetrics {glyphMetrics}
				{}

	PangoGlyph glyphIndex;

	GLTextFontCacheItem& cacheItem;
	GLTextFontTexture& texture;

	GLTextGlyphBBox texturePosition;

	FT_Glyph_Metrics glyphMetrics;

};

class GLTextFontCacheItem final {
public:
	GLTextFontCacheItem();
	GLTextFontCacheItem(PangoFont* font);

	~GLTextFontCacheItem() {
		if (fontDesc != nullptr) {
			pango_font_description_free(fontDesc);
			fontDesc = nullptr;
		}
		freetypeFace = nullptr;
	}

	GLTextFontCacheItem(const GLTextFontCacheItem& source) = delete;

	GLTextFontCacheItem(GLTextFontCacheItem&& source);

	GLTextFontCacheItem& operator = (const GLTextFontCacheItem& source) = delete;

	GLTextFontCacheItem& operator = (GLTextFontCacheItem&& source);

	PangoFontDescription *getFontDesc() {
		return fontDesc;
	}

	guint getFontDescHash() {
		return fontDescHash;
	}

	void addGlyphToTexture(PangoGlyph glyphIndex);

	void exportTextureBitmaps();

private:
	CppPangoFont pangoFont;
	FT_Face freetypeFace = nullptr;
	PangoFontDescription* fontDesc = nullptr;
	guint fontDescHash = 0;
	CppPangoFontMetrics fontMetrics;

	std::list<GLTextFontTexture> textureList;
	std::unordered_map<PangoGlyph,GLTextFontCacheGlyphItem> glyphMap;
};

class GLTextFontCache final {
public:
	GLTextFontCache();
	~GLTextFontCache() {};

	GLTextFontCacheItem* getCacheItem (PangoFont* font);

	void exportTextureBitmaps();

private:
	std::unordered_multimap<guint,GLTextFontCacheItem> fontCache;
};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTCACHE_H_ */
