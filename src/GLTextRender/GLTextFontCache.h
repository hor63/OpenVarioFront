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

namespace OevGLES {

class GLTextFontCache;

class GLTextFontCacheItem final {
public:
	GLTextFontCacheItem();
	GLTextFontCacheItem(PangoFont* font) {
		fontDesc = pango_font_describe(font);
		fontDescHash = pango_font_description_hash(fontDesc);
	}
	~GLTextFontCacheItem() {
		if (fontDesc != nullptr) {
			pango_font_description_free(fontDesc);
			fontDesc = nullptr;
		}
	}

	GLTextFontCacheItem(const GLTextFontCacheItem& source) = delete;
	/*{
		fontDesc = pango_font_description_copy(source.fontDesc);
		fontDescHash = source.fontDescHash;
	}*/

	GLTextFontCacheItem(GLTextFontCacheItem&& source) {
		fontDesc = source.fontDesc;
		source.fontDesc = nullptr;
		fontDescHash = source.fontDescHash;
	}

	GLTextFontCacheItem& operator = (const GLTextFontCacheItem& source) = delete;
	/*{
		fontDesc = pango_font_description_copy(source.fontDesc);
		fontDescHash = source.fontDescHash;
		return *this;
	}*/

	GLTextFontCacheItem& operator = (GLTextFontCacheItem&& source) {
		fontDesc = source.fontDesc;
		source.fontDesc = nullptr;
		fontDescHash = source.fontDescHash;
		return *this;
	}

	PangoFontDescription *getFontDesc() {
		return fontDesc;
	}

	guint getFontDescHash() {
		return fontDescHash;
	}

private:
	PangoFontDescription *fontDesc = nullptr;
	guint fontDescHash = 0;
};

class GLTextFontCache final {
public:
	GLTextFontCache() {};
	~GLTextFontCache() {};

	GLTextFontCacheItem* getCacheItem (PangoFont* font);

private:
	std::unordered_map<guint,GLTextFontCacheItem> fontCache;
};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTCACHE_H_ */
