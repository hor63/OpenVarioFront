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

#include <GLES2/gl2.h>
#include <cstddef>
#include <string>
#include <utility>
#include <vector>
#include <unordered_map>

#include "GLES/GLFramework.h"
#include "GLES/GLObjectWrappers.h"
#include "GLTextGlobals.h"
#include "GLPrograms/GLProgTextTexture.h"
#include "GLPrograms/GLProgDiffuseLight.h"
#include "Renderers/RenderContext.h"
#include "Renderers/RendererBase.h"
#include "pango/pango-types.h"



// forward declarations for private types within the module.
extern "C" {
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;
}

/// \brief Used for sensible shift values to calculate hashes
static constexpr int NumBitsSizeT = sizeof(size_t) * 8;
static constexpr std::size_t AllOnesSizeT = ~static_cast<size_t>(0U);

namespace OevGLES {
	
struct VertexBufferKey {

	/** \brief Color explicitly set, e.g. by attributed text with markups 
	 *
	 * The \p staticColor is used when \ref sharedDefaultColor is empty. 
	 */
	Vec4 staticColor = {1.0f,1.0f,1.0f,1.0f};

	/**
	 * A shared color from the \ref RenderContext.
	 * When it is not empty it takes precedence over \ref staticColor  
	 */		
	Vec4ShPtr sharedDefaultColor;
	
	/**
	 * \brief The raw GL handle of the texture holding the glyph images  
	 */
	 GLuint textureHandle = 0U;

	 /** \brief The hash value of this key
	  *
	  * The hash value is cached because it not trivial to compute.
	  * A value with all bits set should(tm) never be result of the hash calculation
	  */		 
	 mutable std::size_t hashValue = AllOnesSizeT;
	 
	 bool operator == (VertexBufferKey const & comp) const noexcept {
		if (!sharedDefaultColor) {
			return textureHandle == comp.textureHandle &&
				staticColor == comp.staticColor; 
		}
		
		return textureHandle == comp.textureHandle && 
			sharedDefaultColor.get() == comp.sharedDefaultColor.get();
		
	 }
}; // struct VertexBufferKey

} // namespace OevGLES

template <>
struct std::hash<OevGLES::VertexBufferKey>{
	std::size_t operator()(const OevGLES::VertexBufferKey& k) const noexcept;
};

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

	static constexpr std::size_t vertextPositionArrayLen = 4;
	static constexpr std::size_t texturePositionArrayLen = 2;

	using SingleVertexArr = GLfloat[vertextPositionArrayLen];
	
	/// \brief vertex buffer structure of one vertex of a glyph box
	struct GlGlyphCornerVertexStruct {
		SingleVertexArr vertexPosition;
		SingleVertexArr texturePosition;
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

	struct GlRectVertextStruct {
		SingleVertexArr tri1TopLeft;
		SingleVertexArr tri1BottomLeft;
		SingleVertexArr tri1BottomRight;
		SingleVertexArr tri2TopLeft;
		SingleVertexArr tri2BottomRight;
		SingleVertexArr tri2TopRight;
	};

	struct GlRectSizeStruct {
		GLfloat right = 1;
		GLfloat top =1;
		GLfloat width = 1;
		GLfloat height =1;
	};
	
	class VertexBufferPerTexture {
	public:
		std::vector<GlGlyphVertexStruct> vertexVector;

		GLTextFontTexture& fontTexture;

		GLBufferObject vertexBufferHandle;
		GLVertexArrayObject vertexArrayHandle;

		/// 3 vertexes per triangle, 6 vertexes per rectangular glyph
		GLsizei numVertexes;

		VertexBufferPerTexture() = delete;

		VertexBufferPerTexture(
			RenderContextSharedPtr const &contextPtr,
			GLTextFontTexture& fontTexture,
			std::size_t vectorReserveSize)
		: context{contextPtr},
		  fontTexture{fontTexture},
		  vertexBufferHandle{false},
		  numVertexes{0}
		{
			vertexVector.reserve(vectorReserveSize);
		}

		VertexBufferPerTexture(VertexBufferPerTexture const& source) = delete;

		VertexBufferPerTexture(VertexBufferPerTexture&& source) = default;

		VertexBufferPerTexture& operator = (VertexBufferPerTexture const& source) = delete;
		VertexBufferPerTexture& operator = (VertexBufferPerTexture&& source) = delete;
		
		~VertexBufferPerTexture();
	private:
	
		RenderContextSharedPtr context;
	
	}; // class VertexBufferPerTexture

	class VertexBufferForTrapezoids {
	public:
		std::vector<GlRectVertextStruct> vertexVector;

		GLBufferObject vertexBufferHandle;
		GLVertexArrayObject vertexArrayHandle;

		/// 3 vertexes per triangle, 6 vertexes per trapezoid
		GLsizei numVertexes;

		VertexBufferForTrapezoids() = delete;

		VertexBufferForTrapezoids(
			RenderContextSharedPtr const &contextPtr,
			std::size_t vectorReserveSize)
		: context{contextPtr},
		  vertexBufferHandle{false},
		  numVertexes{0}
		{
			vertexVector.reserve(vectorReserveSize);
		}

		VertexBufferForTrapezoids(VertexBufferForTrapezoids const& source) = delete;

		VertexBufferForTrapezoids(VertexBufferForTrapezoids&& source) = default;

		VertexBufferForTrapezoids& operator = (VertexBufferForTrapezoids const& source) = delete;
		VertexBufferForTrapezoids& operator = (VertexBufferForTrapezoids&& source) = delete;
		
		~VertexBufferForTrapezoids();

	private:

		RenderContextSharedPtr context;

	}; // class VertexBufferForTrapezoids
	
	GLTextRenderer(
		RenderContextSharedPtr const &context);
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

	/// \brief Set the text box width in screen pixels
	void setWidth (int width) {
		setWidthSubpixel(width * PANGO_SCALE);
	}

	/// \brief Set the text width in Pango scale (1/1024 pixel)
	void setWidthSubpixel(int width);

	/// \brief Set the text box height in screen pixels
	void setHeight (int height) {
		setHeightSubpixel(height * PANGO_SCALE);
	}

	/// \brief Set the text width in Pango scale (1/1024 pixel)
	void setHeightSubpixel(int height);

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
	void drawGlyph(PangoFont *font, PangoGlyph glyph, double x, double y);

	void drawTrapzoid(PangoRenderPart part, double y1, double x11, double x21,
					  double y2, double x12, double x22);

	/// \see RendererBase::setupVertexBuffers()
	virtual void setupVertexBuffers () override;

	/// \see RendererBase::draw()
	virtual void draw(RenderStandardUniforms const &stdUniformData) override;
	
	GlRectSizeStruct getTextBoxSize () {return textBoxSize;}
	
	bool isDrawBackground() const {
		return drawBackground;
	}

	void setDrawBackground(bool drawBackground = true) {
		this->drawBackground = drawBackground;
	}

	const Vec4ShPtr& getTextColor() const {
		return textColorPtr;
	}

	void setTextColor(Vec4ShPtr const &textColor) {
		this->textColorPtr = textColor;
	}

	const Vec4ShPtr& getBackgroundColor() const {
		return backgroundColorPtr;
	}

	void setBackgroundColor(Vec4ShPtr const &backgroundColor) {
		this->backgroundColorPtr = backgroundColor;
	}

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

	GLProgTextTexture* glGlyphProgram = nullptr;

	GLProgDiffuseLight* glTextBackgroundProgram = nullptr;
	GLBufferObject vertexBufferHandleTextBackground;
	GLVertexArrayObject vertexArrayHandleTextBackground;

	/// \brief Size to reserve the vertex vectors
	///
	/// Is being set each time in \ref renderLayoutSubpixel()
	gint vertexVectorReserveSize = 1;
	
	Vec4ShPtr textColorPtr = std::make_shared<Vec4>(Vec4{1.0f,1.0f,1.0f,1.0f});
	Vec4ShPtr backgroundColorPtr = std::make_shared<Vec4>(Vec4{0.0f,0.0f,0.0f,1.0f});
	bool drawBackground = true;

	/// \brief The rectangle which encloses the text box as drawn.
	PangoRectangle textBoxRect = {-1,-1,-1,-1};
	/// \brief The vertexes of two triangles which form the background rectangle
	GlRectVertextStruct textBackgroundRectVertexes;
	GlRectSizeStruct textBoxSize;

	VertexBufferKey vertexBufferPerTextureColorKey;

	/** \brief Map of vertex buffers, one per glyph texture
	 *
	 * Key is the glyph texture handle which contains the image of a glyph plus the color
	 * in which the glyphs are to be rendered.
	 */
	std::unordered_map<VertexBufferKey,VertexBufferPerTexture> vertextBufferPerTextureMap;

	VertexBufferKey vertexBufferForTrapezoidBackgroundColorKey;
	VertexBufferKey vertexBufferForTrapezoidUnderlineColorKey;
	VertexBufferKey vertexBufferForTrapezoidStrikethroughColorKey;
	VertexBufferKey vertexBufferForTrapezoidOverlineColorKey;
	
	/// \brief Map of vertex buffers, one per color value. The texture handle remains 0.
	std::unordered_map<VertexBufferKey,VertexBufferForTrapezoids> vertexBufferTrapezoidsPerPart;  

	void drawGlyphs (Mat4 const &MVPMatrix);
	void drawTextBoxBackground (
			Mat4 const &MVMatrix,
			Mat4 const &MVPMatrix,
			Vec3 const &lightDir,
			Vec4 const &lightColor,
			Vec4 const &ambientLightColor
			);

	/// \see RendererBase::setupVertexBuffers()
	void setupVertexBuffersGlyphs ();
	/// \see RendererBase::setupVertexBuffers()
	void setupVertexBuffersTextBoxBackground ();


}; // class GLTextRenderer

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTRENDERER_H_ */
