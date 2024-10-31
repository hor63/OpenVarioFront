/*
 * GLTextFontTexture.cpp
 *
 *  Created on: Oct 16, 2024
 *      Author: hor
 *
 *   Manages a GL texture which contains the images of a number of glyphs of a font.
 *   The glyphs are arranged in lines with 1 pixel space to the edges and 2 pixels between the glyphs
 *   Besides the actual texture which is managed by a GLTexture member the class stores the glyph
 *   positions and sizes of the current line as well as the previous line.
 *   This allows precise positioning of new glyhps above the glyphs of the previous line below.
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

#include "GLTextFontTexture.h"

namespace OevGLES {


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLTextFontTexture::GLTextFontTexture(GLTextFontCacheItem* cacheItem,int32_t sizeXY)
:fontCacheItem{cacheItem},
 textureData (textureDimension, textureDimension, TextureData::Luminance, TextureData::Byte)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextFontTexture");
	}
#endif

}

GLTextFontTexture::~GLTextFontTexture() {
}

GLTextFontTexture::GLTextFontTexture(GLTextFontTexture &&other)
:fontCacheItem {other.fontCacheItem},
 previousGlyphLine {std::move(previousGlyphLine)},
 currentGlyphLine {std::move(currentGlyphLine)},
 texture {std::move(other.texture)},
 textureData {std::move(other.textureData)}
{
}

GLTextFontTexture& GLTextFontTexture::operator = (GLTextFontTexture &&other)
{

	fontCacheItem  = other.fontCacheItem;
	previousGlyphLine = std::move(previousGlyphLine);
	currentGlyphLine = std::move(currentGlyphLine);
	texture = std::move(other.texture);
	textureData = std::move(other.textureData);

	other.fontCacheItem = nullptr;

	return (*this);
}

GLTextGlyphBBox GLTextFontTexture::addGlyphToTexture(FT_Bitmap &glyphBitmap) {
	GLTextGlyphBBox ret; // is initially invalid.
	int32_t textureBoxWidth = glyphBitmap.width + 2;
	int32_t textureBoxHeight = glyphBitmap.rows + 2;
	int32_t leftPos = 0;
	int32_t rightPos;
	int32_t bottomPos = 0;
	int32_t topPos = textureBoxHeight;

	if (full) {
		LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__ << ": Texture is already full.");
		return ret;
	}

	// determine the left position in the current line when there is anything in it.
	if (!currentGlyphLine.empty()) {
		auto lastItem = currentGlyphLine.back();
		leftPos = lastItem.xRight + 1;
	}

	rightPos = leftPos + textureBoxWidth;

	if (rightPos >= textureData.getWidth()) {
		// The current line is full. Start a new line.
		previousGlyphLine.clear();
		previousGlyphLine = std::move(currentGlyphLine);
		currentGlyphLine.clear();
		++rowNum;

		LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__ << ": rightPos = " << rightPos
				<< " is right off the texture at " << textureData.getWidth()
				<< ". Start a new line."
				);

		// now reset the horizontal positions back to the start of the new line.
		leftPos = 0;
		rightPos = leftPos + textureBoxWidth;
	}

	LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__ << ": leftPos = " << leftPos
			<< ", rightPos = " << rightPos
			<< " in row " << rowNum);

	// now run through the previous line to find the first glyph which is under the current glyph
	// to determine the height in the texture
	auto prevLineIter = previousGlyphLine.begin();
	while (prevLineIter != previousGlyphLine.end()) {
		// Look for the first glyph which is under the current one.
		if (prevLineIter->xRight >= leftPos) {
			// I found the first glyph in the previous line under the current one
			break;
		}
		LOG4CXX_DEBUG(logger,"\tThis glyph is still to the left, its xRight = "
				<< prevLineIter->xRight);

		++prevLineIter;
	}

	while (prevLineIter != previousGlyphLine.end()) {
		if (prevLineIter->xLeft > rightPos) {
			LOG4CXX_DEBUG(logger,"\tThis glyph is to the right, xLeft = "
					<< prevLineIter->xLeft
					<< ". Leave the loops.");

			break;
		}
		// Now I have a glyph under me
		LOG4CXX_DEBUG(logger,"\tGot a glyph under me. xPos = "
				<< prevLineIter->xLeft << ", " << prevLineIter->xLeft
				<< ", top = " << prevLineIter->yTop
				);

		if (prevLineIter->yTop >= bottomPos) {
			bottomPos = prevLineIter->yTop + 1;
		}

		++prevLineIter;
	}

	topPos = bottomPos + textureBoxHeight;

	if (topPos < textureData.getHeight()) {
		// The glyph fits into the texture.
		// Set the return to the glyph coordinates within the texture.
		ret.xLeft = leftPos + 1;
		ret.xRight = rightPos -1;
		ret.yBottom = bottomPos + 1;
		ret.yTop = topPos - 1;
		LOG4CXX_DEBUG(logger,"\tFound a place for the glyph at "
				<< ret.xLeft << "," << ret.yBottom
				<< "  " << ret.xRight << "," << ret.yTop
				);

		// Store the new texture BBox in the current line.
		currentGlyphLine.push_back(GLTextGlyphBBox(leftPos, bottomPos, rightPos, topPos));

		dirty = true;
	} else {
		LOG4CXX_DEBUG(logger,"\tGlyph does not fit. TopPos " << topPos
				<< " is above the texture height " << textureData.getHeight()
				<< ". Declare the texture full."
				);
		full = true;
	}

	return ret;
}

} /* namespace OevGLES */
