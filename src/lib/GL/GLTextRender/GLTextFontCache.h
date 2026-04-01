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
#include "GLTextGlyphBBox.h"
#include "GLTextFontTexture.h"

namespace OevGLES {

class GLTextGlobals;
class GLTextFontCache;
class GLTextFontTexture;
class GLTextFontCacheItem;

/**
 * @brief Holds data of a glyph image with reference to the texture and the position within that texture
 *
 */
struct GLTextFontCacheGlyphItem {


	GLTextFontCacheGlyphItem(
			PangoGlyph glyphIndex,
			GLTextFontCacheItem& cacheItem,
			GLTextFontTexture& texture,
			GLTextGlyphBBoxI const& texturePosition,
			FT_Glyph_Metrics glyphMetrics,
			bool renderGlyph
			);

	PangoGlyph glyphIndex;

	/// \brief The font cache item to which this glyph resides.
	///
	/// There is one cache item for each unique PangoFont.
	GLTextFontCacheItem& cacheItem;
	/// \brief The texture where the glyph image resides.
	GLTextFontTexture& texture;

	/// \brief Position of the glyph within the texture referenced in \ref texture
	GLTextGlyphBBoxI texturePosition;

	/// \brief Position in the texture from 0.0 to 1.0 normalized to the
	///   texture dimensions.
	GLTextGlyphBBoxF texturePositionNormalized;

	/// \brief FreeType glyph metrics
	FT_Glyph_Metrics glyphMetrics;

	///
	/** \brief Shall this glyph be rendered to the canvas at all?
	 *
	 * Some glyphs have an image dimension of 0, i.e. there is no image to be rendered.
	 * Example are the various white-space characters which still take up space
	 * in a rendered text, but have no image content.
	 * Such glyphs are just skipped in the rendering process.
	 * But I still need to store this information in the cache lists.
	 */
	bool renderGlyph;

};

/**
 * @brief Represents one font on which glyphs can be rendered.
 *
 * Initially an object of this class is being created for one PangFont instance.
 * Different PangoFont instances will reuse this object when the content of the font's PangFontDescription is equal.
 *
 */
class GLTextFontCacheItem final {
	friend class GLTextFontCache;
public:
	/// \brief Default constructor is deleted.
	GLTextFontCacheItem() = delete;
	/**
	 *  @brief Constructor for a given PangoFont
	 *
	 *  The font is stored internally; its references count is incremented and decremented in the destructor.
	 *
	 *  @param font Pointer to a PangoFont object.
	 *  @param globals Back pointer to the globals object which owns the font cache.
	 */
	GLTextFontCacheItem(PangoFont* font, GLTextGlobals* globals);

	~GLTextFontCacheItem() {
		if (fontDesc != nullptr) {
			pango_font_description_free(fontDesc);
			fontDesc = nullptr;
		}
		freetypeFace = nullptr;
	}

	/**
	 *  @brief deleted: You cannot copy instances.
	 *
	 *  @param source The instance you still cannot copy D:
	 */
	GLTextFontCacheItem(const GLTextFontCacheItem& source) = delete;

	/**
	 *  @brief Move constructor. Source loses references to the Pango font, the font face,
	 *  the font description. Contents of textures list and glyph map are moved to the target.
	 *
	 *  @param source Instance whose content is taken over and remains empty, and with nullptr values for the pointers.
	 */
	GLTextFontCacheItem(GLTextFontCacheItem&& source);

	/**
	 *  @brief deleted: You cannot copy instances.
	 *
	 *  @param source The instance you still cannot copy D:
	 *  @return Reference to the copy target
	 */
	GLTextFontCacheItem& operator = (const GLTextFontCacheItem& source) = delete;

	/**
	 *  @brief Move constructor. Source loses references to the Pango font, the font face,
	 *  the font description. Contents of textures list and glyph map are moved to the target.
	 *
	 *  @param source Instance whose content is taken over and remains empty, and with nullptr values for the pointers.
	 *  @return Reference to the target.
	 */
	GLTextFontCacheItem& operator = (GLTextFontCacheItem&& source);

	/**
	 *  @brief Return the font description of the associated PangoFont
	 *
	 *  This instance retains ownership of the font description.
	 *
	 *  @return the font description of the associated PangoFont.
	 */
	PangoFontDescription *getFontDesc() {
		return fontDesc;
	}

	/**
	 *  @brief hash value of the font description of the associated PangoFont.
	 *
	 *  @return hash value of the font description of the associated PangoFont.
	 */
	guint getFontDescHash() {
		return fontDescHash;
	}

	/**
	 *  @brief Add a glyph image to one of the textures of this instance when it did not exist before.
	 *
	 *  The glyph index is the index within the font, not a Unicode code point. Pango provides that index
	 *  in the render callback. Otherwise you need to retrieve the index for the code point from the font yourself.
	 *
	 *  @param glyphIndex Index of the glyph within the associated font.
	 */
	void addGlyphToTexture(PangoGlyph glyphIndex);

	/**
	 *  @brief Retrieve the information about texture, location within and size of a glyph
	 *
	 *  When the glyph did not exist before in any of the textures of the associated font
	 *  it will be added by calling \ref addGlyphToTexture.
	 *
	 *  The glyph index is the index within the font, not a Unicode code point. Pango provides that index
	 *  in the render callback. Otherwise you need to retrieve the index for the code point from the font yourself.
	 *
	 *  @param glyphIndex Index of the glyph within the associated font.
	 *  @return Reference to the glyph cache information.
	 */
	GLTextFontCacheGlyphItem const & getGlyphInfo (PangoGlyph glyphIndex);

	/**
	 *  @brief Writes the texture bitmaps as raw data files.
	 *
	 *  The call is for diagnostic and debugging purposes.\n
	 *  It writes a file with the raw bitmap data as 8-bit grey scale values.
	 *  The file name contains the font family and the pixel sizes in width and height and an index number
	 *  because one FontCacheItem can have more than one texture to hold all glyphs.
	 *
	 *  Please note that the bitmap is stored in GL order, i.e. bottom to top.\n
	 *  You can open and view it for example with gimp, but expect the content to be displayed upside down.
	 *
	 */
	void exportTextureBitmaps();

private:
	GLTextGlobals* globals;

	/// @brief Reference to the PangoFont. Reference count is increased during the liftime of this object.
	CppPangoFont pangoFont;
	/// @brief Freetype font associated with \ref pangoFont
	FT_Face freetypeFace = nullptr;
	/**
	 *  @brief font description of pangoFont at the time of object creation.
	 *
	 *  This is a copy of the internal description of \ref pangoFont and owned by this.
	 *  It will remain constant even if the description of pangoFont is being modified.
	 */
	PangoFontDescription* fontDesc = nullptr;
	/// @brief Cached hash value of \ref fontDesc.
	guint fontDescHash = 0;
	/// @brief Font metrics of \ref pangoFont as at the time of creation of this. It is a copy.
	CppPangoFontMetrics fontMetrics;

	/** \brief Size of the texture.
	 *
	 * The size is always a potency of 2.
	 * Width and height are identical, i.e. the texture is square.
	 * The texture size is being determined that >= 30 average sized glyphs fit into the texture.
	 * Goal is to have max. three textures per font. At four textures the next texture size would already be almost full.
	 * The texture size is hard capped at 1024x1024 pixel.
	 */
	int32_t textureSizePixel;



	/// @brief List of textures which hold the glyph images.
	std::list<GLTextFontTexture> textureList;
	/**
	 *  @brief Map of glyphs which have already been rendered.
	 *
	 *  For each rendered glyph is identified by its index into the font.
	 *
	 *  For each glyph the reference of the texture where it resides,
	 *  the measures and position in the texture are provided.
	 */
	std::unordered_map<PangoGlyph,GLTextFontCacheGlyphItem> glyphMap;

	/**
	 *  @brief Add the Tofu glyph image for a glyph index.
	 *
	 *  When the glyph for a given index cannot be loaded, or the glyph
	 *  can be loaded, but cannot be rendered \ref addGlyphToTexture() calls this
	 *  function.
	 *
	 *  The function ensures that the application does not enter an endless recursive
	 *  loop if even the tofu glyph (index 0) cannot be loaded.
	 *
	 *  @param glyphIndex Index of the glyph within the associated font.
	 */
	void addGlyphAsTofu(PangoGlyph glyphIndex);

	/** \brief Add a self-rendered Tofu glyph based on global font metrics
	 *
	 * The tofu glyph (index = 0) could not be loaded or rendered.
	 * Therefore render an own glyph with a frame. Size is based on the font metrics
	 * for the entire font.
	 */
	void addFallbackTofuGlyph();
};

/**
 *  @brief Map of font cache items which store images of glyphs for the different fonts.
 *
 * Sole purpose of the class is holding the font map which hold textures with glyph images.
 */
class GLTextFontCache final {
public:
	GLTextFontCache(GLTextGlobals* globals);
	~GLTextFontCache() {};

	/**
	 *  @brief Returns a cache item for a given font. Creates a new one when there
	 *  is no one yet in the cache.
	 *
	 *  The criteria to find a cache item is the hash value and equality of
	 *  the font description of \p font, not the identity of the pointer to \p font.\n
	 *  If no matching item can be found a new one is created, and inserted into \ref fontCache.
	 *
	 *  When a new cache item is created the glyph for index 0 is immediately added.
	 *  The glyph with index 0 is the invalid glyph, typically the empty box
	 *  (i.e. the Tofu glyph).
	 *
	 *  @param font The Pango font which is passed in the glyph render callback
	 *  @return A cache item for \p font.
	 */
	GLTextFontCacheItem* getCacheItem (PangoFont* font);

	/**
	 *  @brief  Writes the texture bitmaps of all \ref fontCache items as raw data files.
	 *
	 *  @see \ref GLTextFontCacheItem::exportTextureBitmaps()
	 */
	void exportTextureBitmaps();

private:
	/** @brief Map of all cached fonts with glyph images
	 *
	 * A font is a font face at a specific size, style... in the sense of a
	 * [Pango.Font](https://docs.gtk.org/Pango/class.Font.html)
	 * in contrast to a PangoFontFace
	 *
	 * The key is the hash value of the
	 * [Pango.FontDescription](https://docs.gtk.org/Pango/struct.FontDescription.html)
	 * belonging to the font.
	*/
	std::unordered_multimap<guint,GLTextFontCacheItem> fontCache;

	/** @brief Back reference to the globals to which this font cache belongs.
	 *
	 */
	GLTextGlobals* globals;
};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTCACHE_H_ */
