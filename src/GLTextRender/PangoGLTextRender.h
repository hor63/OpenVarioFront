/*
 * PangoGLTextRender.h
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

#ifndef GLTEXTRENDER_PANGOGLTEXTRENDER_H_
#define GLTEXTRENDER_PANGOGLTEXTRENDER_H_

#include <pango/pangoft2.h>


namespace OevGLES {

class GLTextRenderer;

} // namespace OevGLES

G_BEGIN_DECLS


typedef struct _PangoGLTextRendererClass PangoGLTextRendererClass;
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;


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

PangoGLTextRenderer* pango_gl_text_renderer_new(OevGLES::GLTextRenderer* rendererObj);

G_END_DECLS


#endif /* GLTEXTRENDER_PANGOGLTEXTRENDER_H_ */
