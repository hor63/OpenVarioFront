/*
 * GLTextPangoCPPWrappers.h
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

#ifndef GLTEXTRENDER_GLTEXTPANGOCPPWRAPPERS_H_
#define GLTEXTRENDER_GLTEXTPANGOCPPWRAPPERS_H_

#include <pango/pangoft2.h>

namespace OevGLES {

template <typename GObj>
class CppGObj {

public:

	CppGObj ()
	:gObj (nullptr)
	{}

	~CppGObj (){
		if (gObj != nullptr) {
			g_object_unref(gObj);
			gObj = nullptr;
		}
	}

	CppGObj (GObj* source,bool increaseReference)
	: gObj {source}
	{
		if (increaseReference) {
			g_object_ref(gObj);
		}
	}

	CppGObj (const CppGObj& source)
	:gObj{source.gObj}
	{
		// Copy the pointer but increase the GObject ref count
		g_object_ref(gObj);
	}

	CppGObj (CppGObj&& source)
	:gObj{source.gObj}
	{
		// Move the pointer; do not increase the GObject ref count
		source.gObj = nullptr;
	}

	CppGObj<GObj>& operator = (const CppGObj<GObj>& source) {
		if (gObj) {
			g_object_unref(gObj);
		}
		gObj = source.gObj;
		g_object_ref(gObj);

		return *this;
	}

	CppGObj<GObj>& operator = (CppGObj<GObj>&& source) {
		if (gObj) {
			g_object_unref(gObj);
		}
		gObj = source.gObj;
		source.gObj = nullptr;

		return *this;
	}

	operator GObj* () {
		return gObj;
	}

	operator GObj* const () const {
		return gObj;
	}

	GObj* operator -> () {
		return gObj;
	}

protected:

	GObj* gObj;
};

class CppPangoFontMap : public CppGObj<PangoFontMap> {
public:
	CppPangoFontMap ()
	{
		gObj = pango_ft2_font_map_new();
	}
};

class CppPangoContext : public CppGObj<PangoContext> {
public:

	CppPangoContext () = delete;

	CppPangoContext (PangoFontMap* fontMap) {
		gObj = pango_font_map_create_context (fontMap);
	}
};

class CppPangoFont : public CppGObj<PangoFont> {
public:

	CppPangoFont () = delete;

};

} // namespace OevGLES {


#endif /* GLTEXTRENDER_GLTEXTPANGOCPPWRAPPERS_H_ */
