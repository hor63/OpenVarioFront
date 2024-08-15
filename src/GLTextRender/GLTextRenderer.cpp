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

	pangoLayout = pango_layout_new(globals.getPangoContext());
}

GLTextRenderer::~GLTextRenderer() {
	g_object_unref(pangoLayout);
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
				LOG4CXX_DEBUG(logger,"\tGot one GlyphItem with num_chars = " << glyphItem->item->num_chars
						<< ", num_glyphs = " << glyphItem->glyphs->num_glyphs);

				for (int i = 0;i < glyphItem->glyphs->num_glyphs; ++i) {
					LOG4CXX_DEBUG(logger,"\t\tGlyph["<<i<<"] "
							<< glyphItem->glyphs->glyphs[i].glyph);
				}
			} else {
				LOG4CXX_DEBUG(logger,"Empty run = End of line");
			}

		} while (pango_layout_iter_next_run(layoutIter));
		LOG4CXX_DEBUG(logger,"End of runs");
		pango_layout_iter_free(layoutIter);
	}
#endif // #if defined HAVE_LOG4CXX_H

}

} /* namespace OevGLES */
