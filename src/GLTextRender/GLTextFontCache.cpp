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

GLTextFontCacheItem::GLTextFontCacheItem(PangoFont* font)
: pangoFont (font,true),
  freetypeFace {pango_ft2_font_get_face(pangoFont)},
  fontDesc { pango_font_describe(font)},
  fontDescHash {pango_font_description_hash(fontDesc)},
  fontMetrics {pango_font_get_metrics(pangoFont, nullptr),false}
{

#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextFontCache");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": "
			<< "\n\tfont family = " << pango_font_description_get_family(fontDesc)
			<< "\n\tfont gravity = " << pango_font_description_get_gravity(fontDesc)
			<< "\n\tfont size = " << pango_font_description_get_size(fontDesc) / static_cast<double>(PANGO_SCALE)
			<< "\n\tfont style = " << pango_font_description_get_style(fontDesc)
			<< "\n\tfont weight = " << pango_font_description_get_weight(fontDesc)
			);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "\t fontmetrics : "
			<< "\n\t height = " << pango_font_metrics_get_height(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t ascent = " << pango_font_metrics_get_ascent(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t descent = " << pango_font_metrics_get_descent(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t approx_char_width = " << pango_font_metrics_get_approximate_char_width(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t approx_digit_width = " << pango_font_metrics_get_approximate_digit_width(fontMetrics) / static_cast<double>(PANGO_SCALE)
			);
}

GLTextFontCacheItem::GLTextFontCacheItem(GLTextFontCacheItem&& source)
: pangoFont {std::move(source.pangoFont)},
  freetypeFace {source.freetypeFace},
  fontDesc {source.fontDesc},
  fontDescHash {std::move(source.fontDescHash)},
  fontMetrics {std::move(source.fontMetrics)}
{
	source.freetypeFace = nullptr;
	source.fontDesc = nullptr;
}

GLTextFontCacheItem& GLTextFontCacheItem::operator = (GLTextFontCacheItem&& source) {
	pangoFont = std::move(source.pangoFont);
	freetypeFace = source.freetypeFace;
	source.freetypeFace = nullptr;
	if (fontDesc != nullptr) {
		pango_font_description_free(fontDesc);
	}
	fontDesc = source.fontDesc;
	source.fontDesc = nullptr;
	fontDescHash = source.fontDescHash;
	fontMetrics = std::move(source.fontMetrics);
	return *this;
}


GLTextFontCache::GLTextFontCache()
{

#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextFontCache");
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
			LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": Found record in the cache.");

			break;
		}
	}

	if (result == nullptr) {
		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": Found no record in the cache. Create and insert an new one.");
		GLTextFontCacheItem newCacheItem(font);
		auto fontHash = newCacheItem.getFontDescHash();
		auto insRes = fontCache.insert(std::pair<guint,GLTextFontCacheItem>(fontHash,std::move(newCacheItem)));

		result = &(insRes->second);
	}

	return result;
}

} /* namespace OevGLES */
