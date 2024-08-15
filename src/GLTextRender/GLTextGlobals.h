/*
 * GLTextGlobals.h
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

#ifndef GLTEXTRENDER_GLTEXTGLOBALS_H_
#define GLTEXTRENDER_GLTEXTGLOBALS_H_

#include <pango/pangoft2.h>

namespace OevGLES {

class GLTextGlobals {
public:
	GLTextGlobals();
	virtual ~GLTextGlobals();

	void init();

	PangoFontMap* getFontMap() {
		return fontMap;
	}

	PangoContext* getPangoContext() {
		return pangoContext;
	}

private:

	PangoFontMap* fontMap = nullptr;
	PangoContext* pangoContext = nullptr;

};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTGLOBALS_H_ */
