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

GLTextRenderer::GLTextRenderer(GLTextGlobals& glob) :
		globals{glob}
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextRenderer");
	}
#endif

	pangoTextRenderer = pango_gl_text_renderer_new(this);

	pangoLayout = pango_layout_new(globals.getPangoContext());
	auto fDesc = pango_layout_get_font_description(pangoLayout);
	if (fDesc == nullptr) {
		fDesc = pango_context_get_font_description(globals.getPangoContext());
	}
	if (fDesc) {
		fontDescr = pango_font_description_copy(fDesc);
	} else {
		fontDescr = pango_font_description_new();
		fonts = "Noto Sans";
		pango_font_description_set_family(fontDescr, fonts.c_str());
		pango_font_description_set_size(fontDescr, 11*PANGO_SCALE);
	}

	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__ << ": Initial font family = " << pango_font_description_get_family(fontDescr)
			<< " with size " << (pango_font_description_get_size(fontDescr)/PANGO_SCALE)
			<< " with style " << static_cast<int>(pango_font_description_get_style(fontDescr))
			<< " with weight " << static_cast<int>(pango_font_description_get_weight(fontDescr)));

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
			<< ": Initial font family = " << fonts<<":"<<pango_font_description_get_family(fontDescr)
			<< " with size " << (pango_font_description_get_size(fontDescr)/PANGO_SCALE)
			<< " with style " << static_cast<int>(pango_font_description_get_style(fontDescr))
			<< " with weight " << static_cast<int>(pango_font_description_get_weight(fontDescr)));
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

void GLTextRenderer::renderLayout(int x, int y) {
	renderLayoutSubpixel (x * PANGO_SCALE, y * PANGO_SCALE);

}

void GLTextRenderer::renderLayoutSubpixel(int x, int y) {
	pango_renderer_draw_layout (&pangoTextRenderer->parent_instance, pangoLayout, x, y);
}

void GLTextRenderer::draw_glyph (
			PangoFont        *font,
			PangoGlyph        glyph,
			double            x,
			double            y) {

	FT_Face ftFace = pango_ft2_font_get_face(font);


#if defined HAVE_LOG4CXX_H


	if (logger->isDebugEnabled()){
		PangoFontDescription* fontDesc = pango_font_describe(font);

		if (fontDesc) {

			LOG4CXX_DEBUG(logger,__FUNCTION__ << ": Glyph " << glyph
					<< ", font family " << pango_font_description_get_family(fontDesc)
					<< ", size = " << (static_cast<double>(pango_font_description_get_size(fontDesc))/PANGO_SCALE)
					<< (pango_font_description_get_size_is_absolute(fontDesc)?"Pixel" : "pt")
					<< " at position " << x << ',' << y
					<< " Pointer font = " << reinterpret_cast<void*>(font)
					<< " , ftFace = " << reinterpret_cast<void*>(ftFace)
					);


			auto rc = FT_Load_Glyph(ftFace, glyph, FT_LOAD_RENDER);
			LOG4CXX_DEBUG(logger,"\tFT_Load_Glyph returned "<< rc);

			if (rc == 0) {
				LOG4CXX_DEBUG(logger,"\t Glyph " << glyph << " height= " << (static_cast<double>(ftFace->glyph->metrics.height) / (1 << 6))
						<< " width = " << (static_cast<double>(ftFace->glyph->metrics.width) / (1 << 6))
						<< " bitmap_left = " << ftFace->glyph->bitmap_left
						<< " bitmap_top = " << ftFace->glyph->bitmap_top
						<< " bitmap.num_grays = " << ftFace->glyph->bitmap.num_grays
						<< " bitmap.width = " << ftFace->glyph->bitmap.width
						<< " bitmap.rows = " << ftFace->glyph->bitmap.rows
						);
			}


			pango_font_description_free(fontDesc);
		}

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

} /* namespace OevGLES */
