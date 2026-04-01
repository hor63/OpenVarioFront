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

#include "pango/pango.h"

namespace OevGLES {

/** \brief C++ template wrapper class for managing gObject objects
 *
 * The class takes in the pointer to a gObject object, and managed the reference count
 * internally, and thus also the life time of the gObject itself.
 *
 * All operations must be done via the accessor functions to the \ref gObj pointer.
 * There are no wrappers around the gObject methods
 *
 * \tparam GObj gObject class (i.e. a C struct) name to be wrapped.
 */
template <typename GObj>
class CppGObj {

public:

	CppGObj () = delete;
	/*
	:gObj (nullptr)
	{}
	*/

	/** \brief The destructor just decrements the \ref gObj reference pointer.
	 *
	 * The gObject lib itself destroys the object itself when the reference counter
	 * becomes 0.
	 */
	~CppGObj (){
		if (gObj != nullptr) {
			decrementRef();
			gObj = nullptr;
		}
	}

	/** \brief
	 *
	 * \param source
	 * \param increaseReference
	 */
	CppGObj (GObj* source,bool increaseReference)
	: gObj {source}
	{
		if (increaseReference) {
			incrementRef();
		}
	}

	CppGObj (const CppGObj& source)
	:gObj{source.gObj}
	{
		// Copy the pointer but increase the GObject ref count
		incrementRef();
	}

	CppGObj (CppGObj&& source)
	:gObj{source.gObj}
	{
		// Move the pointer; do not increase the GObject ref count
		source.gObj = nullptr;
	}

	CppGObj<GObj>& operator = (const CppGObj<GObj>& source) {
		if (gObj) {
			decrementRef ();
		}
		gObj = source.gObj;
		incrementRef();

		return *this;
	}


	CppGObj<GObj>& operator = (CppGObj<GObj>&& source) {
		if (gObj) {
			decrementRef ();
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

	void incrementRef() {
		g_object_ref(gObj);
	}

	void decrementRef () {
		g_object_unref(gObj);
	}

protected:

	GObj* gObj;
};

using CppPangoFontMap = CppGObj<PangoFontMap>;

using CppPangoContext = CppGObj<PangoContext>;

using CppPangoFont = CppGObj<PangoFont>;

// using CppPangoFontdescription = CppGObj<PangoFontDescription>;

template <>
void CppGObj<PangoFontMetrics>::incrementRef();

template <>
void CppGObj<PangoFontMetrics>::decrementRef();

using CppPangoFontMetrics = CppGObj<PangoFontMetrics>;

} // namespace OevGLES {


#endif /* GLTEXTRENDER_GLTEXTPANGOCPPWRAPPERS_H_ */
