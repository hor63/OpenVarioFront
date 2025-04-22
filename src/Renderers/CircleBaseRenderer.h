/*
 * CircleBaseRenderer.h
 *
 *  Created on: Apr 22, 2025
 *      Author: hor
 *
 *	Base class for all things circular.
 *	It is based on triangle meshes forming different types of circular forms.
 *	Derived classes will render full or partial arcs, with an inner and outer diameter
 *	or tire-like forms
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2025  Kai Horstmann
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

#ifndef RENDERERS_CIRCLEBASERENDERER_H_
#define RENDERERS_CIRCLEBASERENDERER_H_

#include "RendererBase.h"

namespace OevGLES {

class CircleBaseRenderer: public RendererBase {
public:

	CircleBaseRenderer();
	virtual ~CircleBaseRenderer();
}; // class CircleBaseRenderer

} /* namespace OevGLES */

#endif /* RENDERERS_CIRCLEBASERENDERER_H_ */
