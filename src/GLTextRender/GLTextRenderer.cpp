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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <cmath>

#include "OVFCommon.h"

#include "GLTextRenderer.h"

// ===== Start private PangoGLTextRendererClass glib based stuff ===========================


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif



namespace OevGLES {

class GLTextRenderer;

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

static void pango_gl_text_renderer_draw_glyph     (PangoRenderer    *renderer,
					       PangoFont        *font,
					       PangoGlyph        glyph,
					       double            x,
					       double            y){

	if (PANGO_IS_GL_TEXT_RENDERER(renderer)) {
		PangoGLTextRenderer* pangoGLTextRenderer = PANGO_GL_TEXT_RENDERER(renderer);

		pangoGLTextRenderer->priv->glTextRender->draw_glyph(
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

	renderer_class->draw_glyph = pango_gl_text_renderer_draw_glyph;
	renderer_class->draw_trapezoid = pango_gl_text_renderer_draw_trapezoid;

}

G_END_DECLS


static PangoGLTextRenderer* pango_gl_text_renderer_new(OevGLES::GLTextRenderer* rendererObj) {
	PangoGLTextRenderer* ret = reinterpret_cast<PangoGLTextRenderer*>(g_object_new (PANGO_TYPE_GL_TEXT_RENDERER, NULL));

	ret->priv->glTextRender = rendererObj;

	return ret;
}

// ===== End private PangoGLTextRendererClass glib based stuff =============================

namespace OevGLES {

GLTextRenderer::GLTextRenderer(GLTextGlobalsWeakPtr glob) :
		globals{glob}
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

	pango_layout_get_extents (pangoLayout,&inkRect,&logicalRect);
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
			<< ": Layout ink bounding box topLeft = "
			<< (static_cast<double>(inkRect.x)/PANGO_SCALE) << 'x'
			<< (static_cast<double>(inkRect.y)/PANGO_SCALE)
			<< " size = "
			<< (static_cast<double>(inkRect.width)/PANGO_SCALE) << 'x'
			<< (static_cast<double>(inkRect.height)/PANGO_SCALE)
			);
	LOG4CXX_DEBUG(logger,"\tLayout logical bounding box topLeft = "
			<< (static_cast<double>(logicalRect.x)/PANGO_SCALE) << 'x'
			<< (static_cast<double>(logicalRect.y)/PANGO_SCALE)
			<< " size = "
			<< (static_cast<double>(logicalRect.width)/PANGO_SCALE) << 'x'
			<< (static_cast<double>(logicalRect.height)/PANGO_SCALE)
			);
	LOG4CXX_DEBUG (logger, "\tNumber of Unicode characters = " << pango_layout_get_character_count(pangoLayout));

}

void GLTextRenderer::draw_glyph (
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
			LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
					<< "Texture position of glyph " << glyph << " = "
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

				auto textureIter = vertextBufferPerTextureMap.find(
						glyphInfo.texture.getTexture().getTextureHandle());
				if (textureIter == vertextBufferPerTextureMap.end()) {
					auto newEntry = vertextBufferPerTextureMap.emplace(std::make_pair(
							glyphInfo.texture.getTexture().getTextureHandle(),
							VertexBufferPerTexture{glyphInfo.texture,
						static_cast<size_t>(vertexVectorReserveSize)}));

					LOG4CXX_DEBUG (logger,
							"\tEmplace entry for vertex vector per texture list for texture "
							<< glyphInfo.texture.getTexture().getTextureHandle()
							<< ". Added a new entry = " << (newEntry.second?"Yes":"No"));

					textureIter = newEntry.first;
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
							glyphInfo.texturePositionNormalized.xLeft,
							glyphInfo.texturePositionNormalized.yTop
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
			}

		} else { // if (glyphInfo.renderGlyph)
			LOG4CXX_DEBUG(logger, "\tGlyph " << glyph << " is invisible.");
		}


#if 0


		if (logger->isDebugEnabled()){

			{
				  int x_start, x_limit;
				  int y_start, y_limit;
				  int ixoff = floor (x + 0.5);
				  int iyoff = floor (y + 0.5);
				  int ix, iy;
				  int src , dest;


				  x_start = MAX (0, - (ixoff + ftFace->glyph-> bitmap_left));
				  x_limit = MIN ((int) ftFace->glyph->bitmap.width,
						 (int) (1024 - (ixoff + ftFace->glyph->bitmap_left)));

				  y_start = MAX (0,  - (iyoff - ftFace->glyph->bitmap_top));
				  y_limit = MIN ((int) ftFace->glyph->bitmap.rows,
						 (int) (1024 - (iyoff - ftFace->glyph->bitmap_top)));

				  src =
					y_start * ftFace->glyph->bitmap.pitch;

				  dest =
					(y_start + iyoff - ftFace->glyph->bitmap_top) * 1024 +
					x_start + ixoff + ftFace->glyph->bitmap_left;


				  LOG4CXX_DEBUG(logger,"\tglyph-> bitmap_left = " << ftFace->glyph-> bitmap_left
						  << " glyph->bitmap.width = " << ftFace->glyph->bitmap.width
						  << " glyph->bitmap_top = " << ftFace->glyph->bitmap_top
						  << " glyph->bitmap.rows = " << ftFace->glyph->bitmap.rows
						  << " glyph->bitmap.pitch = " << ftFace->glyph->bitmap.pitch
						  << " ixoff = " << ixoff
						  << " iyoff = " << iyoff
						  << " x_start = " << x_start
						  << " x_limit = " << x_limit
						  << " y_start = " << y_start
						  << " y_limit = " << y_limit
						  << " src offset = " << src
						  << " dest offs (pitch 1024) = " << dest
						  );
			}
		}

#endif // #if defined HAVE_LOG4CXX_H

	} else { //if (globalsPtr) {
		LOG4CXX_WARN(logger, __PRETTY_FUNCTION__ << ": Member globals is gone.");
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

void GLTextRenderer::setupVertexBuffers () {

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "-->Start");

	GLenum glErr = glGetError();

	// Flush the error chain
	while (glErr != GL_NO_ERROR) {
		glErr = glGetError();
	}

	glProgram = GLProgTextTexture::getProgram();

	if (logger->isDebugEnabled()) {
		glErr = glGetError();
		while (glErr != GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger, "\tGLerror in GLProgTextTexture::getProgram() = " << glErr);
			glErr = glGetError();
		}
	}

	glProgram->useProgram();

	if (logger->isDebugEnabled()) {
		glErr = glGetError();
		while (glErr != GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger, "\tGLerror in glProgram->useProgram() = " << glErr);
			glErr = glGetError();
		}
	}

	// Prepare all glyph image textures
	for (auto iter = vertextBufferPerTextureMap.begin();iter != vertextBufferPerTextureMap.end();++iter) {

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per texture = " << iter->second.numVertexes);

		if (iter->second.numVertexes > 0) {
			iter->second.fontTexture.syncTextureDataWithGPU();
			if (iter->second.vertexBufferHandle == 0) {
				LOG4CXX_DEBUG(logger,"\tCreate new vertex buffer handle");
				glGenBuffers(1,&iter->second.vertexBufferHandle);
				if (logger->isDebugEnabled()) {
					glErr = glGetError();
					while (glErr != GL_NO_ERROR) {
						LOG4CXX_DEBUG(logger, "\tGLerror in glGenBuffers() = " << glErr);
						glErr = glGetError();
					}
				}
			}

			LOG4CXX_DEBUG(logger,"\tVertex buffer handle = " << iter->second.vertexBufferHandle);
			glBindBuffer(GL_ARRAY_BUFFER,iter->second.vertexBufferHandle);
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glGenBuffers() = " << glErr);
					glErr = glGetError();
				}
			}

			LOG4CXX_DEBUG(logger,"\tCall glBufferData (target = " << GL_ARRAY_BUFFER
					<< ", size = " << (iter->second.vertexVector.size()*sizeof(GlGlyphVertexStruct))
					<< ", data = " << reinterpret_cast<void*>(&iter->second.vertexVector[0].tri1TopLeft.vertexPosition[0])
					<< ", usage = " << GL_STATIC_DRAW
					<< ")");

			glBufferData(GL_ARRAY_BUFFER,iter->second.vertexVector.size()*sizeof(GlGlyphVertexStruct),
					&iter->second.vertexVector[0].tri1TopLeft.vertexPosition[0],
					GL_STATIC_DRAW);
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glBufferData() = " << glErr);
					glErr = glGetError();
				}
			}

			glBindBuffer(GL_ARRAY_BUFFER,0);
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glBindBuffer(GL_ARRAY_BUFFER,0) = " << glErr);
					glErr = glGetError();
				}
			}
		}
	}
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<--End");

}

void GLTextRenderer::draw(
			OevGLES::Mat4 const &modelMatrix,
			OevGLES::Mat4 const &viewMatrix,
			OevGLES::Mat4 const &ProjMatrix,
			OevGLES::Mat4 const &MVMatrix,
			OevGLES::Mat4 const &MVPMatrix,
			OevGLES::Vec3 const &lightDir,
			OevGLES::Vec4 const &lightColor,
			OevGLES::Vec4 const &ambientLightColor
			) {
	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "-->Start");

	GLenum glErr = glGetError();

	// Flush the error chain
	while (glErr != GL_NO_ERROR) {
		glErr = glGetError();
	}

	glProgram->useProgram();

	if (logger->isDebugEnabled()) {
		glErr = glGetError();
		while (glErr != GL_NO_ERROR) {
			LOG4CXX_DEBUG(logger, "\tGLerror in glProgram->useProgram() = " << glErr);
			glErr = glGetError();
		}
	}

	for (auto iter = vertextBufferPerTextureMap.begin();iter != vertextBufferPerTextureMap.end();++iter) {
		VertexBufferPerTexture& vertexBuffer = iter->second;

		LOG4CXX_DEBUG(logger,"\tNumber vertexes per texture = " << vertexBuffer.numVertexes);

		if (vertexBuffer.numVertexes > 0) {
			vertexBuffer.fontTexture.syncTextureDataWithGPU();


			// Set the uniforms
			glUniformMatrix4fv(glProgram->getUnMvpMatrixLocation(),1,GL_FALSE,&(MVPMatrix(0,0)));
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glUniformMatrix4fv = " << glErr);
					glErr = glGetError();
				}
			}

			glUniform4fv(glProgram->getUnFragColorLocation(),1, &lightColor(0));
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glUniformMatrix4fv = " << glErr);
					glErr = glGetError();
				}
			}

			//vertexBuffer.fontTexture.getTexture().bindToUniformLocation(GL_TEXTURE0, 0, glProgram->getUnTexture0Location());
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in getTexture().bindToUniformLocation() = " << glErr);
					glErr = glGetError();
				}
			}

			// Now assign the attributes in the vertex buffer

			// bind the vertex buffer which contains all vertex data: Model and texture coordinates
			glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer.vertexBufferHandle);
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glBindBuffer() = " << glErr);
					glErr = glGetError();
				}
			}
			glEnableVertexAttribArray(glProgram->getAttVertexPosLocation());
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glEnableVertexAttribArray() = " << glErr);
					glErr = glGetError();
				}
			}
			glVertexAttribPointer(glProgram->getAttVertexPosLocation(),vertextPositionArrayLen,GL_FLOAT,
					GL_FALSE,sizeof(GlGlyphCornerVertexStruct),reinterpret_cast<void*>(offsetof(GlGlyphCornerVertexStruct,vertexPosition)));
			if (logger->isDebugEnabled()) {
				LOG4CXX_DEBUG(logger,"\tCall glVertexAttribPointer (index = " << glProgram->getAttVertexPosLocation()
						<< ", size = " << vertextPositionArrayLen
						<< ", type = " << GL_FLOAT
						<< ", normalized = " << GL_FALSE
						<< ", stride = " << sizeof(GlGlyphCornerVertexStruct)
						<< ", pointer = " << reinterpret_cast<void*>(offsetof(GlGlyphCornerVertexStruct,vertexPosition))
						<< ")");
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glVertexAttribPointer() = " << glErr);
					glErr = glGetError();
				}
			}
			//glEnableVertexAttribArray(glProgram->getAttTexture0PosLocation());
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glEnableVertexAttribArray() = " << glErr);
					glErr = glGetError();
				}
			}
			//glVertexAttribPointer(glProgram->getAttTexture0PosLocation(),texturePositionArrayLen,GL_FLOAT,
			//		GL_FALSE,sizeof(GlGlyphCornerVertexStruct),reinterpret_cast<void*>(4/*offsetof(GlGlyphCornerVertexStruct,texturePosition)*/));
			LOG4CXX_DEBUG(logger,"\tCall glVertexAttribPointer (index = " << glProgram->getAttTexture0PosLocation()
					<< ", size = " << texturePositionArrayLen
					<< ", type = " << GL_FLOAT
					<< ", normalized = " << GL_FALSE
					<< ", stride = " << sizeof(GlGlyphCornerVertexStruct)
					<< ", pointer = " << reinterpret_cast<void*>(offsetof(GlGlyphCornerVertexStruct,texturePosition))
					<< ")");
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glVertexAttribPointer() = " << glErr);
					glErr = glGetError();
				}
			}

			// Now draw the glyphs as pairs of triangles.
			glDrawArrays(GL_TRIANGLES, 0, vertexBuffer.numVertexes);
			if (logger->isDebugEnabled()) {
				glErr = glGetError();
				while (glErr != GL_NO_ERROR) {
					LOG4CXX_DEBUG(logger, "\tGLerror in glDrawArrays() = " << glErr);
					glErr = glGetError();
				}
			}

			if (logger->isTraceEnabled()) {

				LOG4CXX_TRACE(logger,"\tvertexBuffer.vertexVector[0] Vertex positions = \n "
						<< vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri1TopLeft.vertexPosition[3] << "\n"

						<< vertexBuffer.vertexVector[0].tri1BottomLeft.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomLeft.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomLeft.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomLeft.vertexPosition[3] << "\n"

						<< vertexBuffer.vertexVector[0].tri1BottomRight.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomRight.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomRight.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri1BottomRight.vertexPosition[3] << "\n"

						<< vertexBuffer.vertexVector[0].tri2TopLeft.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopLeft.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopLeft.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopLeft.vertexPosition[3] << "\n"

						<< vertexBuffer.vertexVector[0].tri2BottomRight.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri2BottomRight.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri2BottomRight.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri2BottomRight.vertexPosition[3] << "\n"

						<< vertexBuffer.vertexVector[0].tri2TopRight.vertexPosition[0] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopRight.vertexPosition[1] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopRight.vertexPosition[2] << ", "
						<< vertexBuffer.vertexVector[0].tri2TopRight.vertexPosition[3]
						);

			}
			// Reset bindings and assignment of the attribute buffers.
			glDisableVertexAttribArray(glProgram->getAttVertexPosLocation());
			glDisableVertexAttribArray(glProgram->getAttTexture0PosLocation());
			glBindBuffer(GL_ARRAY_BUFFER,0);
		}
	}

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__ << "<-- End");
}

} /* namespace OevGLES */
