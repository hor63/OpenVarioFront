/*
 * GLTextPangoCPPWrappers.cpp
 *
 *  Created on: Oct 5, 2024
 *      Author: hor
 *
 *	Simple C++ wrappers around Pango classes which take care of glib object lifetime
 *	but allow simple copy and move by using the glib reference count where possible.
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

#include "GLTextPangoCPPWrappers.h"

namespace OevGLES {


template <>
void CppGObj<PangoFontMetrics>::incrementRef() {
	pango_font_metrics_ref (gObj);
}

template <>
void CppGObj<PangoFontMetrics>::decrementRef() {
	pango_font_metrics_unref (gObj);
}


} // namespace OevGLES {

