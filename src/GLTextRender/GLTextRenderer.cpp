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

#include "OVFCommon.h"

#include "GLTextRenderer.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

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

void GLTextRenderer::setFontSize(double sizePoints) {
	pango_font_description_set_size(fontDescr,(static_cast<gint>(sizePoints*PANGO_SCALE)));
	pango_layout_set_font_description(pangoLayout,fontDescr);
}

void GLTextRenderer::setFonts(std::string fontNames) {
	fonts = fontNames;
	pango_font_description_set_family(fontDescr,fonts.c_str());
	pango_layout_set_font_description(pangoLayout,fontDescr);
}

double GLTextRenderer::getFontSize() {
	return static_cast<double>(pango_font_description_get_size(fontDescr)) / PANGO_SCALE;
}

} /* namespace OevGLES */
