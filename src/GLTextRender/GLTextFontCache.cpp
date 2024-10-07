/*
 * GLTextFontCache.cpp
 *
 *  Created on: Oct 3, 2024
 *      Author: hor
 *
 *   Cache and cache item classes for the font and font bitmap cache.
 *   The key for the cache item is the PangoFont hash value.
 *   The PangoFont is the specialization of a font face at a specific size and various attributes (style, weight ...)
 *   The cache items also maintains the list the font textures for the rendered glyphs as well as the map for the
 *   glyph descriptions for this font.
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

#include "GLTextFontCache.h"

namespace OevGLES {

} /* namespace OevGLES */
