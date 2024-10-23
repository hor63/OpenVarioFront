/*
 * GLTextRenderer.h
 *
 *  Created on: Aug 10, 2024
 *      Author: hor
 *
 *  Top class for rendering a text.
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

#ifndef GLTEXTRENDER_GLTEXTRENDERER_H_
#define GLTEXTRENDER_GLTEXTRENDERER_H_

#include <string>

#include "GLTextGlobals.h"


// forward declarations for private types within the module.
extern "C" {
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;
}

namespace OevGLES {

class GLTextRenderer {
public:

	GLTextRenderer(GLTextGlobals& glob);
	virtual ~GLTextRenderer();

	void setText (const std::string& str);

	const std::string& getText() const {
		return text;
	}

	void setFontSize (double sizePoints);
	void setFonts(std::string fontNames);

	double getFontSize();
	const std::string& getFonts() {
		return fonts;
	}

	PangoLayout* getPangoLayout() {
		return pangoLayout;
	}

	/** \brief Render the text into the GLES context
	 *
	 * The text must be set with \ref setText() before rendering.<br>
	 * Text attributes must be set with the respective methods of this class before rendering.
	 *
	 * The text is being rendered into a \p PangoLayout with a special GL renderer object
	 *
	 * @x: the X position of the left of the layout (in pixels)
	 * @y: the Y position of the top of the layout (in pixels)
	 *
	 */
	void
	renderLayout (
			 int          x = 0,
			 int          y = 0);

	/** \brief Render the text into the GLES context
	 *
	 * Renders a `PangoLayout` onto an OpenGL ES2 context, with he
	 * location specified in fixed-point Pango units rather than
	 * pixels.
	 *
	 * (Using this will avoid extra inaccuracies from rounding
	 * to integer pixels multiple times, even if the final glyph
	 * positions are integers.)
	 *
	 * The text must be set with \ref setText() before rendering.<br>
	 * Text attributes must be set with the respective methods of this class before rendering.
	 *
	 * The text is being rendered into a \p PangoLayout with a special GL renderer object
	 *
	 * @x: the X position of the left of the layout (in Pango units)
	 * @y: the Y position of the top of the layout (in Pango units)
	 *
	 *
	 */
	void
	renderLayoutSubpixel (
					  int          x = 0,
					  int          y = 0);

	void draw_glyph (
			PangoFont           *font,
			PangoGlyph          glyph,
			double              x,
			double              y);

private:


	enum RenderMode {
		RENDER_GLYPHS, /**< \brief Render glyphs to the screen. If needed add missing glyphs to the font bitmaps.
						*
						* This mode will always work. However it can be highly inefficient because every glyph
						* which has not been rendered before must be added to a texture, and the texture been uploaded
						* into GPU memory again.\n
						* It is therefore much more efficient to render the expected glyphs once in \ref BUILD_GLYPH_CACHE_ONLY mode
						* at the start.
						*

		BUILD_GLYPH_CACHE_ONLY	/**< \brief Only build the font textures but do not render the text to the display.
								 *
								 * This is much more efficient because in render mode the texture must be
								 * uploaded into the GPU for each added character immediately.
								 * In BUILD_GLYPH_CACHE_ONLY mode the textures are being build up in the client memory only.
								 * When switching to Render mode all modified textures are being uploaded into GPU memory.
								 *
								 * Please note that caching glyphs in this mode applies for the given font face its attributes and size only.
								 */
	};

	std::string text;
	std::string fonts;

	PangoLayout* pangoLayout = nullptr;
	PangoFontDescription* fontDescr = nullptr;
	PangoGLTextRenderer* pangoTextRenderer;

	PangoFont* previousFont = nullptr;
	GLTextFontCacheItem* previousCacheItem = nullptr;

	GLTextGlobals& globals;
};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTRENDERER_H_ */
