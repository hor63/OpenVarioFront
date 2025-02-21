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
#include <vector>
#include <unordered_map>

#include "GLTextGlobals.h"
#include "GLPrograms/GLProgTextTexture.h"
#include "Renderers/RendererBase.h"



// forward declarations for private types within the module.
extern "C" {
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;
}

namespace OevGLES {

class GLTextRenderer: public RendererBase  {
public:
	enum RenderMode {
		RENDER_GLYPHS, /**< \brief Render glyphs to the screen. If needed add missing glyphs to the font bitmaps.
						*
						* This mode will always work. However it can be highly inefficient because every glyph
						* which has not been rendered before must be added to a texture, and the texture been uploaded
						* into GPU memory again.\n
						* It is therefore much more efficient to render the expected glyphs once in \ref BUILD_GLYPH_CACHE_ONLY mode
						* at the start.
						*
						*/

		BUILD_GLYPH_CACHE_ONLY,	/**< \brief Only build the font textures but do not render the text to the display.
								 *
								 * This is much more efficient because in render mode the texture must be
								 * uploaded into the GPU for each added character immediately.
								 * In BUILD_GLYPH_CACHE_ONLY mode the textures are being build up in the client memory only.
								 * When switching to Render mode all modified textures are being uploaded into GPU memory.
								 *
								 * Please note that caching glyphs in this mode applies for the given font face its attributes and size only.
								 */
	};

	/// \brief vertex buffer structure of one vertex of a glyph box
	struct GlGlyphCornerVertexStruct {
		GLfloat vertexPosition [4];
		GLfloat texturePosition [2];
	};

	/** \brief vertex buffer structure of a glyph box with two triangles
	 *
	 * The vertexes are:
	 * a. First triangle
	 * 	0. Top left
	 * 	1. Bottom left
	 * 	2. Bottom right
	 * b. Second triangle
	 * 	3. Top Left
	 * 	4. Bottom right
	 * 	5. Top right
	 *
	 * 	This forms two triangles with vertexes in counter-clock oder.
	 */
	struct GlGlyphVertexStruct {
		GlGlyphCornerVertexStruct tri1TopLeft;
		GlGlyphCornerVertexStruct tri1BottomLeft;
		GlGlyphCornerVertexStruct tri1BottomRight;
		GlGlyphCornerVertexStruct tri2TopLeft;
		GlGlyphCornerVertexStruct tri2BottomRight;
		GlGlyphCornerVertexStruct tri2TopRight;
	};


	struct VertexBufferPerTexture {
		std::vector<GlGlyphVertexStruct> vertexVector;

		GLTextFontTexture& fontTexture;

		GLuint vertexBufferHandle;

		VertexBufferPerTexture(GLTextFontTexture& fontTexture, size_t vectorReserveSize)
		: fontTexture{fontTexture},
		  vertexBufferHandle{0}
		{
			vertexVector.reserve(vectorReserveSize);
		}

		VertexBufferPerTexture(VertexBufferPerTexture const& source) = delete;

		VertexBufferPerTexture(VertexBufferPerTexture&& source)
		: vertexVector {std::move(source.vertexVector)},
		  fontTexture{source.fontTexture},
		  vertexBufferHandle{source.vertexBufferHandle}
		{}

		VertexBufferPerTexture& operator = (VertexBufferPerTexture const& source) = delete;
		VertexBufferPerTexture& operator = (VertexBufferPerTexture&& source) = delete;

	};


	GLTextRenderer(GLTextGlobalsWeakPtr glob);
	virtual ~GLTextRenderer();

	void setText (const std::string& str);

	const std::string& getText() const {
		return text;
	}

	/** \brief Set the font size
	 *
	 * \param sizePoints Font size in points (what else 🙃)
	 */
	void setFontSize (double sizePoints);

	/** \brief Set the font name or list of font names to choose from
	 *
	 * \param fontNames Name of the font family, or comma separated list of font families to choose from
	 * \see [Pango.FontDescription.set_family](https://docs.gtk.org/Pango/method.FontDescription.set_family.html)
	 */
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
	 * \param x: the X position of the left of the layout (in pixels)
	 * \param y: the Y position of the top of the layout (in pixels)
	 * \param renderMode: Whether to only build up the glyph cache or actually render a text to the screen
	 *
	 */
	void
	renderLayout (
			 int          x = 0,
			 int          y = 0,
			 RenderMode   renderMode = RENDER_GLYPHS);

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
	 * \param x: the X position of the left of the layout (in Pango units)
	 * \param y: the Y position of the top of the layout (in Pango units)
	 * \param renderMode: Whether to only build up the glyph cache or actually render a text to the screen
	 *
	 */
	void
	renderLayoutSubpixel (
					  int          x = 0,
					  int          y = 0,
						 RenderMode   renderMode = RENDER_GLYPHS);

	/** \brief Callback from a Pango.Layout rendering text
	 *
	 * \param font Pango font to render a glyph.
	 * \param glyph The glyph index in \p font.
	 * \param x Position of the glyph (can be fractions of a pixel)
	 * \param y Position of the glyph (can be fractions of a pixel)
	 *
	 *  \see [Pango.Renderer.draw_glyph](https://docs.gtk.org/Pango/vfunc.Renderer.draw_glyph.html)
	 */
	void draw_glyph (
			PangoFont           *font,
			PangoGlyph          glyph,
			double              x,
			double              y);

	/// \see RendererBase::setupVertexBuffers()
	virtual void setupVertexBuffers () override;

	/// \see RendererBase::draw()
	virtual void draw(
				OevGLES::Mat4 const &modelMatrix,
				OevGLES::Mat4 const &viewMatrix,
				OevGLES::Mat4 const &ProjMatrix,
				OevGLES::Mat4 const &MVMatrix,
				OevGLES::Mat4 const &MVPMatrix,
				OevGLES::Vec3 const &lightDir,
				OevGLES::Vec4 const &lightColor,
				OevGLES::Vec4 const &ambientLightColor
				) override;

private:

	std::string text;
	std::string fonts;

	RenderMode renderMode = RENDER_GLYPHS;

	PangoLayout* pangoLayout = nullptr;
	PangoFontDescription* fontDescr = nullptr;
	PangoGLTextRenderer* pangoTextRenderer;

	PangoFont* previousFont = nullptr;
	GLTextFontCacheItem* previousFontCacheItem = nullptr;

	GLTextGlobalsWeakPtr globals;

	GLProgTextTexture* glProgram = nullptr;
	GLuint vertexBufferHandle = 0;

	/// \brief Size to reserve the vertex vectors
	///
	/// Is being set each time in \ref renderLayoutSubpixel()
	gint vertexVectorReserveSize = 1;

	/// \brief The rectangle which circumfences the text box as drawn.
	PangoRectangle inkRect = {-1,-1,-1,-1};
	PangoRectangle logicalRect = {-1,-1,-1,-1};

	std::unordered_map<GLuint,VertexBufferPerTexture> vertextBufferPerTextureMap;

}; // class GLTextRenderer

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTRENDERER_H_ */
