/*
 * GLTextRenderer.cpp
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
#include "OVFCommon.h"

#include "pango/pango-renderer.h"

#include "GLES/GLFramework.h"
#include "GLTextRenderer.h"

// ===== Start private PangoGLTextRendererClass glib based stuff ===========================


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

namespace OevGLES {

size_t VertexBufferKey::hash() const noexcept {

	if (hashValueDirty){
	
		hashValue = textureHandle << 8;
		if (staticColorUsed) {
			hashValue ^= 
			static_cast<std::size_t>(staticColor(0) * 255.0f) ^
			(static_cast<std::size_t>(staticColor(1) * 255.0f) << 4) ^
			(static_cast<std::size_t>(staticColor(2) * 255.0f) << 8) ^
			(static_cast<std::size_t>(staticColor(3) * 255.0f) << 12);
		} else {
			hashValue ^= reinterpret_cast<std::size_t>(sharedDefaultColor.get());
		}
		hashValueDirty = false;
	}
		
	return hashValue;
}

class GLTextRenderer;

static constexpr guint16 AllOnesGuint16 = ~static_cast<guint16>(0U);
static constexpr Vec4::Scalar AllOnesGuint16F = static_cast<Vec4::Scalar>(AllOnesGuint16);

/** \brief Store a reference to a \ref VertexBufferKey whose color has been set to a static color
 * 
 * Sole purpose is to reset the color of the referenced \ref VertexBufferKey back to using the shared color pointer
 * in the destructor.
 * This way I can store all locally modified color keys in a container.
 * When the container is being destroyed all stored color attributes are automatically reset to the shared default color.
 * The charm is that this reset by destructor works under all circumstances, even when an exception flies by   
 */
class VertexBufferKeyLocColorSet final {
	public:
	
	VertexBufferKeyLocColorSet(VertexBufferKey& vertBufKey,PangoAttrColor const & staticColor) :
	vertexBufferKeyWithStatColor{vertBufKey}
	{
		Vec4 newStaticColor = {
			static_cast<Vec4::Scalar>(staticColor.color.red) / AllOnesGuint16F,
			static_cast<Vec4::Scalar>(staticColor.color.green) / AllOnesGuint16F,
			static_cast<Vec4::Scalar>(staticColor.color.blue) / AllOnesGuint16F,
			1.0f
		};
		vertexBufferKeyWithStatColor.setStaticColor(newStaticColor);
	}
	
	VertexBufferKeyLocColorSet(VertexBufferKeyLocColorSet const & source) = delete;
	VertexBufferKeyLocColorSet(VertexBufferKeyLocColorSet && source) = delete;
	VertexBufferKeyLocColorSet& operator = (VertexBufferKeyLocColorSet const & source) = delete;
	VertexBufferKeyLocColorSet& operator = (VertexBufferKeyLocColorSet && source) = delete;
	
	~VertexBufferKeyLocColorSet() {
		vertexBufferKeyWithStatColor.useSharedColorPtr();
	}
	private:

	VertexBufferKey & vertexBufferKeyWithStatColor; 
}; // VertexBufferKeyLocColorSet
} // namespace OevGLES

G_BEGIN_DECLS


typedef struct _PangoGLTextRendererClass PangoGLTextRendererClass;

typedef struct _PangoGLTextRendererPrivate PangoGLTextRendererPrivate;

struct _PangoGLTextRenderer
{
  PangoRenderer parent_instance;

  PangoGLTextRendererPrivate* priv;
};

struct _PangoGLTextRendererClass
{
	PangoRendererClass parent_class;

	void (*end_parent)(PangoRenderer *renderer);

	void (*prepare_run_parent)(PangoRenderer *renderer, PangoLayoutRun *run);
};

static PangoGLTextRenderer* pango_gl_text_renderer_new(OevGLES::GLTextRenderer* rendererObj);

#define PANGO_GL_TEXT_RENDERER(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRenderer))
#define PANGO_IS_GL_TEXT_RENDERER(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PANGO_TYPE_GL_TEXT_RENDERER))

#define PANGO_GL_TEXT_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))
#define PANGO_IS_GL_TEXT_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PANGO_TYPE_GL_TEXT_RENDERER))
#define PANGO_GL_TEXT_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))

struct _PangoGLTextRendererPrivate {
	OevGLES::GLTextRenderer* glTextRender;
};


G_DEFINE_TYPE_WITH_PRIVATE (PangoGLTextRenderer, pango_gl_text_renderer, PANGO_TYPE_RENDERER)

#define PANGO_TYPE_GL_TEXT_RENDERER            (pango_gl_text_renderer_get_type())

/// \brief At the end of the rendering reset all colors to the shared default color.
static void pango_gl_text_renderer_end (PangoRenderer *renderer) {
	auto rendererClass = PANGO_GL_TEXT_RENDERER_GET_CLASS(renderer);
	auto glRenderer = PANGO_GL_TEXT_RENDERER(renderer);

	if (glRenderer == nullptr || rendererClass == nullptr) {
		LOG4CXX_WARN (logger, __PRETTY_FUNCTION__
			<< ": Renderer " << renderer << " is not a PangoGLTextRenderer!"
			);
		return;
	}

	LOG4CXX_DEBUG (logger, __PRETTY_FUNCTION__);

	// First reset all previously statically set colors back to default shared color.
	glRenderer->priv->glTextRender->vertexBufferPerTextureColorKey
		.useSharedColorPtr();
	for (auto &colorKey :
		 glRenderer->priv->glTextRender->vertexBufferForTrapezoidColorKeys) {
		colorKey.useSharedColorPtr();
	}

	// Call the superclass member.
	if (rendererClass->end_parent != nullptr) {
		rendererClass->end_parent(renderer);
	}
}

/// \brief Extract the color attributes for the run and set the colors. Then call the base class method.
static void pango_gl_text_renderer_prepare_run(PangoRenderer *renderer, PangoLayoutRun *glyphItem) {
	auto rendererClass = PANGO_GL_TEXT_RENDERER_GET_CLASS(renderer);
	auto glRenderer = PANGO_GL_TEXT_RENDERER(renderer);
	
	if (glRenderer == nullptr || rendererClass == nullptr) {
		LOG4CXX_WARN (logger, __PRETTY_FUNCTION__
			<< ": Renderer " << renderer << " is not a PangoGLTextRenderer!"
			);
		return;
	}

	
	LOG4CXX_DEBUG (logger, __PRETTY_FUNCTION__
		<< "===================================>\n"
		<< ": numGlyphs = " << glyphItem->glyphs->num_glyphs
		<< ", length = " << glyphItem->item->length
		<< ", numChars = " << glyphItem->item->num_chars
		<< ", offset = " << glyphItem->item->offset
	);

	// First reset all previously statically set colors back to default shared color.
	glRenderer->priv->glTextRender->vertexBufferPerTextureColorKey
		.useSharedColorPtr();
	for (auto &colorKey :
		 glRenderer->priv->glTextRender->vertexBufferForTrapezoidColorKeys) {
		colorKey.useSharedColorPtr();
	}

	// Now walk through the attributes for this run and set static colors according to the attributes.
	for (auto attrListItem = glyphItem->item->analysis.extra_attrs;
		 attrListItem != nullptr; attrListItem = attrListItem->next) {

		auto attr =
			reinterpret_cast<PangoAttribute const *>(attrListItem->data);

		LOG4CXX_DEBUG (logger,
			"\t attribute type = " << attr->klass->type
			<< ", start index = " << attr->start_index
			<< ", end index = " << attr->end_index
		);

		switch (attr->klass->type) {
			case PANGO_ATTR_FOREGROUND:
			{
				auto colorAttr = reinterpret_cast<PangoAttrColor const *>(attr);
				OevGLES::Vec4 newStaticColor = {
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.red) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.green) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.blue) / OevGLES::AllOnesGuint16F,
					1.0f
				};
				glRenderer->priv->glTextRender->vertexBufferPerTextureColorKey
					.setStaticColor(newStaticColor);
				glRenderer->priv->glTextRender
					->vertexBufferForTrapezoidColorKeys
						[PANGO_RENDER_PART_FOREGROUND]
					.setStaticColor(newStaticColor);
				
				LOG4CXX_DEBUG(logger, "\tSet foreground color to " << newStaticColor.transpose());
				break;
			}
			case PANGO_ATTR_BACKGROUND:
			{
				auto colorAttr = reinterpret_cast<PangoAttrColor const *>(attr);
				OevGLES::Vec4 newStaticColor = {
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.red) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.green) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.blue) / OevGLES::AllOnesGuint16F,
					1.0f
				};
				glRenderer->priv->glTextRender
					->vertexBufferForTrapezoidColorKeys
						[PANGO_RENDER_PART_BACKGROUND]
					.setStaticColor(newStaticColor);

					LOG4CXX_DEBUG(logger, "\tSet background color to " << newStaticColor.transpose());
				break;
			}
			case PANGO_ATTR_UNDERLINE_COLOR:
			{
				auto colorAttr = reinterpret_cast<PangoAttrColor const *>(attr);
				OevGLES::Vec4 newStaticColor = {
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.red) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.green) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.blue) / OevGLES::AllOnesGuint16F,
					1.0f
				};
				glRenderer->priv->glTextRender
					->vertexBufferForTrapezoidColorKeys
						[PANGO_RENDER_PART_UNDERLINE]
					.setStaticColor(newStaticColor);

					LOG4CXX_DEBUG(logger, "\tSet underline color to " << newStaticColor.transpose());
				break;
			}
			case PANGO_ATTR_STRIKETHROUGH_COLOR:
			{
				auto colorAttr = reinterpret_cast<PangoAttrColor const *>(attr);
				OevGLES::Vec4 newStaticColor = {
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.red) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.green) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.blue) / OevGLES::AllOnesGuint16F,
					1.0f
				};
				glRenderer->priv->glTextRender
					->vertexBufferForTrapezoidColorKeys
						[PANGO_RENDER_PART_STRIKETHROUGH]
					.setStaticColor(newStaticColor);

					LOG4CXX_DEBUG(logger, "\tSet strikethrough color to " << newStaticColor.transpose());
				break;
			}
			case PANGO_ATTR_OVERLINE_COLOR:
			{
				auto colorAttr = reinterpret_cast<PangoAttrColor const *>(attr);
				OevGLES::Vec4 newStaticColor = {
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.red) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.green) / OevGLES::AllOnesGuint16F,
					static_cast<OevGLES::Vec4::Scalar>(colorAttr->color.blue) / OevGLES::AllOnesGuint16F,
					1.0f
				};
				glRenderer->priv->glTextRender
					->vertexBufferForTrapezoidColorKeys
						[PANGO_RENDER_PART_OVERLINE]
					.setStaticColor(newStaticColor);

					LOG4CXX_DEBUG(logger, "\tSet overline color to " << newStaticColor.transpose());
				break;
			}
			default:
				// A non-color attribute is handled by the Pango core itself.
				break;
		} // switch (attr->klass->type)

	}

	if (rendererClass->prepare_run_parent != nullptr) {
		rendererClass->prepare_run_parent(renderer, glyphItem);
	}

	LOG4CXX_DEBUG (logger, __PRETTY_FUNCTION__
		<< "<<<===================================");
}

static void pango_gl_text_renderer_draw_glyph     (PangoRenderer    *renderer,
					       PangoFont        *font,
					       PangoGlyph        glyph,
					       double            x,
					       double            y){

	if (PANGO_IS_GL_TEXT_RENDERER(renderer)) {
		PangoGLTextRenderer* pangoGLTextRenderer = PANGO_GL_TEXT_RENDERER(renderer);

		pangoGLTextRenderer->priv->glTextRender->drawGlyph(
				font,
				glyph,
				x,y);
	}

}

static void pango_gl_text_renderer_draw_trapezoid (PangoRenderer    *renderer,
					       PangoRenderPart   part,
					       double            y1,
					       double            x11,
					       double            x21,
					       double            y2,
					       double            x12,
					       double            x22){

	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": part = " << static_cast<int>(part)
			<< ", y1  = " << y1
			<< ", x11 = " << x11
			<< ", x21 = " << x21
			<< ", y2  = " << y2
			<< ", x12 = " << x12
			<< ", x22 = " << x22
			);

	if (PANGO_IS_GL_TEXT_RENDERER(renderer)) {
		PangoGLTextRenderer* pangoGLTextRenderer = PANGO_GL_TEXT_RENDERER(renderer);

		pangoGLTextRenderer->priv->glTextRender->drawTrapezoid(
			part, y1, x11, x21, y2, x12, x22);
	}
}


static void
pango_gl_text_renderer_init (PangoGLTextRenderer *self /* G_GNUC_UNUSED*/) {

	self->priv = reinterpret_cast<PangoGLTextRendererPrivate*>(pango_gl_text_renderer_get_instance_private (self));
	self->priv->glTextRender = nullptr;

	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": self = " << reinterpret_cast<void*>(self)
			<<  ", self->priv = " << reinterpret_cast<void*>(self->priv)
			);

}

static void
pango_gl_text_renderer_class_init (PangoGLTextRendererClass *klass) {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.PangoGLTextRender");
	}
#endif

	PangoRendererClass *renderer_class = PANGO_RENDERER_CLASS (klass);

	klass->end_parent = renderer_class->end;
	klass->prepare_run_parent = renderer_class->prepare_run;
	
	renderer_class->draw_glyph = pango_gl_text_renderer_draw_glyph;
	renderer_class->draw_trapezoid = pango_gl_text_renderer_draw_trapezoid;
	renderer_class->prepare_run = pango_gl_text_renderer_prepare_run;
	renderer_class->end = pango_gl_text_renderer_end;
	

}

G_END_DECLS


static PangoGLTextRenderer* pango_gl_text_renderer_new(OevGLES::GLTextRenderer* rendererObj) {
	PangoGLTextRenderer *ret = reinterpret_cast<PangoGLTextRenderer *>(
		g_object_new(PANGO_TYPE_GL_TEXT_RENDERER, NULL));

	ret->priv->glTextRender = rendererObj;

	return ret;
}

// ===== End private PangoGLTextRendererClass glib based stuff =============================

namespace OevGLES {

	#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr logger = 0;
	#endif

GLTextRenderer::VertexBufferPerTexture::~VertexBufferPerTexture() {
	
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": vertexBufferHandle = " << vertexBufferHandle.get()
		<< ", vertexArrayHandle  = " << vertexArrayHandle.get()
		);
}

GLTextRenderer::VertexBufferForTrapezoids::~VertexBufferForTrapezoids() {}

/// \brief Copy template for the vertices data of the background rectangle.
///
///  The x and y corners must be adjusted to the actual size of the background rectangle of the text box
///
/// Please note that the z-coordinate is a little recessed compared to the model of the glyph vertices to ensure that both are being drawn.
///
static GLTextRenderer::GlRectVertextStruct constexpr textBackgroundRectVertexesTemplate = GLTextRenderer::GlRectVertextStruct{
			.tri1TopLeft {0.0f,0.0f,-0.0f,1.0f},
			.tri1BottomLeft {0.0f,-1.0f,-0.0f,1.0f},
			.tri1BottomRight {1.0f,-1.0f,-0.0f,1.0f},
			.tri2TopLeft {0.0f,0.0f,-0.0f,1.0f},
			.tri2BottomRight {1.0f,-1.0f,-0.0f,1.0f},
			.tri2TopRight {1.0f,0.0f,-0.0f,1.0f}
	};

static GLfloat const textBackgroundRectNormal [GLTextRenderer::vertextPositionArrayLen] {
	0.0f,0.0f,1.0f,0.0f};

GLTextRenderer::GLTextRenderer(
		RenderContextSharedPtr const &context) :
		RendererBase{context},
		globals{this->context->glTextGlobSharedPtr},
		textBackgroundRectVertexes{textBackgroundRectVertexesTemplate},
		vertexBufferHandleTextBackground{false}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextRenderer");
	}
#endif

	auto globalsPtr = globals.lock();

	if (globalsPtr) {

		pangoTextRenderer = pango_gl_text_renderer_new(this);

		pangoLayout = pango_layout_new(globalsPtr->getPangoContext());
		auto fDesc = pango_layout_get_font_description(pangoLayout);
		if (fDesc == nullptr) {
			fDesc = pango_context_get_font_description(globalsPtr->getPangoContext());
		}
		if (fDesc) {
			fontDescr = pango_font_description_copy(fDesc);
		} else {
			fontDescr = pango_font_description_new();
			fonts = "Noto Sans";
			pango_font_description_set_family(fontDescr, fonts.c_str());
			pango_font_description_set_size(fontDescr, 11*PANGO_SCALE);
		}

		PangoFontMask fontMask = pango_font_description_get_set_fields (fontDescr);
		if (!(fontMask & PANGO_FONT_MASK_SIZE)){
			pango_font_description_set_size(fontDescr, 11*PANGO_SCALE);
		}
		if (!(fontMask & PANGO_FONT_MASK_FAMILY)){
			fonts = "Noto Sans";
			pango_font_description_set_family(fontDescr,fonts.c_str());
		} else {
			fonts = pango_font_description_get_family(fontDescr);
		}
		pango_layout_set_font_description(pangoLayout,fontDescr);

		LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
				<< ": Font family = " << fonts<<":"<<pango_font_description_get_family(fontDescr)
				<< " with size " << (pango_font_description_get_size(fontDescr)/PANGO_SCALE)
				<< " with style " << static_cast<int>(pango_font_description_get_style(fontDescr))
				<< " with weight " << static_cast<int>(pango_font_description_get_weight(fontDescr)));

	} else { // if (globalsPtr) {
		LOG4CXX_WARN(logger, __PRETTY_FUNCTION__ << ": Member globals is gone.");
	}
	
	
	// Set the default colors.
	vertexBufferPerTextureColorKey.setSharedColorPtr(context->foregroundColorPtr);
	for (auto & colorKey :vertexBufferForTrapezoidColorKeys) {
		// set the color to the shared foreground pointer
		colorKey.setSharedColorPtr(context->foregroundColorPtr);
	}
	// .. except of course the background color
	vertexBufferForTrapezoidColorKeys[PANGO_RENDER_PART_BACKGROUND]
		.setSharedColorPtr(context->backgroundColorPtr);
}

GLTextRenderer::~GLTextRenderer() {
	if (pangoLayout) {
		g_object_unref(pangoLayout);
	}
	if (fontDescr) {
		pango_font_description_free (fontDescr);
	}
	if (pangoTextRenderer) {
		g_object_unref(pangoTextRenderer);
	}

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": vertexBufferHandleTextBackground = " << vertexBufferHandleTextBackground.get()
		<< ", vertexArrayHandleTextBackground  = " << vertexArrayHandleTextBackground.get()
		);
}

void GLTextRenderer::setText (const std::string& str){
	text = str;

	pango_layout_set_text(pangoLayout, text.c_str(), -1);

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << ": text = " << text);
	LOG4CXX_DEBUG(logger,"\tNumber of characters" << pango_layout_get_character_count(pangoLayout));

#if defined HAVE_LOG4CXX_H
	if (logger->isTraceEnabled()) {
		PangoLayoutIter *layoutIter = pango_layout_get_iter (pangoLayout);

		do {
			PangoGlyphItem *glyphItem = pango_layout_iter_get_run_readonly(layoutIter);

			if (glyphItem) {
				auto glyphFontDescr = pango_font_describe(glyphItem->item->analysis.font);
				LOG4CXX_TRACE(logger,"\tGot one GlyphItem with num_chars = " << glyphItem->item->num_chars
						<< ", num_glyphs = " << glyphItem->glyphs->num_glyphs);

				LOG4CXX_TRACE(logger,"\t\t Font pointer = " << reinterpret_cast<void*>(glyphItem->item->analysis.font));
				if (glyphFontDescr) {

					LOG4CXX_TRACE(logger,"\t\tFont family = " << pango_font_description_get_family (glyphFontDescr)
							<< " with size " << (pango_font_description_get_size(fontDescr)/PANGO_SCALE)
							<< " with style " << static_cast<int>(pango_font_description_get_style(glyphFontDescr))
							<< " with weight " << static_cast<int>(pango_font_description_get_weight(glyphFontDescr)));

					pango_font_description_free(glyphFontDescr);
				}

				for (int i = 0;i < glyphItem->glyphs->num_glyphs; ++i) {
					LOG4CXX_TRACE(logger,"\t\t\tGlyph["<<i<<"] = "
							<< glyphItem->glyphs->glyphs[i].glyph
							<< " at x = " << glyphItem->glyphs->glyphs[i].geometry.x_offset
							<< ", y = " << glyphItem->glyphs->glyphs[i].geometry.y_offset
							<< " with width = " << glyphItem->glyphs->glyphs[i].geometry.y_offset
														);
				}
			} else {
				LOG4CXX_TRACE(logger,"Empty run = End of line");
			}

		} while (pango_layout_iter_next_run(layoutIter));
		LOG4CXX_TRACE(logger,"End of runs");
		pango_layout_iter_free(layoutIter);
	}
#endif // #if defined HAVE_LOG4CXX_H

}

void GLTextRenderer::setTextColor(Vec4ShPtr const &textColorPtr) {
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << " this = " << this
		<< ": Color ptr = " << textColorPtr.get()
		<< ", Color = " << (*textColorPtr.get()).transpose()
	);
	vertexBufferPerTextureColorKey.setSharedColorPtr(textColorPtr);
	// Remember the background color
	auto backgroundColorBackup =
		vertexBufferForTrapezoidColorKeys[PANGO_RENDER_PART_BACKGROUND]
			.getSharedColorPtr();
			
	// write all colors
	for (auto& colorKey :vertexBufferForTrapezoidColorKeys) {
			colorKey.setSharedColorPtr(textColorPtr);
	}
	
	// and restore the background color.
	vertexBufferForTrapezoidColorKeys[PANGO_RENDER_PART_BACKGROUND]
		.setSharedColorPtr(backgroundColorBackup);
}

void GLTextRenderer::setBackgroundColor(Vec4ShPtr const &backgroundColorPtr) {
	this->backgroundColorPtr = backgroundColorPtr;
	vertexBufferForTrapezoidColorKeys[PANGO_RENDER_PART_BACKGROUND]
		.setSharedColorPtr(backgroundColorPtr);
}

void GLTextRenderer::renderLayout(int x, int y, RenderMode renderMode) {
	renderLayoutSubpixel (x * PANGO_SCALE, y * PANGO_SCALE, renderMode);

}

void GLTextRenderer::renderLayoutSubpixel(int x, int y, RenderMode renderMode) {

	auto charCount = pango_layout_get_character_count(pangoLayout);
	if (charCount < 1) {
		return;
	}

	// Ensures that not more than the vectors are not being resized more than once,
	// but does not waste excessive memory when there are more than one texture per
	// font. Most fonts will not have more than 2 textures for the map.
	// Chinese and Kanji are a totally different beast however.
	vertexVectorReserveSize = charCount / 2 + 1;

	this->renderMode = renderMode;


	for (auto iter = vertextBufferPerTextureMap.begin();
			iter!=vertextBufferPerTextureMap.end();
			++iter){
		iter->second.vertexVector.clear();
		iter->second.numVertexes = 0;
	}


	pango_renderer_draw_layout (&pangoTextRenderer->parent_instance, pangoLayout, x, y);

	pango_layout_get_extents (pangoLayout,nullptr,&textBoxRect);
	pango_extents_to_pixels(&textBoxRect,nullptr);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
			<< ": Layout ink bounding box topLeft = "
			<< textBoxRect.x << 'x'
			<< textBoxRect.y
			<< ", size = "
			<< textBoxRect.width << 'x'
			<< textBoxRect.height
			);

	textBackgroundRectVertexes.tri1TopLeft[0] =
			textBackgroundRectVertexes.tri1BottomLeft[0] =
			textBackgroundRectVertexes.tri2TopLeft[0] =
					textBoxRect.x;

	textBackgroundRectVertexes.tri1BottomRight[0] =
			textBackgroundRectVertexes.tri2BottomRight[0] =
			textBackgroundRectVertexes.tri2TopRight[0] =
					textBoxRect.x + textBoxRect.width;

	textBackgroundRectVertexes.tri1TopLeft[1] =
			textBackgroundRectVertexes.tri2TopLeft[1] =
			textBackgroundRectVertexes.tri2TopRight[1] =
					-textBoxRect.y;

	textBackgroundRectVertexes.tri1BottomLeft[1] =
			textBackgroundRectVertexes.tri1BottomRight[1] =
			textBackgroundRectVertexes.tri2BottomRight[1] =
					-textBoxRect.y - textBoxRect.height;

	textBoxSize = {
		static_cast<GLfloat>(textBoxRect.x),
		static_cast<GLfloat>(-textBoxRect.y),
		static_cast<GLfloat>(textBoxRect.width),
		static_cast<GLfloat>(textBoxRect.height)
	};

	LOG4CXX_DEBUG (logger, "\tNumber of Unicode characters = " << pango_layout_get_character_count(pangoLayout));

}

void GLTextRenderer::drawGlyph (
			PangoFont        *font,
			PangoGlyph        glyph,
			double            x,
			double            y) {

#if 0
	FT_Face ftFace = pango_ft2_font_get_face(font);
#endif

	auto globalsPtr = globals.lock();
	if (globalsPtr) {

		if (font != previousFont) {
			previousFont = font;
			previousFontCacheItem = globalsPtr->getFontCache().getCacheItem(font);
		}

		auto glyphInfo = previousFontCacheItem->getGlyphInfo(glyph);
		if (glyphInfo.renderGlyph) {
			LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << " this = " << this
					<< ": Texture position of glyph " << glyph << " = "
					<< glyphInfo.texturePosition.xLeft << 'x'
					<< glyphInfo.texturePosition.yBottom << ' '
					<< glyphInfo.texturePosition.xRight << 'x'
					<< glyphInfo.texturePosition.yTop
					);

			LOG4CXX_DEBUG(logger,"\t"
					<< "horiBearingX = " << glyphInfo.glyphMetrics.horiBearingX/64.0
					<< " horiBearingY = " << glyphInfo.glyphMetrics.horiBearingY/64.0
					<< " width = " << glyphInfo.glyphMetrics.width/64.0
					<< " height = " << glyphInfo.glyphMetrics.height/64.0
					);

			LOG4CXX_DEBUG(logger, "\tDraw the glyph to "
					<< x << ',' << y);

			if (renderMode == RENDER_GLYPHS) {
				GLfloat left = static_cast<GLfloat>(
							static_cast<double>(glyphInfo.glyphMetrics.horiBearingX) / 64.0
							+ x
						);
				GLfloat right = static_cast<GLfloat>(
							static_cast<double>(glyphInfo.glyphMetrics.width) / 64.0
							+ left
						);

				// Note that the Pango coordinates count y from top down,
				// whereas OpenGL counts from bottom to top.
				// Therefore I start the text box at 0 and let it grow downwards negative.
				// I keep track of the overall bounding box, and lift the bottom of
				// the entire box with the model matrix later up to 0.
				GLfloat top = static_cast<GLfloat> (
							static_cast<double>(glyphInfo.glyphMetrics.horiBearingY) / 64.0
							- y
						);
				GLfloat bottom = static_cast<GLfloat> (
							top
							- static_cast<double> (glyphInfo.glyphMetrics.height) / 64.0
						);

				LOG4CXX_DEBUG(logger, "\tDraw the glyph box from "
						<< left << 'x' << top
						<< " to " << right << 'x' << bottom);

				LOG4CXX_DEBUG(logger, "\tThe image is in the texture from "
						<< glyphInfo.texturePositionNormalized.xLeft
						<< 'x' << glyphInfo.texturePositionNormalized.yTop
						<< " to " << glyphInfo.texturePositionNormalized.xRight
						<< 'x' << glyphInfo.texturePositionNormalized.yBottom
						);

				vertexBufferPerTextureColorKey.setTextureHandle(
					glyphInfo.texture.getTexture().getTextureHandle());
				auto textureIter = vertextBufferPerTextureMap.find(
					vertexBufferPerTextureColorKey);
				if (textureIter == vertextBufferPerTextureMap.end()) {
					auto newEntry =
						vertextBufferPerTextureMap.emplace(std::make_pair(
							vertexBufferPerTextureColorKey,
							VertexBufferPerTexture{
								context, glyphInfo.texture,
								static_cast<size_t>(vertexVectorReserveSize)}));

					LOG4CXX_DEBUG (logger,
							"\tEmplace entry for vertex vector per texture list for texture "
							<< glyphInfo.texture.getTexture().getTextureHandle()
							<< ", hash value = " << vertexBufferPerTextureColorKey.hash() << " and "
							<< (newEntry.first->first.getSharedColorPtr()? "dynamic" : "static")
							<< " color "
							<< newEntry.first->first.getColor().transpose()
							<< ". Added a new entry = " << (newEntry.second?"Yes":"No"));

					textureIter = newEntry.first;
				} else {
					LOG4CXX_DEBUG (logger,
							"\tFound vertex vector per texture list for texture "
							<< glyphInfo.texture.getTexture().getTextureHandle()
							<< ", hash value = " << vertexBufferPerTextureColorKey.hash()
							<< " and " << (textureIter->first.getSharedColorPtr()? "dynamic" : "static")
							<< " color " << textureIter->first.getColor().transpose()
						);
				}

				textureIter->second.vertexVector.push_back(GlGlyphVertexStruct {
					.tri1TopLeft = GlGlyphCornerVertexStruct {
							.vertexPosition = {left,top,0.0f,1.0f},
							.texturePosition = {
								glyphInfo.texturePositionNormalized.xLeft,
								glyphInfo.texturePositionNormalized.yTop
							}
					},
					.tri1BottomLeft = GlGlyphCornerVertexStruct {
						.vertexPosition = {left,bottom,0.0f,1.0f},
						.texturePosition = {
							glyphInfo.texturePositionNormalized.xLeft,
							glyphInfo.texturePositionNormalized.yBottom
						}
					},
					.tri1BottomRight = GlGlyphCornerVertexStruct {
						.vertexPosition = {right,bottom,0.0f,1.0f},
						.texturePosition = {
							glyphInfo.texturePositionNormalized.xRight,
							glyphInfo.texturePositionNormalized.yBottom
						}
					},
					.tri2TopLeft = GlGlyphCornerVertexStruct {
						.vertexPosition = {left,top,0.0f,1.0f},
						.texturePosition = {
							glyphInfo.texturePositionNormalized.xLeft,
							glyphInfo.texturePositionNormalized.yTop
						}
					},
					.tri2BottomRight = GlGlyphCornerVertexStruct {
						.vertexPosition = {right,bottom,0.0f,1.0f},
						.texturePosition = {
							glyphInfo.texturePositionNormalized.xRight,
							glyphInfo.texturePositionNormalized.yBottom
						}
					},
					.tri2TopRight = GlGlyphCornerVertexStruct {
						.vertexPosition = {right,top,0.0f,1.0f},
						.texturePosition = {
							glyphInfo.texturePositionNormalized.xRight,
							glyphInfo.texturePositionNormalized.yTop
						}
					}
				});
				textureIter->second.numVertexes += 6;

				LOG4CXX_DEBUG(logger,"vertexVector capacity = "
						<< textureIter->second.vertexVector.capacity()
						<< ", number elements = "
						<< textureIter->second.vertexVector.size());
			} else { // if (renderMode == RENDER_GLYPHS)
				LOG4CXX_DEBUG(logger, "\tNo visible output intended. Just glyph caching");
			}
						

		} else { // if (glyphInfo.renderGlyph)
			LOG4CXX_DEBUG(logger, "\tGlyph " << glyph << " is invisible.");
		}

	} else { //if (globalsPtr) {
		LOG4CXX_WARN(logger, __PRETTY_FUNCTION__ << ": Member globals is gone.");
	}

}

void GLTextRenderer::drawTrapezoid(
			PangoRenderPart   part,
			double            y1,
			double            x11,
			double            x21,
			double            y2,
			double            x12,
			double            x22
		) {
			
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << " this = " << this
			<< "part = " << part
			<< "y1   = " << y1 
			<< "x11  = " << x11 
			<< "x21  = " << x21 
			<< "y1   = " << y1 
			<< "x12  = " << x12 
			<< "x22  = " << x22 
			);

	if (renderMode == RENDER_GLYPHS) {

		if (static_cast<int>(part) > PANGO_RENDER_PART_OVERLINE) {
			part = PANGO_RENDER_PART_FOREGROUND;
		}
		
		auto &colorKey = vertexBufferForTrapezoidColorKeys[part];
		LOG4CXX_DEBUG(logger,
			   "\tColor ptr [" << static_cast<int>(part) << "] = "
			   << vertexBufferForTrapezoidColorKeys[part].getSharedColorPtr()
			<< ", Color = " << vertexBufferForTrapezoidColorKeys[part].getColor().transpose()
		);

		
		auto vertexBufferIter = vertexBufferTrapezoidsPerColor.find(colorKey);
		if(vertexBufferIter == vertexBufferTrapezoidsPerColor.end()) {
			auto newEntry =
				vertexBufferTrapezoidsPerColor.emplace(std::make_pair(
					colorKey,
					VertexBufferForTrapezoids{context,1}));
					
			vertexBufferIter = newEntry.first;

			LOG4CXX_DEBUG (logger,
					"\tEmplace entry for vertex vector for "
					<< (newEntry.first->first.getSharedColorPtr()? "dynamic" : "static")
					<< " color "
					<< newEntry.first->first.getColor().transpose()
					<< ", hash value = " << vertexBufferPerTextureColorKey.hash()
					<< ". Added a new entry = " << (newEntry.second?"Yes":"No"));

		} else {
			LOG4CXX_DEBUG (logger,
					"\tFound vertex vector per color list for hash value = "
					<< vertexBufferPerTextureColorKey.hash()
					<< " and " << (vertexBufferIter->first.getSharedColorPtr()? "dynamic" : "static")
					<< " color " << vertexBufferIter->first.getColor().transpose()
				);
		}
		
		GLfloat yTop = -y1;
		GLfloat yBottom = -y2;
		
		GLfloat xTopLeft = x11;
		GLfloat xTopRight = x21;
		GLfloat xBottomLeft = x12;
		GLfloat xBottomRight = x22;
		
		LOG4CXX_DEBUG(logger, '\t'
			<< ", yTop = " << yTop
			<< ", yBottom = " << yBottom

			<< ", xTopLeft = " << xTopLeft
			<< ", xTopRight = " << xTopRight
			<< ", xBottomLeft = " << xBottomLeft
			<< ", xBottomRight = " << xBottomRight
				);

		
        // The coordinates are as follows:
		// y1    x11-----x12
		//  ^      /    /
		//  |     /    /
        // y2 x21/----/x22
		vertexBufferIter->second.vertexVector.push_back(GlRectVertextStruct {
			.tri1TopLeft = {
				xTopLeft,yTop,0.0f,1.0f
			},
			.tri1BottomLeft = { 
				xBottomLeft,yBottom,0.0f,1.0f
			},
			.tri1BottomRight = { 
				xBottomRight,yBottom,0.0f,1.0f
			},
			.tri2TopLeft = { 
				xTopLeft,yTop,0.0f,1.0f
			},
			.tri2BottomRight = { 
				xBottomRight,yBottom,0.0f,1.0f
			},
			.tri2TopRight = { 
				xTopRight,yTop,0.0f,1.0f
			}
		});
		vertexBufferIter->second.numVertexes += 6;

		LOG4CXX_DEBUG(logger,"vertexVector capacity = "
				<< vertexBufferIter->second.vertexVector.capacity()
				<< ", number elements = "
				<< vertexBufferIter->second.vertexVector.size());
	} else { // if (renderMode == RENDER_GLYPHS)
		LOG4CXX_DEBUG(logger, "\tNo visible output intended. Just glyph caching");
	}
}


void GLTextRenderer::setFontSize(double sizePoints) {
	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": sizePoints = " << sizePoints);
	pango_font_description_set_size(fontDescr,(static_cast<gint>(sizePoints*PANGO_SCALE)));
	pango_layout_set_font_description(pangoLayout,fontDescr);
}

void GLTextRenderer::setFonts(std::string fontNames) {
	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": fontNames = " << fontNames);
	fonts = fontNames;
	pango_font_description_set_family(fontDescr,fonts.c_str());
	pango_layout_set_font_description(pangoLayout,fontDescr);
}

double GLTextRenderer::getFontSize() {
	double ret = static_cast<double>(pango_font_description_get_size(fontDescr)) / PANGO_SCALE;

	LOG4CXX_DEBUG(logger,__FUNCTION__ << ": ret = " << ret);

	return ret;
}

void GLTextRenderer::setWidthSubpixel(int width) {
	pango_layout_set_width(pangoLayout, width);
}

void GLTextRenderer::setHeightSubpixel(int height) {
	pango_layout_set_height(pangoLayout, height);
}

void GLTextRenderer::setupVertexBuffers () {
	setupVertexBuffersGlyphs ();

	setupVertexBuffersTrapezoids();
	
	if (drawBackground) {
		setupVertexBuffersTextBoxBackground ();
	}
}

void GLTextRenderer::setupVertexBuffersTextBoxBackground () {
	// First get the program
	glTextBackgroundProgram = OevGLES::GLProgDiffuseLight::getProgram();

	if(!vertexBufferHandleTextBackground.valid()) {
		vertexBufferHandleTextBackground = GLBufferObject(true);
	}
	GlBindArrayBufferObject bindTextBackgroundBuffer(vertexBufferHandleTextBackground);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textBackgroundRectVertexes),
				 &textBackgroundRectVertexes, GL_STATIC_DRAW);

	if (context->vertexArrayIsUsable && !vertexArrayHandleTextBackground.valid()) {
		vertexArrayHandleTextBackground = GLVertexArrayObject (context);
		GLBindVertexArrayObject bindTextBackgroundVertexArray (vertexArrayHandleTextBackground);

		// setup the vertex coordinates
		glEnableVertexAttribArray(glTextBackgroundProgram->getVertexPosLocation());
		glVertexAttribPointer(
			glTextBackgroundProgram->getVertexPosLocation(),
			vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
			vertextPositionArrayLen * sizeof(GLfloat),
			reinterpret_cast<void const *>(0U));
	} // if (GLFramework::isVertexArrayUsable()) {

}

void GLTextRenderer::setupVertexBuffersGlyphs () {

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "-->Start");

	GLenum glErr = glGetError();

	// Flush the error chain
	while (glErr != GL_NO_ERROR) {
		glErr = glGetError();
	}

	glGlyphProgram = GLProgTextTexture::getProgram();

	// Prepare all glyph image textures
	for (auto iter = vertextBufferPerTextureMap.begin();iter != vertextBufferPerTextureMap.end();++iter) {

		VertexBufferPerTexture& vertexBuffer = iter->second;

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per texture = " << vertexBuffer.numVertexes);

		vertexBuffer.fontTexture.getTexture().setMagnificationFilter(GLTexture::Linear);
		vertexBuffer.fontTexture.getTexture().setMinificationFilter(GLTexture::Linear);

		if (vertexBuffer.numVertexes > 0) {
			vertexBuffer.fontTexture.syncTextureDataWithGPU();
			if (!vertexBuffer.vertexBufferHandle.valid()) {
				LOG4CXX_DEBUG(logger,"\tCreate new vertex buffer handle");
				vertexBuffer.vertexBufferHandle = GLBufferObject(true);
			}

			LOG4CXX_DEBUG(logger,"\tVertex buffer handle = " << vertexBuffer.vertexBufferHandle.get());
			GlBindArrayBufferObject bindVertexBufferObject (vertexBuffer.vertexBufferHandle);

			LOG4CXX_DEBUG(logger,"\tCall glBufferData (target = " << GL_ARRAY_BUFFER
					<< ", size = " << (vertexBuffer.vertexVector.size()*sizeof(GlGlyphVertexStruct))
					<< ", data = " << reinterpret_cast<void*>(&vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[0])
					<< ", usage = " << GL_STATIC_DRAW
					<< ")");

			glBufferData(GL_ARRAY_BUFFER,vertexBuffer.vertexVector.size()*sizeof(GlGlyphVertexStruct),
					&vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[0],
					GL_STATIC_DRAW);

			if(context->vertexArrayIsUsable && !vertexBuffer.vertexArrayHandle.valid()){
				vertexBuffer.vertexArrayHandle = GLVertexArrayObject(context);
				GLBindVertexArrayObject bindVertexArray (vertexBuffer.vertexArrayHandle);
				
				glEnableVertexAttribArray(glGlyphProgram->getAttVertexPosLocation());
				glVertexAttribPointer(
					glGlyphProgram->getAttVertexPosLocation(),
					vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(GlGlyphCornerVertexStruct),
					reinterpret_cast<void *>(
						offsetof(GlGlyphCornerVertexStruct, vertexPosition)));
				glEnableVertexAttribArray(
					glGlyphProgram->getAttTexture0PosLocation());
				glVertexAttribPointer(
					glGlyphProgram->getAttTexture0PosLocation(),
					texturePositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(GlGlyphCornerVertexStruct),
					reinterpret_cast<void const *>(
						offsetof(GlGlyphCornerVertexStruct, texturePosition)));
			}
		}
	}
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<--End");

}

void GLTextRenderer::setupVertexBuffersTrapezoids() {
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << " this = " << this << "-->Start");

	GLenum glErr = glGetError();

	// Flush the error chain
	while (glErr != GL_NO_ERROR) {
		glErr = glGetError();
	}

	glSimpleFillProg = GLProgControlSimpleFill::getProgram();

	// Prepare all trapezoid vertex buffers
	for (auto iter = vertexBufferTrapezoidsPerColor.begin();iter != vertexBufferTrapezoidsPerColor.end();++iter) {

		auto& vertexBuffer = iter->second;

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per color = " << vertexBuffer.numVertexes);

		if (vertexBuffer.numVertexes > 0) {
			if (!vertexBuffer.vertexBufferHandle.valid()) {
				vertexBuffer.vertexBufferHandle = GLBufferObject(true);
				LOG4CXX_DEBUG(logger,"\tCreate new vertex buffer handle = "
					<< vertexBuffer.vertexBufferHandle.get());
			}

			LOG4CXX_DEBUG(logger,"\tVertex buffer handle = " << vertexBuffer.vertexBufferHandle.get());
			GlBindArrayBufferObject bindVertexBufferObject (vertexBuffer.vertexBufferHandle);

			LOG4CXX_DEBUG(logger,"\tCall glBufferData (target = " << GL_ARRAY_BUFFER
					<< ", size = " << (vertexBuffer.vertexVector.size()*sizeof(GlRectVertextStruct))
					<< ", data = " << reinterpret_cast<void*>(&vertexBuffer.vertexVector[0].tri1TopLeft[0])
					<< ", usage = " << GL_STATIC_DRAW
					<< ")");

			glBufferData(GL_ARRAY_BUFFER,vertexBuffer.vertexVector.size()*sizeof(GlRectVertextStruct),
					&vertexBuffer.vertexVector[0].tri1TopLeft[0],
					GL_STATIC_DRAW);

			if(context->vertexArrayIsUsable && !vertexBuffer.vertexArrayHandle.valid()){
				vertexBuffer.vertexArrayHandle = GLVertexArrayObject(context);
				GLBindVertexArrayObject bindVertexArray (vertexBuffer.vertexArrayHandle);
				GLint boundArrayBuffer = 0;

				glGetIntegerv (GL_ARRAY_BUFFER_BINDING,&boundArrayBuffer);
				
				LOG4CXX_DEBUG(logger,
					"\tBoundArrayBuffer = " << boundArrayBuffer
					);

								
				glEnableVertexAttribArray(glSimpleFillProg->getVertexPosLocation());
				glVertexAttribPointer(
					glSimpleFillProg->getVertexPosLocation(),
					vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(SingleVertexArr),
					reinterpret_cast<void *>(0));
			}
		}
	}
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<--End");

}

void GLTextRenderer::draw(RenderStandardUniforms const &stdUniformData) {

	if (drawBackground) {
		drawTextBoxBackground(
				stdUniformData.getMVMatrixC(),
				stdUniformData.getMVPMatrixC(),
				stdUniformData.getLightDirC(),
				stdUniformData.getLightColorC(),
				stdUniformData.getAmbientLightColorC()
				);

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(-0.5f,-1.0);
	}

	// Glyph boxes partially overlap intentionally.
	// This may be for kerning like "AV", or for kalligraphic scripts like
	// Arabic, or Hindi or Bangali. The glyphs are rendered in transparent mode
	// anyway. To avoid that the overlapping part a later glyph is not rendered
	// due to the depth test from an earlier glyph in the same plane the depth
	// test function must temporarily be changed from the default GL_LESS to
	// GL_LEQUAL. 
	GLint depthFuncBackup = GL_LESS;
	glGetIntegerv(GL_DEPTH_FUNC,&depthFuncBackup);
	glDepthFunc(GL_LEQUAL);
	
	// All text and trapezoids are drawn transparent
	{
		auto blendAttrs = std::make_unique<BlendAttributeSetRestoreStd>();

		drawTrapezoids (stdUniformData.getMVPMatrixC());
		drawGlyphs(stdUniformData.getMVPMatrixC());
	}

	glDepthFunc(depthFuncBackup);

	if (drawBackground) {
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

}

void GLTextRenderer::drawGlyphs (OevGLES::Mat4 const &MVPMatrix){
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "-->Start");

	GLenum glErr = glGetError();

	GlProgUse useGlyphProgram(*glGlyphProgram);

	for (auto iter = vertextBufferPerTextureMap.begin();iter != vertextBufferPerTextureMap.end();++iter) {
		auto& vertexBuffer = iter->second;
		auto& vertexColorKey = iter->first;

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per texture = " << vertexBuffer.numVertexes);

		if (vertexBuffer.numVertexes > 0) {
			vertexBuffer.fontTexture.syncTextureDataWithGPU();


			// Set the uniforms
			glUniformMatrix4fv(glGlyphProgram->getUnMvpMatrixLocation(), 1,
							   GL_FALSE, &(MVPMatrix(0, 0)));
			glUniform4fv(glGlyphProgram->getUnFragColorLocation(), 1,
						 &(vertexColorKey.getColor()(0)));
			vertexBuffer.fontTexture.getTexture().bindToUniformLocation(
				GL_TEXTURE1, 1, glGlyphProgram->getUnTexture0Location());

			GLBindVertexArrayObject bindVertexArrayObject;
			GlBindArrayBufferObject bindArrayBufferObject;
			GLVertexArrayAttribObject vertexArrayEnableVertexPos;
			GLVertexArrayAttribObject vertexArrayEnableTexture0Pos;

			// Now assign the attributes in the vertex buffer
			if(vertexBuffer.vertexArrayHandle.valid()) {
				
				bindVertexArrayObject = GLBindVertexArrayObject (vertexBuffer.vertexArrayHandle);
			} else {
				// bind the vertex buffer which contains all vertex data: Model and texture coordinates
				bindArrayBufferObject = GlBindArrayBufferObject (vertexBuffer.vertexBufferHandle);

				vertexArrayEnableVertexPos = GLVertexArrayAttribObject(
					true, glGlyphProgram->getAttVertexPosLocation());
				glVertexAttribPointer(
					glGlyphProgram->getAttVertexPosLocation(),
					vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(GlGlyphCornerVertexStruct),
					reinterpret_cast<void *>(
						offsetof(GlGlyphCornerVertexStruct, vertexPosition)));

				vertexArrayEnableTexture0Pos = GLVertexArrayAttribObject(
					true, glGlyphProgram->getAttTexture0PosLocation());
				glVertexAttribPointer(
					glGlyphProgram->getAttTexture0PosLocation(),
					texturePositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(GlGlyphCornerVertexStruct),
					reinterpret_cast<void const *>(
						offsetof(GlGlyphCornerVertexStruct, texturePosition)));
			} // if(vertexBuffer.vertexArrayHandle == 0)

			// Now draw the glyphs as pairs of triangles.
			BlendAttributeSetRestore setAndRestoreBlendMode;

			glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.numVertexes);

		}
	}

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<-- End");
}

void GLTextRenderer::drawTrapezoids (OevGLES::Mat4 const &MVPMatrix){
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << " this = " << this << "-->Start");

	GLenum glErr = glGetError();

	GlProgUse useSimpleFillProgram(*glSimpleFillProg);

	for (auto iter = vertexBufferTrapezoidsPerColor.begin();iter != vertexBufferTrapezoidsPerColor.end();++iter) {
		auto& vertexBuffer = iter->second;
		auto& vertexColorKey = iter->first;

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per trapezoid color = " << vertexBuffer.numVertexes);

		LOG4CXX_DEBUG(logger,"\tVertex color = " << vertexColorKey.getColor().transpose());

		if (vertexBuffer.numVertexes > 0) {

			// Set the uniforms
			glUniformMatrix4fv(glSimpleFillProg->getMvpMatrixLocation(), 1,
							   GL_FALSE, &(MVPMatrix(0, 0)));

			glUniform4fv(glSimpleFillProg->getFillColorLocation(), 1,
						 &(vertexColorKey.getColor()(0)));

			GLBindVertexArrayObject bindVertexArrayObject;
			GlBindArrayBufferObject bindArrayBufferObject;
			GLVertexArrayAttribObject vertexArrayEnableVertexPos;

			// Now assign the attributes in the vertex buffer
			if(vertexBuffer.vertexArrayHandle.valid()) {
				
				bindVertexArrayObject = GLBindVertexArrayObject (vertexBuffer.vertexArrayHandle);
			} else {
				// bind the vertex buffer which contains all vertex data: Model and texture coordinates
				bindArrayBufferObject = GlBindArrayBufferObject (vertexBuffer.vertexBufferHandle);

				vertexArrayEnableVertexPos = GLVertexArrayAttribObject(
					true, glSimpleFillProg->getVertexPosLocation());
				glVertexAttribPointer(
					glSimpleFillProg->getVertexPosLocation(),
					vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
					sizeof(SingleVertexArr),
					reinterpret_cast<void const*>(0));

			} // if(vertexBuffer.vertexArrayHandle == 0)

			glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.numVertexes);

		}
	}

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<-- End");
}

void GLTextRenderer::drawTextBoxBackground (
		OevGLES::Mat4 const &MVMatrix,
		OevGLES::Mat4 const &MVPMatrix,
		OevGLES::Vec3 const &lightDir,
		OevGLES::Vec4 const &lightColor,
		OevGLES::Vec4 const &ambientLightColor
		) {
	// make the text background program current
	GlProgUse useTextBackgroundProgram (*glTextBackgroundProgram);

	// Set the uniforms
	glUniformMatrix4fv(glTextBackgroundProgram->getMvpMatrixLocation(), 1,
					   GL_FALSE, &(MVPMatrix(0, 0)));
	glUniformMatrix4fv(glTextBackgroundProgram->getMvMatrixLocation(), 1,
					   GL_FALSE, &(MVMatrix(0, 0)));

	glUniform3fv(glTextBackgroundProgram->getLightDirLocation(), 1,
				 &(lightDir(0)));
	glUniform4fv(glTextBackgroundProgram->getLightColorLocation(), 1,
				 &(lightColor(0)));
	glUniform4fv(glTextBackgroundProgram->getAmbientLightColorLocation(), 1,
				 &(ambientLightColor(0)));

	// set the color attribute constant
	glDisableVertexAttribArray(glTextBackgroundProgram->getVertexColorLocation());
	glVertexAttrib4fv(glTextBackgroundProgram->getVertexColorLocation(),&(*backgroundColorPtr)(0));

	// The normal is the same value for all vertexes
	glDisableVertexAttribArray(glTextBackgroundProgram->getVertexNormalLocation());
	glVertexAttrib4fv(glTextBackgroundProgram->getVertexNormalLocation(),textBackgroundRectNormal);

	GLBindVertexArrayObject bindTextBackgroundVertexArray;
	GlBindArrayBufferObject bindTextBackgroundBuffer(vertexBufferHandleTextBackground);
	GLVertexArrayAttribObject enableVertexArrayObject;

	if (vertexArrayHandleTextBackground.valid()) {
		bindTextBackgroundVertexArray = GLBindVertexArrayObject (vertexArrayHandleTextBackground);
	} else {
		bindTextBackgroundBuffer = GlBindArrayBufferObject (vertexBufferHandleTextBackground);
	
		// setup the vertex coordinates
		glEnableVertexAttribArray(glTextBackgroundProgram->getVertexPosLocation());
		enableVertexArrayObject = GLVertexArrayAttribObject(
			true, glTextBackgroundProgram->getVertexPosLocation());
		glVertexAttribPointer(glTextBackgroundProgram->getVertexPosLocation(),
							  vertextPositionArrayLen, GL_FLOAT, GL_FALSE,
							  vertextPositionArrayLen * sizeof(GLfloat),
							  reinterpret_cast<void const *>(0U));
	}

	// Draw in transparent mode when the Alpha value is not totally opaque.
	std::unique_ptr<BlendAttributeSetRestoreStd> blendAttrs;
	if ((*backgroundColorPtr)(3) < 1.0f) {
		blendAttrs = std::unique_ptr<BlendAttributeSetRestoreStd>(new BlendAttributeSetRestoreStd);
	}

	glDrawArrays(GL_TRIANGLES,0,6);

}
} /* namespace OevGLES */
