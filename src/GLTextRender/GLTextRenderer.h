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

#include <string>

#include "GLTextGlobals.h"

namespace OevGLES {

class GLTextRenderer {
public:
	GLTextRenderer(GLTextGlobals& glob);
	virtual ~GLTextRenderer();

	void setText (const std::string& str);

	void renderText();

	void setFontSize (double sizePoints);
	void setFonts(std::string fonts);

	double getFontSize();
	const std::string& getFonts();

	PangoLayout* getPangoLayout() {
		return pangoLayout;
	}

private:
	std::string text;

	PangoLayout* pangoLayout = nullptr;

	GLTextGlobals& globals;
};

} /* namespace OevGLES */

#endif /* GLTEXTRENDER_GLTEXTRENDERER_H_ */
