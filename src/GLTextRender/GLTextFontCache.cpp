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

#include "GLTextGlobals.h"
#include "GLTextFontCache.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLTextFontCacheGlyphItem::GLTextFontCacheGlyphItem (
		PangoGlyph glyphIndex,
		GLTextFontCacheItem& cacheItem,
		GLTextFontTexture& texture,
		GLTextGlyphBBoxI const& texturePosition,
		FT_Glyph_Metrics glyphMetrics,
		bool renderGlyph
		) :
			glyphIndex {glyphIndex},
			cacheItem {cacheItem},
			texture {texture},
			texturePosition {texturePosition},
			texturePositionNormalized {
				static_cast<float>(texturePosition.xLeft) /
					static_cast<float>(texture.getTextureData().getWidth()),
					// Add 0.5 pixel on the bottom to the top. Some characters
					// were slightly truncated at the top, particular characters
					// with round tops, like 'C' '0' 'O'
				static_cast<float>(texturePosition.yBottom - 0.5f) /
					static_cast<float>(texture.getTextureData().getHeight()),
				static_cast<float>(texturePosition.xRight) /
					static_cast<float>(texture.getTextureData().getWidth()),
					// Add 0.5 pixel on the top to the top. Some characters
					// were slightly truncated at the top, particular characters
					// with round tops, like 'C' '0' 'O'
				(static_cast<float>(texturePosition.yTop) + 0.5f) /
					static_cast<float>(texture.getTextureData().getHeight())
			},
			glyphMetrics {glyphMetrics},
			renderGlyph{renderGlyph}
{

	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": "
			<< " New glyph index = " << glyphIndex
			<< ", pos = " << texturePosition.xLeft
			<< "x" << texturePosition.yBottom
			<< " " << texturePosition.xRight
			<< "x" << texturePosition.yTop
			);
	LOG4CXX_DEBUG(logger,
			   "\tNormalized position = " << texturePositionNormalized.xLeft
			<< "x" << texturePositionNormalized.yBottom
			<< " " << texturePositionNormalized.xRight
			<< "x" << texturePositionNormalized.yTop
			);


}

GLTextFontCacheItem::GLTextFontCacheItem(PangoFont* font, GLTextGlobals* globals)
: pangoFont (font,true),
  globals{globals},
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
	LOG4CXX_DEBUG(logger,"\tfontmetrics : "
			<< "\n\t height = " << pango_font_metrics_get_height(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t ascent = " << pango_font_metrics_get_ascent(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t descent = " << pango_font_metrics_get_descent(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t approx_char_width = " << pango_font_metrics_get_approximate_char_width(fontMetrics) / static_cast<double>(PANGO_SCALE)
			<< "\n\t approx_digit_width = " << pango_font_metrics_get_approximate_digit_width(fontMetrics) / static_cast<double>(PANGO_SCALE)
			);

	// Add 1 to width and height due to the 1 pixel extra space.
	// This may seem insignificant but is a real factor for small fonts (10 pt and smaller).
	auto areaGlyph = (1 + pango_font_metrics_get_ascent(fontMetrics) / static_cast<double>(PANGO_SCALE)) *
			(1 + pango_font_metrics_get_approximate_char_width(fontMetrics) / static_cast<double>(PANGO_SCALE));

	textureSizePixel = 32;
	for (;;) {
		LOG4CXX_DEBUG(logger,"\tTheoretic glyphs per "
				<< textureSizePixel << 'x' << textureSizePixel << " texture = "
				<< (textureSizePixel*textureSizePixel/areaGlyph));
		if ((textureSizePixel*textureSizePixel/areaGlyph) > 40.0) {
			break;
		}
		if (textureSizePixel >= 1024) {
			break;
		}

		textureSizePixel *= 2;
	}

	LOG4CXX_DEBUG(logger,"\tUse textures with size " << textureSizePixel << 'x' << textureSizePixel);

}

GLTextFontCacheItem::GLTextFontCacheItem(GLTextFontCacheItem&& source)
: pangoFont {std::move(source.pangoFont)},
  globals {source.globals},
  freetypeFace {source.freetypeFace},
  fontDesc {source.fontDesc},
  fontDescHash {std::move(source.fontDescHash)},
  fontMetrics {std::move(source.fontMetrics)},
  textureList {std::move(source.textureList)},
  textureSizePixel{source.textureSizePixel}
{
	source.globals = nullptr;
	source.freetypeFace = nullptr;
	source.fontDesc = nullptr;
	source.textureList.clear();
}

GLTextFontCacheItem& GLTextFontCacheItem::operator = (GLTextFontCacheItem&& source) {
	pangoFont = std::move(source.pangoFont);
	globals = source.globals;
	source.globals = nullptr;
	freetypeFace = source.freetypeFace;
	source.freetypeFace = nullptr;
	if (fontDesc != nullptr) {
		pango_font_description_free(fontDesc);
	}
	fontDesc = source.fontDesc;
	source.fontDesc = nullptr;
	fontDescHash = source.fontDescHash;
	fontMetrics = std::move(source.fontMetrics);
	textureList.clear();
	textureList = std::move(source.textureList);
	source.textureList.clear();
	return *this;
}


void GLTextFontCacheItem::addGlyphToTexture(PangoGlyph glyphIndex) {
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": Glyph index = " << glyphIndex
			<< " for font family " << freetypeFace->family_name
			<< ", size " << (static_cast<float>(freetypeFace->size->metrics.height)/64.0f));

	auto foundGlyph = glyphMap.find(glyphIndex);
	if (foundGlyph != glyphMap.end()) {
		// I have this glyph already cached.
		LOG4CXX_DEBUG(logger,"\tFound the glyph in the cache. Nothing to do.");
		return;
	}

	// The glyph had not been cached.
	// Load the glyph and render it
	auto ftRet = FT_Load_Glyph(freetypeFace, glyphIndex, FT_LOAD_DEFAULT);
	if (ftRet != 0) {
		LOG4CXX_ERROR (logger,__PRETTY_FUNCTION__ << ": Could not load glyph with index " << glyphIndex
				<< " for font family " << freetypeFace->family_name
				<< ", size " << (static_cast<float>(freetypeFace->size->metrics.height)/64.0f)
				<< ". Error is " << ftRet);
		addGlyphAsTofu(glyphIndex);
		return;
	}

	LOG4CXX_DEBUG (logger,"\tLoaded the glyph successfully. Glyph size = "
			<< freetypeFace->glyph->metrics.width/64.0f << "x" << freetypeFace->glyph->metrics.height/64.0f
			<< ", Bearing x = " << freetypeFace->glyph->metrics.horiBearingX/64.0f
			<< ", y = " << freetypeFace->glyph->metrics.horiBearingY/64.0f);

	GLTextGlyphBBoxI textureBBox;
	GLTextFontTexture *texture = nullptr;
	bool renderGlyph = true;

	// Check if the glyph should be rendered at all.
	// When width or height is 0 it is not to be rendered.
	if (freetypeFace->glyph->metrics.height == 0 || freetypeFace->glyph->metrics.height == 0) {
		// do not render the glyph. Skip over rendering it.

		LOG4CXX_DEBUG (logger,"\tGlyph image is empty. Glyph will not be rendered."
				);

		textureBBox.xLeft = textureBBox.xRight = 0;
		textureBBox.yBottom = textureBBox.yTop = 0;
		renderGlyph = false;
		// Obtain any texture there should be always one for the cache item
		auto textureListItem = textureList.begin();
		texture = textureListItem.operator ->();

	} else {
		// Glyph can be rendered.
		ftRet = FT_Render_Glyph(freetypeFace->glyph,FT_RENDER_MODE_NORMAL);
		if (ftRet != 0) {
			LOG4CXX_ERROR (logger,__PRETTY_FUNCTION__ << ": Could not render glyph with index " << glyphIndex
					<< " for font family " << freetypeFace->family_name
					<< ", size " << (static_cast<float>(freetypeFace->size->metrics.height)/64.0f)
					<< ". Error is " << ftRet);
			addGlyphAsTofu(glyphIndex);
			return;
		}

		LOG4CXX_DEBUG (logger,"\tRendered the glyph succesfully. Width ="
				<< freetypeFace->glyph->bitmap.width
				<< ", height = " << freetypeFace->glyph->bitmap.rows
				<< ", pitch = " << freetypeFace->glyph->bitmap.pitch
				<< ", num_grays = " << freetypeFace->glyph->bitmap.num_grays
				);

		for (auto textureListItem = textureList.begin();textureListItem != textureList.end();++textureListItem){
			texture = textureListItem.operator ->();
			if (!texture->isFull()) {
				textureBBox = texture->addGlyphToTexture(freetypeFace->glyph);

				if (textureBBox.isValid()) {
					LOG4CXX_DEBUG (logger,"\tAdded the glyph to the texture. Exit texture list loop.");
					break;
				} else {
					LOG4CXX_DEBUG (logger,"\tCould not add the glyph to the texture. Continue with the loop.");
				}
			} else {
				LOG4CXX_DEBUG (logger,"\tThe texture is full. Continue with the loop.");
			}
		}

		if (!textureBBox.isValid()) {
			// An invalid BBox means there was no existing texture where the glyph would fit.
			// Therefore create a new one and insert it into the list.
			auto newItem = textureList.insert(textureList.begin(),GLTextFontTexture(*this,textureSizePixel));

			texture = newItem.operator ->();
			textureBBox = newItem->addGlyphToTexture(freetypeFace->glyph);

			LOG4CXX_DEBUG (logger,"\tAdd a new texture. Glyph validity = " << textureBBox.isValid());
		}

	} // Glyph can be rendered.

	if (textureBBox.isValid() && texture != nullptr) {
		glyphMap.insert(std::pair(glyphIndex,GLTextFontCacheGlyphItem(glyphIndex, *this, *texture, textureBBox, freetypeFace->glyph->metrics,renderGlyph)));
	}
}

GLTextFontCacheGlyphItem const& GLTextFontCacheItem::getGlyphInfo (PangoGlyph glyphIndex) {
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": Glyph index = " << glyphIndex
			<< " for font family " << freetypeFace->family_name
			<< ", size " << (static_cast<float>(freetypeFace->size->metrics.height)/64.0f));

	auto foundGlyph = glyphMap.find(glyphIndex);
	if (foundGlyph != glyphMap.end()) {
		// I have this glyph already cached.
		LOG4CXX_DEBUG(logger,"\tFound the glyph in the cache. I'm done here.");
		return foundGlyph->second;
	}

	LOG4CXX_DEBUG(logger,"\tThe glyph did not yet exist in the cache. Add it.");

	// The glyph does not yet exist. Add it.
	addGlyphToTexture(glyphIndex);

	// The glyph is guaranteed to exist now. (and be it as the tofu glyph)
	foundGlyph = glyphMap.find(glyphIndex);
	return foundGlyph->second;


}


void GLTextFontCacheItem::addGlyphAsTofu(PangoGlyph glyphIndex) {

	if (glyphIndex == 0) {
		// Ups! I cannot even load the Tofu glyph.
		// Provide a dummy glyph image based on font metrics.
		addFallbackTofuGlyph();
	} else {
		// look up the tofu glyph information
		auto tofuGlyphInfo = glyphMap.find(glyphIndex);
		if (tofuGlyphInfo == glyphMap.end()) {
			// No tofu glyph image had been added yet.
			addGlyphToTexture(0);
			// Now the Tofu image *should* (tm) exist
			tofuGlyphInfo = glyphMap.find(glyphIndex);
		}

		if (tofuGlyphInfo != glyphMap.end()) {
			// Add the tofu glyph image for the new glyph index
			glyphMap.insert(std::pair(glyphIndex,GLTextFontCacheGlyphItem(glyphIndex, *this,  tofuGlyphInfo->second.texture, tofuGlyphInfo->second.texturePosition, tofuGlyphInfo->second.glyphMetrics,true)));
		}
	}
}

void GLTextFontCacheItem::addFallbackTofuGlyph() {
	int32_t tofuWidth = pango_font_metrics_get_approximate_char_width (fontMetrics) / PANGO_SCALE;
	// Use only 2/3 of the ascent. This is about the height of the real tofu glyph with index 0.
	int32_t tofuHeight = pango_font_metrics_get_ascent (fontMetrics) * 2 / 3 / PANGO_SCALE;

	LOG4CXX_DEBUG (logger,"" << __PRETTY_FUNCTION__<< ":");

	LOG4CXX_DEBUG (logger,"\tresolutionDpiX = " << globals->resolutionDpiX()
			<< ", approximate_char_width " << fontMetrics->approximate_char_width / PANGO_SCALE
			<< ", tofuWidth = " << tofuWidth);

	LOG4CXX_DEBUG (logger,"\tresolutionDpiY = " << globals->resolutionDpiY()
			<< ", ascent " << fontMetrics->ascent / PANGO_SCALE
			<< ", tofuHeight = " << tofuHeight);

	GLTextGlyphBBoxI glyphBox;
	GLTextFontTexture *texture = nullptr;

	for (auto textureListItem = textureList.begin();textureListItem != textureList.end(); ++textureListItem) {
		texture = textureListItem.operator ->();
		glyphBox = texture->addFallbackTofuGlyphToTexture(tofuWidth, tofuHeight);
		if (glyphBox.isValid()) {
			break;
		}
	}

	if (!glyphBox.isValid()) {
		// An invalid BBox means there was no existing texture where the glyph would fit.
		// Therefore create a new one and insert it into the list.
		auto newItem = textureList.insert(textureList.begin(),GLTextFontTexture(*this,textureSizePixel));

		texture = newItem.operator ->();
		glyphBox = texture->addFallbackTofuGlyphToTexture(tofuWidth, tofuHeight);

		LOG4CXX_DEBUG (logger,"\tAdd a new texture. Glyph validity = " << glyphBox.isValid());
	}

	if (glyphBox.isValid() && texture != nullptr) {
		// Put your own fake glyph metrics together.
		FT_Glyph_Metrics glyphMetrics {
		    tofuWidth, // FT_Pos  width;
		    tofuHeight,// FT_Pos  height;

			0,// FT_Pos  horiBearingX;
			0,// FT_Pos  horiBearingY;
			tofuWidth + 1,// FT_Pos  horiAdvance;

		    0,// FT_Pos  vertBearingX;
		    0,// FT_Pos  vertBearingY;
		    tofuHeight + 1// FT_Pos  vertAdvance;

		};
		// Add the self-drawn tofu glyph as index 0 to the map.
		glyphMap.insert(std::pair(0,GLTextFontCacheGlyphItem(0, *this, *texture, glyphBox, glyphMetrics,true)));

	}
}

void GLTextFontCacheItem::exportTextureBitmaps() {

	int i = 0;
	for (auto textureItem = textureList.begin(); textureItem != textureList.end(); ++textureItem){
		textureItem->exportTextureBitmap(i);
		++i;
	}

}


GLTextFontCache::GLTextFontCache(GLTextGlobals* globals)
 : globals {globals}
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
		GLTextFontCacheItem newCacheItem(font,globals);
		auto fontHash = newCacheItem.getFontDescHash();
		auto insRes = fontCache.insert(std::pair<guint,GLTextFontCacheItem>(fontHash,std::move(newCacheItem)));

		result = &(insRes->second);

		// Add the Tofu glyph from the start.
		result->addGlyphToTexture(0);
	}

	return result;
}

void GLTextFontCache::exportTextureBitmaps() {

	for (auto cacheItem = fontCache.begin();cacheItem != fontCache.end();++cacheItem) {
		cacheItem->second.exportTextureBitmaps();
	}
}


} /* namespace OevGLES */
