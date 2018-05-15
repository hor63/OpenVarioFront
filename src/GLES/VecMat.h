/*
 * VecMat.h
 *
 *  Created on: May 15, 2018
 *      Author: hor
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2018  Kai Horstmann
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


#ifndef VECMAT_H_
#define VECMAT_H_

#include <math.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLES2/gl2platform.h>

#include "Eigen"

namespace OevGLES {

// vector and matrix type definitions like in OpenGL

typedef Eigen::Matrix<GLfloat,2,1> Vec2;
typedef Eigen::Matrix<GLfloat,3,1> Vec3;
typedef Eigen::Matrix<GLfloat,4,1> Vec4;

typedef Eigen::Matrix<GLfloat,2,2> Mat2;
typedef Eigen::Matrix<GLfloat,3,3> Mat3;
typedef Eigen::Matrix<GLfloat,4,4> Mat4;

/** \brief Constructs and returns a translation matrix
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param x Shift by x
 * @param y Shift by y
 * @param z Shift by z
 * @return Translation matrix
 */
Mat4 translationMatrix (GLfloat x, GLfloat y, GLfloat z );

/** \brief Constructs and returns a scaling matrix
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param x Scale by x
 * @param y Scale by y
 * @param z Scale by z
 * @return Scaling matrix
 */
Mat4 scalingMatrix (GLfloat x, GLfloat y, GLfloat z );

/** \brief Constructs and returns a rotation matrix around the X axis
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param adX Angle in Degrees around the X-Axis
 * @return Rotation matrix around the X axis
 */
Mat4 rotationMatrixX (GLfloat adX);

/** \brief Constructs and returns a rotation matrix around the Y axis
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param adY Angle in Degrees around the Y-Axis
 * @return Rotation matrix around the Y axis
 */
Mat4 rotationMatrixY (GLfloat adY);

/** \brief Constructs and returns a rotation matrix around the Z axis
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param adZ Angle in Degrees around the Z-Axis
 * @return Rotation matrix around the Z axis
 */
Mat4 rotationMatrixZ (GLfloat adZ);

/** \brief Constructs and returns the View matrix of a virtual camera looking to a defined point.
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param camPos Position of the virtual camera
 * @param lookAt Point at which the camera looks at.
 * @param up Up vector of the camera. Used to tilt the Camera.
 * @return View matrix
 */
Mat4 viewMatrix (Vec3 const& camPos, Vec3 const &lookAt, Vec3 const & up);

/** \brief Constructs and returns a projection matrix
 *
 * \see <a href="http://antongerdelan.net/teaching/3dprog1/maths_cheat_sheet.pdf" >Dr Anton Gerdelan's 3d Math cheat sheet</a>
 *
 * @param near Near plane of the viewing frustum
 * @param far  Far plane of the viewing frustum
 * @param aspect Aspect of the image, i.e. width/height
 * @param fieldOfViewAngle Field of view angle in degrees
 * @return Projection Matrix
 */
Mat4 projectionMatrix (GLfloat near, GLfloat far, GLfloat aspect, GLfloat fieldOfViewAngle);

}

#endif /* VECMAT_H_ */
