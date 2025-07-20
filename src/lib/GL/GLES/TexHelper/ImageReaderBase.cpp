/*
 * ImageReaderBase.cpp
 *
 *  Created on: Jul 20, 2025
 *      Author: hor
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "lib/GL/GLES/TexHelper/ImageReaderBase.h"

namespace OevGLES {


#if defined HAVE_LOG4CXX_H
static log4cxx::LoggerPtr logger = 0;
#endif


ImageReaderBase::ImageReaderBase(std::string const &fileName)
	:fileName{fileName} {
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.ImageReaderBase");
	}
#endif

		LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ", fileName = " << this->fileName);
}

ImageReaderBase::ImageReaderBase(
	char const *memLocation,
	int memLength,
	std::string const &imageName)
	:memLocation{memLocation},
	memLength{memLength},
	fileName{imageName} 
{
#if defined HAVE_LOG4CXX_H
	if (!logger) {
		logger = log4cxx::Logger::getLogger("OpenVarioFront.ImageReaderBase");
	}
#endif

	LOG4CXX_DEBUG(logger,__PRETTY_FUNCTION__
		<< ": memLocation = "
		<< reinterpret_cast<void const *>(this->memLocation)
		<< ", lenPNGData = " << this->memLength
		<< ", fileName = " << this->fileName);
}


ImageReaderBase::~ImageReaderBase() {

}

} /* namespace OevGLES */
