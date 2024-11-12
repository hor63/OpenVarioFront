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

/// Coordinates follow OpenGL convention>
/// X-coordinates go left->right.\n
/// Y-coordinates go bottom->up.
struct GLTextGlyphBBox final {
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

	/**
		 * @fn bool isValid()
	 * @brief
	 *
	 * @return true when the x and y coordinates of the top-left corner are both >= 0
	 */
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

/**
 * @struct GLTextFontCacheGlyphItem
 * @brief Holds data of a glyph image with reference to the texture and the position within that texture
 *
 */
struct GLTextFontCacheGlyphItem {

	GLTextFontCacheGlyphItem(
			PangoGlyph glyphIndex,
			GLTextFontCacheItem& cacheItem,
			GLTextFontTexture& texture,
			GLTextGlyphBBox const& texturePosition,
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

/**
 * @class GLTextFontCacheItem
 * @brief Represents one font on which glyphs can be rendered.
 *
 * Initially an object of this class is being created for one PangFont instance.
 * Different PangoFont instances will reuse this object when the content of the font's PangFontDescription is equal.
 *
 */
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

	/// @fn  GLTextFontCacheItem(const GLTextFontCacheItem&)
	/// @brief deleted: You cannot copy instances.
	///
	/// @param source The instance you still cannot copy D:
	GLTextFontCacheItem(const GLTextFontCacheItem& source) = delete;

	/// @fn  GLTextFontCacheItem(GLTextFontCacheItem&&)
	/// @brief Move constructor. Source loses references to the Pango font, the font face,
	/// the font description. Contents of textures list and glyph map are moved to the target.
	///
	/// @param source Instance whose content is taken over and remains empty, and with nullptr values for the pointers.
	GLTextFontCacheItem(GLTextFontCacheItem&& source);

	/// @fn GLTextFontCacheItem operator =&(const GLTextFontCacheItem&)
	/// @brief deleted: You cannot copy instances.
	///
	/// @param source The instance you still cannot copy D:
	/// @return Reference to the copy target
	GLTextFontCacheItem& operator = (const GLTextFontCacheItem& source) = delete;

	/// @fn GLTextFontCacheItem operator =&(GLTextFontCacheItem&&)
	/// @brief Move constructor. Source loses references to the Pango font, the font face,
	/// the font description. Contents of textures list and glyph map are moved to the target.
	///
	/// @param source Instance whose content is taken over and remains empty, and with nullptr values for the pointers.
	/// @return reference to the target.
	GLTextFontCacheItem& operator = (GLTextFontCacheItem&& source);

	/// @fn PangoFontDescription getFontDesc*()
	/// @brief Return the font description of the associated PangoFont
	///
	/// This instance retains ownership of the font description.
	///
	/// @return the font description of the associated PangoFont.
	PangoFontDescription *getFontDesc() {
		return fontDesc;
	}

	/// @fn guint getFontDescHash()
	/// @brief hash value of the font description of the associated PangoFont.
	///
	/// @return hash value of the font description of the associated PangoFont.
	guint getFontDescHash() {
		return fontDescHash;
	}

	/// @fn void addGlyphToTexture(PangoGlyph)
	/// @brief Add a glyph image to one of the textures of this instance when it did not exist before.
	///
	/// The glyph index is the index within the font, not a Unicode code point. Pango provides that index
	/// in the render callback. Otherwise you need to retrieve the index for the code point from the font yourself.
	///
	/// @param glyphIndex Index of the glyph within the associated font.
	void addGlyphToTexture(PangoGlyph glyphIndex);

	/// @fn void exportTextureBitmaps()
	/// @brief Writes the texture bitmaps as raw data files.
	///
	/// The call is for diagnostic and debugging purposes.\n
	/// It writes a file with the raw bitmap data as 8-bit grey scale values.
	/// The file name contains the font family and the pixel sizes in width and height and an index number
	/// because one FontCacheItem can have more than one texture to hold all glyphs.
	///
	/// Please note that the bitmap is stored in GL order, i.e. bottom to top.\n
	/// You can open and view it for example with gimp, but expect the content to be displayed upside down.
	///
	///
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
