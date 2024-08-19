/*
 * PangoGLTextRender.cpp
 *
 *  Created on: Aug 18, 2024
 *      Author: hor
 *
 *  Implements a PangoRenderer for Freetype 2 Glyphs into an OpenGL surface
 *
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

#include <glib-object.h>

#include "PangoGLTextRender.h"
#include <pango/pangoft2.h>

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

extern "C" {
typedef struct _PangoGLTextRendererClass PangoGLTextRendererClass;
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;

#define PANGO_TYPE_GL_TEXT_RENDERER            (pango_gl_text_renderer_get_type())
#define PANGO_GL_TEXT_RENDERER(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PANGO_TYPE_GL_TEXT_RENDERER, PangoGlTextRenderer))
#define PANGO_IS_GL_TEXT_RENDERER(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PANGO_TYPE_GL_TEXT_RENDERER))

#define PANGO_GL_TEXT_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))
#define PANGO_IS_GL_TEXT_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PANGO_TYPE_GL_TEXT_RENDERER))
#define PANGO_GL_TEXT_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))

struct _PangoGLTextRenderer
{
  PangoRenderer parent_instance;

};

struct _PangoGLTextRendererClass
{
  PangoRendererClass parent_class;
};

static void pango_gl_text_renderer_draw_glyph     (PangoRenderer    *renderer,
					       PangoFont        *font,
					       PangoGlyph        glyph,
					       double            x,
					       double            y){

#if defined HAVE_LOG4CXX_H

	if (logger->isDebugEnabled()){
		PangoFontDescription* fontDesc = pango_font_describe(font);

		if (fontDesc) {

			LOG4CXX_DEBUG(logger,__FUNCTION__ << ": Glyph " << glyph
					<< ", font family " << pango_font_description_get_family(fontDesc)
					<< ", size = " << (static_cast<double>(pango_font_description_get_size(fontDesc))/PANGO_SCALE)
					<< (pango_font_description_get_size_is_absolute(fontDesc)?"Pixel" : "pt")
					<< " at position " << x << ',' << y);

			FT_Face ftFace = pango_ft2_font_get_face(font);

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
	}

#endif // #if defined HAVE_LOG4CXX_H

}

static void pango_gl_text_renderer_draw_trapezoid (PangoRenderer    *renderer,
					       PangoRenderPart   part,
					       double            y1,
					       double            x11,
					       double            x21,
					       double            y2,
					       double            x12,
					       double            x22){

}

G_DEFINE_TYPE (PangoGLTextRenderer, pango_gl_text_renderer, PANGO_TYPE_RENDERER)

static void
pango_gl_text_renderer_init (PangoGLTextRenderer *self /* G_GNUC_UNUSED*/) {

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

} // extern "C"

static PangoRenderer *pangoGlTextRenderer = nullptr;

static PangoRenderer *getPangoGLRenderer(){
	if (pangoGlTextRenderer == nullptr) {
		pangoGlTextRenderer = reinterpret_cast<PangoRenderer *>(g_object_new (PANGO_TYPE_GL_TEXT_RENDERER, NULL));
	}

	return pangoGlTextRenderer;
}


void
pango_gl_text_render_layout_subpixel (
				  PangoLayout *layout,
				  int          x,
				  int          y)
{
  PangoContext *context;
  PangoFontMap *fontmap;
  PangoRenderer *renderer;

  g_return_if_fail (PANGO_IS_LAYOUT (layout));

  context = pango_layout_get_context (layout);
  fontmap = pango_context_get_font_map (context);
  renderer = getPangoGLRenderer();

  pango_renderer_draw_layout (renderer, layout, x, y);
}

void pango_gl_text_render_layout (
			 PangoLayout *layout,
			 int          x,
			 int          y)
{
  pango_gl_text_render_layout_subpixel (layout, x * PANGO_SCALE, y * PANGO_SCALE);
}

