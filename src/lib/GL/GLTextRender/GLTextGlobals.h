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

#include <memory>

#include "GLTextPangoCPPWrappers.h"
#include "GLTextFontCache.h"

#include "GLES/GLFrameWorkPtr.h"

namespace OevGLES {

class GLFramework;

/**
 * \brief Holder of the entire object hierarchy needed to implement the font and
 * glyph cache used to render text with OpenGL.
 *
 */
class GLTextGlobals {
	friend class SDLRenderSurface;
public:
	virtual ~GLTextGlobals();

	CppPangoFontMap getFontMap() {
		return fontMap;
	}

	CppPangoContext getPangoContext() {
		return pangoContext;
	}

	GLTextFontCache&	getFontCache () {
		return fontCache;
	}

	void setResolutionDPI(double resolutionX,double resolutionY);

	double resolutionDpiX() {
		return resX;
	}
	double resolutionDpiY() {
		return resY;
	}

private:

	CppPangoFontMap fontMap;
	CppPangoContext pangoContext;
	GLTextFontCache	fontCache;
	/// Resolution in DPI
	///
	/// Initial one point is one pixel.
	///
	/// I need to store the resolutions here because I can set the resolutions in
	/// a \p PangoFT2FontMap but I cannot retrieve the effective resolution with
	/// the PangoFT2FontMap API.
	double resX = 72.0;
	/// \see resX
	double resY = 72.0;

	/// \brief Instances must only be created as members of \ref GLFramework
	GLTextGlobals();

};

using GLTextGlobalsWeakPtr = std::weak_ptr<GLTextGlobals>;
using GLTextGlobalsSharedPtr = std::shared_ptr<GLTextGlobals>;

} /* namespace OevGLES */

#include "GLES/GLFramework.h"

#endif /* GLTEXTRENDER_GLTEXTGLOBALS_H_ */
