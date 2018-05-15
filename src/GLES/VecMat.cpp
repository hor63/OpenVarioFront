/*
 * VecMat.cpp
 *
 *  Created on: May 15, 2018
 *      Author: hor
 *
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif


#include "VecMat.h"
#include "OVFCommon.h"

namespace OevGLES {

#if defined HAVE_LOG4CXX_H
    static log4cxx::LoggerPtr logger = 0;

#endif

static void initLogger() {
#if defined HAVE_LOG4CXX_H
	if (logger == 0) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.VecMat");
	}
#else
	;
#endif
}


Mat4 translationMatrix (GLfloat x, GLfloat y, GLfloat z ) {
	Mat4 rc;

	initLogger();

	rc << 	1.0f, 0.0f, 0.0f, x,
			0.0f, 1.0f, 0.0f, y,
			0.0f, 0.0f, 1.0f, z,
			0.0f, 0.0f, 0.0f, 1.0f;

	LOG4CXX_DEBUG(logger,"translationMatrix (x = " << x << ", y " << y << ", z = " << z << ") =\n" << rc );

	return rc;
}

Mat4 scalingMatrix (GLfloat x, GLfloat y, GLfloat z ) {
	Mat4 rc;

	initLogger();

	rc << 	x   , 0.0f, 0.0f, 0.0f,
			0.0f, y   , 0.0f, 0.0f,
			0.0f, 0.0f, z   , 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f;

	LOG4CXX_DEBUG(logger,"scalingMatrix (x = " << x << ", y " << y << ", z = " << z << ") =\n" << rc );

	return rc;
}

Mat4 rotationMatrixX (GLfloat adX) {
	Mat4 rc;
	GLfloat sinX = sinf(adX/180.0*M_PI);
	GLfloat cosX = sinf(adX/180.0*M_PI);

	initLogger();


	/*
	 * 		1		0		0		0
	 * 		0		cos(θ)	−sin(θ)	0
	 * 		0		sin(θ)	cos(θ)	0
	 * 		0		0		0		1
	 */

	rc << 	1.0f,	0.0f,	0.0f,	0.0f,
			0.0f,	cosX,	-sinX,	0.0f,
			0.0f,	sinX,	cosX,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f;

	LOG4CXX_DEBUG(logger,"rotationMatrixX (adX = " << adX << ") =\n" << rc );

	return rc;
}

Mat4 rotationMatrixY (GLfloat adY) {
	Mat4 rc;
	GLfloat sinY = sinf(adY/180.0*M_PI);
	GLfloat cosY = sinf(adY/180.0*M_PI);

	initLogger();


	/*
	 * 		cos(θ)	0		sin(θ)	0
	 * 		0		1		0		0
	 * 		−sin(θ)	0		cos(θ)	0
	 * 		0		0		0		1
	 */

	rc << 	cosY,	0.0f,	sinY,	0.0f,
			0.0f,	1.0f,	0.0f,	0.0f,
		   -sinY,	0.0f,	cosY,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f;

	LOG4CXX_DEBUG(logger,"rotationMatrixY (adY = " << adY << ") =\n" << rc );

	return rc;
}

Mat4 rotationMatrixZ (GLfloat adZ) {
	Mat4 rc;
	GLfloat sinZ = sinf(adZ/180.0*M_PI);
	GLfloat cosZ = sinf(adZ/180.0*M_PI);

	initLogger();


	/*
	 * 		cos(θ)	−sin(θ)	0		0
	 * 		sin(θ)	cos(θ)	0		0
	 * 		0		0		1		0
	 * 		0		0		0		1
	 */

	rc <<	cosZ,	-sinZ,	0.0f,	0.0f,
			sinZ,	cosZ,	0.0f,	0.0f,
			0.0f,	0.0f,	1.0f,	0.0f,
			0.0f,	0.0f,	0.0f,	1.0f;

	LOG4CXX_DEBUG(logger,"rotationMatrixZ (adZ = " << adZ << ") =\n" << rc );

	return rc;
}

Mat4 viewMatrix (Vec3 const& camPos, Vec3 const &lookAt, Vec3 const & up) {
	Mat4 rc;
	Vec3 forwardVec = (lookAt - camPos).normalized();
	Vec3 rightVec = forwardVec.cross(up.normalized());
	Vec3 myUp = rightVec.cross(forwardVec).normalized();

	initLogger();

	/*
	 *
	 * V= {
	 * Rx	Ry	Rz	−Px
	 * Ux	Uy	Uz	−Py
	 * −Fx	−Fy	−Fz	−Pz
	 * 0	0	0	1
	 * Where U is a vector pointing up, F forward, and P is world position of camera
	 * R is the right vector. This is the cross product of
	 *
	 */

	rc <<	rightVec(0),	rightVec(1),	rightVec(2),	-camPos(0),
			myUp(0),		myUp(1),		myUp(2),		-camPos(1),
			-forwardVec(0),	-forwardVec(1),	-forwardVec(2),	-camPos(2),
			0.0f,			0.0f,			0.0f,			1.0f;

	LOG4CXX_DEBUG(logger,"viewMatrix (camPos = \n" << camPos << ", lookAt = \n" << lookAt << ", up = \n" << up<< ")" );
	LOG4CXX_DEBUG(logger,"viewMatrix: forwardVec = \n" << forwardVec);
	LOG4CXX_DEBUG(logger,"viewMatrix: rightVec = \n" << rightVec);
	LOG4CXX_DEBUG(logger,"viewMatrix: myUp = \n" << myUp);
	LOG4CXX_DEBUG(logger,"viewMatrix: rc = \n" << rc);
	return rc;
}

Mat4 projectionMatrix (GLfloat near, GLfloat far, GLfloat aspect, GLfloat fieldOfViewAngle) {
	/*
	 *
	 * range = tan(fov/2) ∗ near
	 * Sx = (2 ∗ near)/(range ∗ aspect + range ∗ aspect)
	 * Sy = near/range
	 * Sz = −(far + near)/(far − near)
	 * Pz = −(2 ∗ far ∗ near)/(far − near)
	 *
	 * P = {
	 * Sx 0 0 0
	 * 0 Sy 0 0
	 * 0 0 Sz Pz
	 * 0 0 −1 0
	 * }
	 */

	Mat4 rc;
	GLfloat range  = tanf((fieldOfViewAngle / 180.0 * M_PI) / 2.0f) * near;
	GLfloat Sx = (2.0f * near) / (range * aspect + range * aspect);
	GLfloat Sy = near / range;
	GLfloat Sz = -(far + near) / (far - near);
	GLfloat Pz = -(2.0f * far * near) / (far - near);

	initLogger();

	rc <<	Sx,		0.0f,	0.0f,	0.0f,
			0.0f,	Sy,		0.0f,	0.0f,
			0.0f,	0.0f,	Sz,		Pz,
			0.0f,	0.0f,	-1.0f,	0.0f;

	LOG4CXX_DEBUG(logger,"projectionMatrix (near = " << near << ", far = " << far << ", aspect = " << aspect << ", fieldOfViewAngle = " << fieldOfViewAngle << ")" );
	LOG4CXX_DEBUG(logger,"projectionMatrix: range = " << range);
	LOG4CXX_DEBUG(logger,"projectionMatrix = \n" << rc);

	return rc;

}


} /* namespace OevGLES */
