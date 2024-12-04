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

#include "GLTextGlobals.h"
#include "GLTextFontTexture.h"

#include FT_BITMAP_H

namespace OevGLES {


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


GLTextFontTexture::GLTextFontTexture(GLTextFontCacheItem& cacheItem,int32_t sizeXY)
:fontCacheItem{cacheItem},
 textureData (sizeXY, sizeXY, TextureData::Luminance, TextureData::Byte)
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextFontTexture");
	}
#endif

	LOG4CXX_DEBUG (logger,__PRETTY_FUNCTION__ << "Create font texture with square pixel size " << sizeXY);

}

GLTextFontTexture::~GLTextFontTexture() {
}

GLTextFontTexture::GLTextFontTexture(GLTextFontTexture &&other)
:fontCacheItem {other.fontCacheItem},
 previousGlyphLine {std::move(other.previousGlyphLine)},
 currentGlyphLine {std::move(other.currentGlyphLine)},
 texture {std::move(other.texture)},
 textureData {std::move(other.textureData)},
 full {other.full},
 dirty {other.dirty},
 rowNum {other.rowNum}
{
}

GLTextGlyphBBox GLTextFontTexture::addGlyphToTexture(FT_GlyphSlot glyphSlot) {
	GLTextGlyphBBox ret; // is initially invalid.
	FT_Bitmap &glyphBitmap = glyphSlot->bitmap;
	int32_t textureBoxWidth = glyphBitmap.width + 1;
	int32_t textureBoxHeight = glyphBitmap.rows + 1;

	if (full) {
		LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__ << ": Texture is already full.");
		return ret;
	}

	LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__ << ": Add glyph with size "
			<< glyphBitmap.width
			<< 'x' << glyphBitmap.rows
			);

	ret = addNewRectangle(textureBoxWidth,textureBoxHeight);

	if (ret.isValid()) {
		// The glyph fits into the texture.

		// Set the return to the glyph coordinates within the texture.
		// ret contains the box within the texture.
		// the bounding box of the glyph is one pixel smaller.

		ret.xRight --;
		ret.yTop --;

		LOG4CXX_DEBUG(logger,"\tFound a place for the glyph at "
				<< ret.xLeft << "," << ret.yBottom
				<< "  " << ret.xRight << "," << ret.yTop
				);

		copyGlyphImageToTexture(ret,glyphSlot);

	} else {
		LOG4CXX_DEBUG(logger,"\tGlyph does not fit. The texture is full.");
	}

	return ret;
}

GLTextGlyphBBox GLTextFontTexture::addFallbackTofuGlyphToTexture(int32_t width,int32_t height) {
	GLTextGlyphBBox ret; // is initially invalid.

	ret = addNewRectangle(width + 1,height + 1);

	if (ret.isValid()) {
		// The glyph fits into the texture.

		// Set the return to the glyph coordinates within the texture.
		// ret contains the box within the texture.
		// the bounding box of the glyph is one pixel smaller.

		ret.xRight --;
		ret.yTop --;

		LOG4CXX_DEBUG(logger,"\tFound a place for the tofu rectangle at "
				<< ret.xLeft << "," << ret.yBottom
				<< "  " << ret.xRight << "," << ret.yTop
				);

		drawTofuGlyphImageToTexture(ret);

	} else {
		LOG4CXX_DEBUG(logger,"\tGlyph does not fit. The texture is full.");
	}


	return ret;
}
void GLTextFontTexture::startNewGlyphLine() {

	// look for the top of any glyphs in previousGlyphLine in the gap between the rightmost glyph
	// in currentGlyphLine and the right edge of the texture.
	int32_t leftGapEdge = 0;

	if (!currentGlyphLine.empty()) {
		auto rightGlyph = currentGlyphLine.back();
		leftGapEdge = rightGlyph.xRight;
	}

	LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__
			<< ": leftGapEdge = " << leftGapEdge);

	if (!previousGlyphLine.empty()) {
		auto floorGlyph = previousGlyphLine.end();
		--floorGlyph;
		for (;;) {

			if (floorGlyph->xRight > leftGapEdge) {
				LOG4CXX_DEBUG(logger,"\tGlyph is in the gap with xRight = " << floorGlyph->xRight);
				if (floorGlyph->yTop >= topPosPreviousLines) {
					topPosPreviousLines = floorGlyph->yTop + 1;
					LOG4CXX_DEBUG(logger,"\tNew topPosPreviousLines = " << topPosPreviousLines);
				}
			} else {
				// This glyph is left to the gap.
				LOG4CXX_DEBUG(logger,"\tGlyph is left of the gap with xRight = " << floorGlyph->xRight);
				break;
			}

			if (floorGlyph == previousGlyphLine.begin()) {
				break;
			}

			--floorGlyph;
		}
	}

	previousGlyphLine.clear();
	previousGlyphLine = std::move(currentGlyphLine);
	currentGlyphLine.clear();
	++rowNum;

}

void GLTextFontTexture::copyGlyphImageToTexture(GLTextGlyphBBox const glyphCoord, FT_GlyphSlot glyphSlot ) {
	FT_Bitmap myBitmap;
	FT_Bitmap* bitmapPtr ;

	FT_Bitmap_Init( &myBitmap);

	if (glyphSlot->bitmap.pixel_mode != FT_PIXEL_MODE_GRAY || glyphSlot->bitmap.num_grays != 256) {
		FT_Bitmap_Convert(glyphSlot->library, &glyphSlot->bitmap, &myBitmap, 1);
		bitmapPtr = &myBitmap;
	} else {
		bitmapPtr = &glyphSlot->bitmap;
	}

	int numLines = bitmapPtr->rows;
	int numColumns = bitmapPtr->width;

	// Use negative bitmap pitch because a GL texture goes from bottom to top
	// but a positive Freetype pitch indicates top to bottom
	int strideSource = -bitmapPtr->pitch;
	int strideDest = textureData.getWidth();

	uint8_t* source;
	if (strideSource < 0) {
		source = reinterpret_cast<uint8_t*>(bitmapPtr->buffer) - (strideSource * (numLines-1));
	} else {
		source = reinterpret_cast<uint8_t*>(bitmapPtr->buffer);
	}

	uint8_t* dest = reinterpret_cast<uint8_t*>(textureData.getDataPtr())
			+ glyphCoord.yBottom * strideDest
			+ glyphCoord.xLeft;

	// Now copy the stuff.
	if (glyphSlot->bitmap.num_grays == 256) {
		for (int i = 0;i < numLines; ++i) {
			for (int k = 0; k < numColumns; ++k) {
				dest[k] = source[k];
			}
			dest += strideDest;
			source += strideSource;
		}
	} else {
		float normalizeFactor = 255.0f / static_cast<float>(glyphSlot->bitmap.num_grays);
		for (int i = 0;i < numLines; ++i) {
			for (int k = 0; k < numColumns; ++k) {
				auto val = source[k] * normalizeFactor;
				if (val > 255.0f){
					val = 255.0f;
				}
				dest[k] = static_cast<uint8_t>(val);
			}
			dest += strideDest;
			source += strideSource;
		}

	}

	dirty = true;

	FT_Bitmap_Done(glyphSlot->library, &myBitmap);
}

void GLTextFontTexture::drawTofuGlyphImageToTexture(GLTextGlyphBBox const glyphCoord) {

	int numLines = glyphCoord.height();
	int numColumns = glyphCoord.width();

	int strideDest = textureData.getWidth();

	uint8_t* dest = reinterpret_cast<uint8_t*>(textureData.getDataPtr())
			+ glyphCoord.yBottom * strideDest
			+ glyphCoord.xLeft;

	// Draw the bottom line
	for (int k = 0; k < numColumns; ++k) {
		dest[k] = 0xff;
	}
	dest += strideDest;

	// Draw the left and right vertical lines
	for (int i = 1;i < numLines - 1; ++i) {
		*dest = 0xff;
		dest[numColumns - 1] = 0xff;
		dest += strideDest;
	}
	// Draw the top line
	for (int k = 0; k < numColumns; ++k) {
		dest[k] = 0xff;
	}

	dirty = true;

}

GLTextGlyphBBox GLTextFontTexture::addNewRectangle(int32_t width, int32_t height) {
	GLTextGlyphBBox ret;
	int32_t leftPos = 1;
	int32_t bottomPos = topPosPreviousLines;

	LOG4CXX_DEBUG(logger,"" << __PRETTY_FUNCTION__
			<< ": width = " << width
			<< ", height = " << height);

	if (full) {
		LOG4CXX_DEBUG(logger,"\tTexture is full. Return immediately.");
		return ret;
	}

	// determine the left position in the current line when there is anything in it.
	if (!currentGlyphLine.empty()) {
		auto lastItem = currentGlyphLine.back();
		leftPos = lastItem.xRight;
	}

	int32_t rightPos = leftPos + width;

	if (rightPos > textureData.getWidth()) {
		// The current line is full. Start a new line.
		startNewGlyphLine();

		LOG4CXX_DEBUG(logger,"\trightPos = " << rightPos
				<< " is right off the texture at " << textureData.getWidth()
				<< ". Start a new line."
				);

		// now reset the horizontal positions back to the start of the new line.
		leftPos = 1;
		rightPos = 1 + width;
	}

	LOG4CXX_DEBUG(logger,"\tleftPos = " << leftPos
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
		if (prevLineIter->xLeft >= rightPos) {
			LOG4CXX_DEBUG(logger,"\tThis glyph is to the right, xLeft = "
					<< prevLineIter->xLeft
					<< ". Leave the loops.");

			break;
		}
		// Now I have a glyph under me
		LOG4CXX_DEBUG(logger,"\tGot a glyph under me. x = "
				<< prevLineIter->xLeft << "," << prevLineIter->xRight
				<< ", top = " << prevLineIter->yTop
				);

		if (prevLineIter->yTop > bottomPos) {
			bottomPos = prevLineIter->yTop;
		}

		++prevLineIter;
	}

	int32_t topPos = bottomPos + height;

	if (topPos <= textureData.getHeight()) {
		// The glyph fits into the texture.
		// Set the return to the glyph coordinates within the texture.
		ret.xLeft = leftPos;
		ret.xRight = rightPos;
		ret.yBottom = bottomPos;
		ret.yTop = topPos;

		// Store the new texture BBox in the current line.
		currentGlyphLine.push_back(ret);

		LOG4CXX_DEBUG(logger,"\tFound a place for the glyph at "
				<< ret.xLeft << "," << ret.yBottom
				<< "  " << ret.xRight << "," << ret.yTop
				<< ", texture BBox at "
				<< leftPos << "," << bottomPos
				<< " " << rightPos << "," << topPos
				);

	} else {

		full = true;
		LOG4CXX_DEBUG(logger,"\tCould not fit the glyph into the texture."
				<< " topPos = " << topPos
				<< "is higher then the texture height = " << textureData.getHeight());
	}

	return ret;
}

void GLTextFontTexture::exportTextureBitmap(int bitmapNumber) {

	std::ostringstream str;
	std::FILE* outFile;

	str << pango_font_description_get_family(fontCacheItem.getFontDesc())
			<< "_" << (static_cast<double>(pango_font_description_get_size(fontCacheItem.getFontDesc())) / PANGO_SCALE)
			<< "_" << textureData.getWidth() << "x" << textureData.getHeight()
			<< "_" << bitmapNumber
			<< ".data";

	outFile = std::fopen(str.str().c_str(), "wb");

	if (outFile) {
		std::fwrite (textureData.getDataPtr(), sizeof(uint8_t), textureData.getDataBufferLength(), outFile);
		std::fclose(outFile);
		outFile = nullptr;
	}

}

} /* namespace OevGLES */
