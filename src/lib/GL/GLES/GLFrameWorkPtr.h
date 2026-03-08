/*
 * GLFrameWorkPtr.h
 *
 *  Created on: Feb 8, 2025
 *      Author: hor
 *
 * Forward and type alias declarations in a separate header file to avoid issues
 * with circular includes of GLFramework and subordinate classes
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

#ifndef GLES_GLFRAMEWORKPTR_H_
#define GLES_GLFRAMEWORKPTR_H_

namespace OevGLES {



class GLFramework;

using GLFrameworkWeakPtr = std::weak_ptr<GLFramework>;
using GLFrameworkSharedPtr = std::shared_ptr<GLFramework>;

} // namespace OevGLES {


#endif /* GLES_GLFRAMEWORKPTR_H_ */
