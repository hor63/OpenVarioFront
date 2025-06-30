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

#include "GLTextGlyphBBox.h"
#include "GLES/GLTexture.h"
#include "freetype/freetype.h"

namespace OevGLES {


// Forward declarations
class GLTextFontCacheItem;


/** \brief Manages a GL texture which contains the images of a number of glyphs of a font.
 *
 *   The glyphs are arranged in lines with 1 pixel space to the edges and between the glyphs
 *   Besides the actual texture which is managed by a GLTexture member the class stores the glyph
 *   positions and sizes of the current line as well as the previous line.
 *   This allows precise positioning of new glyhps above the glyphs of the previous line below.
 *
 */
class GLTextFontTexture {
public:

	using GlyphBBoxList = std::list<GLTextGlyphBBoxI>;

	/** \brief Constructor
	 *
	 * Constructs one texture of a cache item.
	 * The texture will hold multiple glyph images for the font.
	 *
	 * The texture is always square.
	 *
	 * \param cacheItem: Back reference to the owning cache item
	 * \param sizeXY: Size of the texture in x and y direction
	 */
	GLTextFontTexture(GLTextFontCacheItem& cacheItem,int32_t sizeXY);
	GLTextFontTexture(const GLTextFontTexture &other) = delete;

	/** \brief Move constructor
	 *
	 * All movable class items are moved from other to this.
	 *
	 * \param other Source object. Contents are moved to the new object here
	 */
	GLTextFontTexture(GLTextFontTexture &&other);
	virtual ~GLTextFontTexture();
	GLTextFontTexture& operator=(const GLTextFontTexture &other) = delete;
	GLTextFontTexture& operator=(GLTextFontTexture &&other) = delete;

	/** \brief Adds the image of a glyph to the texture when there is enough space.
	 *
	 * Look for enough free space for the glyph image calling addNewRectangle().
	 * If enough free space is available copy the glyph into the texture calling
	 * copyGlyphImageToTexture().
	 *
	 * \param glyphSlot Contains size information, and the glyph image itself
	 * \return Coordinates of the glyph image within the texture.
	 * Use the method isValid() to check if the glyph was added, or if the texture
	 * is full, and the glyph did not fit any more.
	 */
	GLTextGlyphBBoxI addGlyphToTexture(FT_GlyphSlot glyphSlot);

	/** \brief Draw a tofu glyph youself in the texture with the given size
	 *
	 * The emergency fallback when not even the tofu glyph (index 0) of a font
	 * cannot be rendered.
	 * The method draws a rectangle with one pixel width height and width at a free
	 * place in the texture.
	 *
	 * \param width Width of the tofu glyph in pixel
	 * \param height Height of the tofu glyph in pixel
	 * \return Coordinates of the glyph image within the texture.
	 * Use the method isValid() to check if the glyph was added, or if the texture
	 * is full, and the glyph did not fit any more.
	 */
	GLTextGlyphBBoxI addFallbackTofuGlyphToTexture(int32_t width,int32_t height);

	/** \brief Uploads glyph image data into the texture when changed
	 *
	 * Upload the glyph texture data into the GPU when the flag \ref dirty is \p true.
	 * The flag \ref dirty is false after the call.
	 *
	 */
	void syncTextureDataWithGPU();

	/** \brief Check if the texture is assumed to be full
	 *
	 * The texture is assumed to be full when the previous attempt to add a glyph
	 * failed due to lack of space.
	 * This is speed optimized but not for maximum utilization of the texture area.
	 * A particularly tall character (e.g. "J" may not fit but smaller glyphs may very well.
	 *
	 * \return True when the previous attempt to add a glyph failed due to lack of
	 * space.
	 */
	bool isFull() const {
		return full;
	}


	/** \brief Save the texture as a raw bitmap
	 *
	 * Used for debugging and analysis of the glyph placing algorithm only.
	 *
	 * Saves the texture as raw bitmap with suffix .data.
	 * The name is formed from the font face name, the font and bitmap size,
	 * and the bitmap number of the owning \ref fontCacheItem.
	 * One example of a file name is \p Noto \p Sans_12_128x128_0.data
	 *
	 * \param bitmapNumber Number of the texture within the owning \ref fontCacheItem
	 */
	void exportTextureBitmap(int bitmapNumber);

	/// \see \ref textureData
	TextureData& getTextureData () {
		return textureData;
	}

	/// \see \ref texture
	GLTexture& getTexture () {
		return texture;
	}


private:


	/// \brief The owning cache item, representing one font.
	GLTextFontCacheItem& fontCacheItem;

	/** \brief previous line of glyphs
	 *
	 * The previous line of glyphs is used to the determine the bottom of the glyphs
	 * which are added to \ref currentGlyphLine.
	 * The bottom of a new glyph is determined by the top of the glyphs in this list
	 * which are actually below the new glyph to optimize space usage.
	 *
	 * Initially this list is of course empty, i.e. any new glyph will be placed
	 * at the bottom of the texture.
	 *
	 */
	GlyphBBoxList previousGlyphLine;

	/** \ brief The current (topmost) line of glyphs
	 *
	 * New glyphs are added right to the last glyph in the list.
	 */
	GlyphBBoxList currentGlyphLine;

	/// \brief The GL texture which is eventually used to render the glyph images to the screen
	GLTexture texture;

	/** \brief The data object which manages the texture data for \ref texture.
	 *
	 * I am copying the glyph images directly into the memory buffer provided by
	 * this member.
	 */
	TextureData textureData;

	/// \brief When a glyph does not fit into the texture I assume it is full.
	///
	/// When true any attempt to adding a glyph is immediately rejected.
	bool full = false;
	/// \brief Determines if the local texture buffer is not synchronized with the GL texture data.
	///
	/// If true the data in \ref textureData must be uploaded into \ref texture before rendering
	/// to the screen.
	bool dirty = true;

	/** \brief Determine how low a glyph can be positioned when \ref previousGlyphLine does not determine a lower limit.
	 *
	 * It can happen that there is a gap left between the rightmost glyph in \ref previousGlyphLine and the right edge of the texture.
	 * When a narrow glyph is placed very close to the right edge it may be right of the rightmost glyph in \ref previousGlyphLine.
	 * Therefore this glyph would be placed at the very bottom of the texture thus overwriting a glyph lying there.
	 * This value here ensures that a glyph which "falls though the cracks" does not drop to the bottom overwriting a random glyph at
	 * the bottom right of the texture.
	 *
	 */
	int32_t topPosPreviousLines = 1;

	/// count the rows being filled for diagnostics and debugging purposes.
	int rowNum = 0;

	/** \brief Copy the rendered glyph image from the freetype bitmap to the texture
	 *
	 * Sets \ref dirty true.
	 *
	 * \param glyphCoord Destination coordinates where the glyph image is to be drawn.
	 * \param glyphSlot The Freetype glyph slot with the loaded glyph and the rendered glyph image
	 */
	void copyGlyphImageToTexture(GLTextGlyphBBoxI const glyphCoord, FT_GlyphSlot glyphSlot );

	/** \brief Draws a rectangle with a line one pixel wide, i.e. the Tofu.
	 *
	 * The fallback of fallbacks. When even the standard tofu glyph of a font with
	 * index 0 cannot be loaded I am frawing this emergency glyph my self in into
	 * the texture bitmap.
	 *
	 * Sets \ref dirty true.
	 *
	 * \param glyphCoord Corners of the tofo box to draw.
	 */
	void drawTofuGlyphImageToTexture(GLTextGlyphBBoxI const glyphCoord);

	/** \brief Moves the content of \ref currentGlyphLine to \ref previousGlyphLine.
	 *
	 * First re-calculates \ref topPosPreviousLines for \ref previousGlyphLine before its content is being overwritten.
	 * Then moves the content of \ref currentGlyphLine to \ref previousGlyphLine.
	 * Finally clears \ref currentGlyphLine.
	 */
	void startNewGlyphLine();

	/** \brief Add a new rectangle in the texture and return its coordinates
	 *
	 *	If the rectangle does not fit into the texture \ref full is being set \p true.
	 *
	 * \param width Width in pixel
	 * \param height Height in pixel
	 * \return The coordinates (bounding box) of the rectangle in the texture.
	 *   Check validity of the return value. When it is invalid no space could be
	 *   found in this texture for a rectangle of the given size.
	 */
	GLTextGlyphBBoxI addNewRectangle(int32_t width, int32_t height);

};



} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTFONTTEXTURE_H_ */
