/*
 * GLTextGlobals.cpp
 *
 *  Created on: Aug 10, 2024
 *      Author: hor
 *
 *  Holds the global PangoFT2 fontmap and the Pango context.
 *  It exists only once (singleton).
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


#include "GLTextGlobals.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif

GLTextGlobals::GLTextGlobals() {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.GLTextRender.GLTextGlobals");
	}
#endif

}

GLTextGlobals::~GLTextGlobals() {

	if (pangoContext) {
		g_object_unref(pangoContext);
		pangoContext = nullptr;
	}

	if (fontMap) {
		g_object_unref(fontMap);
		fontMap = nullptr;
	}

}

void GLTextGlobals::init() {

	if (!fontMap) {
		fontMap = pango_ft2_font_map_new();
	}

	if (!pangoContext && fontMap) {
		pangoContext = pango_font_map_create_context (fontMap);
	}

#if defined HAVE_LOG4CXX_H
	if (pangoContext) {
		PangoFontFamily** families = nullptr;
		int numFamilies = 0;

		pango_context_list_families (pangoContext,&families,&numFamilies);
		LOG4CXX_DEBUG(logger, "Number of font families is " << numFamilies);
		for (int i = 0;i < numFamilies;++i) {
			LOG4CXX_TRACE(logger, "Font family ["<<i<<" is " << pango_font_family_get_name(families[i]));
		}
		if (families) {
			g_free(families);
		}

		auto fontDesc = pango_context_get_font_description(pangoContext);
		if (fontDesc) {
			LOG4CXX_DEBUG(logger, "Default font family = " << pango_font_description_get_family(fontDesc)
					<< " with size " << (pango_font_description_get_size(fontDesc)/PANGO_SCALE)
					<< " with style " << static_cast<int>(pango_font_description_get_style(fontDesc))
					<< " with weight " << static_cast<int>(pango_font_description_get_weight(fontDesc)));
		} else {
			LOG4CXX_DEBUG(logger, "Default fontDesc of pangoContext is NULL!");
		}

		PangoFontDescription* newFontDesc = pango_font_description_new ();
		if (newFontDesc) {

			pango_font_description_set_family(newFontDesc, "Noto Sans");
			pango_font_description_set_size(newFontDesc, 20*PANGO_SCALE);

			pango_context_set_font_description(pangoContext, newFontDesc);

			pango_font_description_free(newFontDesc);
			newFontDesc = nullptr;

			fontDesc = pango_context_get_font_description(pangoContext);
			if (fontDesc) {
				LOG4CXX_DEBUG(logger, "Default font family = " << pango_font_description_get_family(fontDesc)
						<< " with size " << (pango_font_description_get_size(fontDesc)/PANGO_SCALE)
						<< " with style " << static_cast<int>(pango_font_description_get_style(fontDesc))
						<< " with weight " << static_cast<int>(pango_font_description_get_weight(fontDesc)));
			} else {
				LOG4CXX_DEBUG(logger, "Default fontDesc of pangoContext is NULL!");
			}

		}

	} else {
		LOG4CXX_FATAL(logger, "pangoContext is NULL!");
	}

#endif // #if defined HAVE_LOG4CXX_H

}

} /* namespace OevGLES */
