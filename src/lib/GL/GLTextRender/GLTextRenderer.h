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

#include <array>

#include <GLES2/gl2.h>

#include "GLES/GLFramework.h"
#include "GLES/GLObjectWrappers.h"
#include "GLTextGlobals.h"
#include "GLPrograms/GLProgTextTexture.h"
#include "GLPrograms/GLProgDiffuseLight.h"
#include "GLPrograms/GLProgControlSimpleFill.h"
#include "Renderers/RenderContext.h"
#include "Renderers/RendererBase.h"
#include "glibconfig.h"
#include "pango/pango-types.h"



// forward declarations for private types within the module.
extern "C" {
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;
}

/// \brief Used for sensible shift values to calculate hashes
static constexpr int NumBitsSizeT = sizeof(size_t) * 8;
static constexpr std::size_t AllOnesSizeT = ~static_cast<size_t>(0U);

namespace OevGLES {

/** \brief Class to be the key for \ref GLTextRenderer::vertextBufferPerTextureMap and
 * \ref GLTextRenderer::vertexBufferTrapezoidsPerPart;  
 *
 * In addition objects also carry the current color for the stuff to render with GL. 
 */
class VertexBufferKey {
public:

	/** \brief Default constructor; initializes static color white.
	 *  
	 * Initializes the static color to white, texture handle = 0,
	 * shared color pointer is empty.
	 */
	VertexBufferKey() {}

	/** \brief Constructor for a static color object
	 *
	 * \param sharedDefaultColor Usually comes from \ref RenderContext.
	 * \param textureHandle Only used when being the key for \ref GLTextRenderer::vertextBufferPerTextureMap
	 */  
	VertexBufferKey(Vec4 const& staticColor,GLuint textureHandle = 0U) :
		staticColor{staticColor},
		textureHandle{textureHandle},
		staticColorUsed {true} 
	{}

	/** \brief Constructor for a shared color pointer object
	 *
	 * \param sharedDefaultColor Usually comes from \ref RenderContext.  
	 * \param textureHandle Only used when being the key for \ref GLTextRenderer::vertextBufferPerTextureMap
	 */
	VertexBufferKey(Vec4ShPtr const &sharedDefaultColor,GLuint textureHandle = 0U) :
		sharedDefaultColor{sharedDefaultColor},
		textureHandle{textureHandle},
		staticColorUsed {!sharedDefaultColor} 
	{}

	bool operator == (VertexBufferKey const & comp) const noexcept {
		if (staticColorUsed) {
			return textureHandle == comp.textureHandle &&
				staticColor == comp.staticColor; 
		} else {
			return textureHandle == comp.textureHandle && 
				sharedDefaultColor.get() == comp.sharedDefaultColor.get();
		}
	}

	/** \brief Set a static and absolute color.
	 *
	 * The shared color pointer is preserved but simply not used now.
	 * When you want to use a static color only temporarily you can switch back
	 * to the shared color pointer calling \ref useSharedColorPtr().
	 */
	void setStaticColor (Vec4 const& newStaticColor) {
		staticColor = newStaticColor;
		staticColorUsed = true;
		hashValueDirty = true;
	}

	/** \brief Restore use of the shared color pointer after temporarily switching to static color.
	 *
	 * When an object is switched temporarily to static color using \ref setStaticColor you can switch back
	 * to the shared color pointer which is retained.
	 *
	 * \return \p true when \ref sharedDefaultColor is valid;
	 *	\p false when \ref sharedDefaultColor is empty. In this case the object will keep using the static color. 
	 */
	bool useSharedColorPtr() {
		if (sharedDefaultColor) {
			staticColorUsed = false;
			hashValueDirty = true;
			return true;
		}
		
		return false;
	}
	
	/** \brief Set and use the shared color pointer.
	 *
	 * When \p newSharedColorPtr is empty the object will use the static color, whatever its value is.
	 *
	 * \return \p true when \p newSharedColorPtr is valid; \p false when \p newSharedColorPtr is empty.
	 * In this case the value in \ref staticColor is being used.
	 */
	bool setSharedColorPtr (Vec4ShPtr const& newSharedColorPtr) {
		sharedDefaultColor = newSharedColorPtr;
		staticColorUsed = !sharedDefaultColor;
		hashValueDirty = true;
		
		return !staticColorUsed;
	}
	
	/** \brief Return the active color value. 
	 *
	 * \return The currently active color value. Either \ref staticColor or *\ref sharedDefaultColor
	 */
	Vec4 const &getColor () const noexcept {
		if (staticColorUsed) {
			return staticColor;
		} else {
			return *sharedDefaultColor;
		}
	}
	
	Vec4ShPtr const &getSharedColorPtr() const noexcept {
		return sharedDefaultColor;
	}
	
	GLuint getTextureHandle() const noexcept {return textureHandle;}
	
	void setTextureHandle(GLuint textureHandle) {
		if (this->textureHandle != textureHandle) {
			this->textureHandle = textureHandle;
			hashValueDirty = true;
		}
	}
	
	/** \brief Whether static or shared color pointer is used for \ref getColor().
	 *
	 * \return When static color is explicitly set or when an empty color shared pointer was set.
	 *
	 * \see \ref setStaticColor()
	 * \see \ref setSharedColorPtr()
	 */
	bool isStaticColorUsed () {return staticColorUsed;}
	
	/** \brief Return the hash value of the object from the active color and texture handle value.
	 */
	size_t hash() const noexcept;
	
private:

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
	
	bool staticColorUsed = true;
	
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
	mutable bool hashValueDirty = true;
}; // class VertexBufferKey

} // namespace OevGLES

template <>
struct std::hash<OevGLES::VertexBufferKey>{
	std::size_t operator()(const OevGLES::VertexBufferKey& k) const noexcept {
		return k.hash();
	}
};

G_BEGIN_DECLS
// Static functions within GLTextRenderer.cpp. Here declared for friend declaration in class GLTextRenderer.
static void pango_gl_text_renderer_prepare_run(PangoRenderer *renderer, PangoLayoutRun *glyphItem);
static void pango_gl_text_renderer_end (PangoRenderer *renderer);

G_END_DECLS

namespace OevGLES {
	
class GLTextRenderer: public RendererBase  {
	
	friend void ::pango_gl_text_renderer_prepare_run(PangoRenderer *renderer, PangoLayoutRun *glyphItem);
	friend void ::pango_gl_text_renderer_end (PangoRenderer *renderer);
	
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
	
	class VertexBufferPerTexture final {
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

	class VertexBufferForTrapezoids final {
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

	/** \brief Callback from a Pango.Layout rendering text
	 * 
	 * \param part
	 * \param y1 y position of the upper horizontal line
	 * \param x11 Upper left x position
	 * \param x21 Upper right x position
	 * \param y2 y position of the lower horizontal line
	 * \param x12 Lower left x position
	 * \param x22 Lower right x position
	 */
	void drawTrapezoid(PangoRenderPart part, double y1, double x11, double x21,
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
		return vertexBufferPerTextureColorKey.getSharedColorPtr();
	}

	void setTextColor(Vec4ShPtr const &textColorPtr);

	const Vec4ShPtr& getBackgroundColor() const {
		return backgroundColorPtr;
	}

	void setBackgroundColor(Vec4ShPtr const &backgroundColor);

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
	
	GLProgControlSimpleFill* glSimpleFillProg = nullptr;
//	GLBufferObject vertexBufferHandleSimpleFills;
//	GLVertexArrayObject vertexArrayHandleSimpleFills;

	GLProgDiffuseLight* glTextBackgroundProgram = nullptr;
	GLBufferObject vertexBufferHandleTextBackground;
	GLVertexArrayObject vertexArrayHandleTextBackground;

	/// \brief Size to reserve the vertex vectors
	///
	/// Is being set each time in \ref renderLayoutSubpixel()
	gint vertexVectorReserveSize = 1;
	
//	Vec4ShPtr textColorPtr = std::make_shared<Vec4>(Vec4{1.0f,1.0f,1.0f,1.0f});
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

	/// \brief Color keys for the different \p PangoRenderPart indexes
	std::array<VertexBufferKey,PANGO_RENDER_PART_OVERLINE + 1> vertexBufferForTrapezoidColorKeys;
	
	/// \brief Map of vertex buffers, one per color value. The texture handle remains 0.
	std::unordered_map<VertexBufferKey,VertexBufferForTrapezoids> vertexBufferTrapezoidsPerColor;  

	void drawGlyphs (Mat4 const &MVPMatrix);
	void drawTrapezoids (Mat4 const &MVPMatrix);
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
	/// \see RendererBase::setupVertexBuffers()
	void setupVertexBuffersTrapezoids();


}; // class GLTextRenderer

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTRENDERER_H_ */
