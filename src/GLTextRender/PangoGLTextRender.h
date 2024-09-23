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

#define PANGO_TYPE_GL_TEXT_RENDERER            (pango_gl_text_renderer_get_type())

typedef struct _PangoGLTextRendererClass PangoGLTextRendererClass;
typedef struct _PangoGLTextRenderer PangoGLTextRenderer;

#define PANGO_GL_TEXT_RENDERER(object)         (G_TYPE_CHECK_INSTANCE_CAST ((object), PANGO_TYPE_GL_TEXT_RENDERER, PangoGlTextRenderer))
#define PANGO_IS_GL_TEXT_RENDERER(object)      (G_TYPE_CHECK_INSTANCE_TYPE ((object), PANGO_TYPE_GL_TEXT_RENDERER))

#define PANGO_GL_TEXT_RENDERER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))
#define PANGO_IS_GL_TEXT_RENDERER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), PANGO_TYPE_GL_TEXT_RENDERER))
#define PANGO_GL_TEXT_RENDERER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), PANGO_TYPE_GL_TEXT_RENDERER, PangoGLTextRendererClass))

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


/**
 * pango_gl_text_render_layout_subpixel:
 * @layout: a `PangoLayout`
 * @x: the X position of the left of the layout (in Pango units)
 * @y: the Y position of the top of the layout (in Pango units)
 *
 * Render a `PangoLayout` onto an OpenGL ES2 context, with he
 * location specified in fixed-point Pango units rather than
 * pixels.
 *
 * (Using this will avoid extra inaccuracies from rounding
 * to integer pixels multiple times, even if the final glyph
 * positions are integers.)
 *
 */
void
pango_gl_text_render_layout_subpixel (
				  PangoLayout *layout,
				  int          x,
				  int          y);

#endif /* GLTEXTRENDER_PANGOGLTEXTRENDER_H_ */
