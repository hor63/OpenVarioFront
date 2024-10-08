/*
 * GLTextFontCache.cpp
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "GLTextFontCache.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

GLTextFontCacheItem::GLTextFontCacheItem(PangoFont* font) {

#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextRenderer");
	}
#endif

	fontDesc = pango_font_describe(font);
	fontDescHash = pango_font_description_hash(fontDesc);
	fontMetrics = pango_font_get_metrics(font, nullptr);
}

GLTextFontCache::GLTextFontCache()
{

#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextRenderer");
	}
#endif

}


GLTextFontCacheItem* GLTextFontCache::getCacheItem (PangoFont* font) {
	GLTextFontCacheItem* result = nullptr;
	PangoFcFont* fcFont = PANGO_FC_FONT(font);

	auto range = fontCache.equal_range(pango_font_description_hash(fcFont->description));

	for (auto iter = range.first;iter != range.second; ++iter){
		if (pango_font_description_equal(fcFont->description, iter->second.getFontDesc())) {
			result = &iter->second;
			break;
		}
	}

	if (result == nullptr) {
		GLTextFontCacheItem newCacheItem(font);
		auto fontHash = newCacheItem.getFontDescHash();
		auto insRes = fontCache.insert(std::pair<guint,GLTextFontCacheItem>(fontHash,std::move(newCacheItem)));

		result = &(insRes->second);
	}

	return result;
}

} /* namespace OevGLES */
