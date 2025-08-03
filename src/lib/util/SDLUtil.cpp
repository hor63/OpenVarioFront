/*
 * SDLUtil.cpp
 *
 *  Created on: Aug 3, 2025
 *      Author: hor
 *
 *  Mostly error handling support for SDL calls
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

#include "SDLUtil.h"

#include <SDL3/SDL_error.h>

#include "fmt/base.h"
#include "fmt/format.h"

#include "OVFCommon.h"

#include "ExceptionBase.h"

namespace OevGLES {
	
void reportSDLError( std::source_location const& sourceLocation,char const *sdlFunctionName) {
	std::string sdlErrorMsg;
	auto sdlErrorMsgCStr = SDL_GetError();

	if (sdlErrorMsgCStr == nullptr) {
		sdlErrorMsg = _("No SDL error message found");
		sdlErrorMsgCStr = sdlErrorMsg.c_str();
	}

	auto errorText = fmt::format (fmt::runtime(_(
		"An error occurred in SDL function {0} at {1}:{2} in function {3}. Message:\n{4}"
		)),
		sdlFunctionName,
		sourceLocation.file_name(),sourceLocation.line(),sourceLocation.function_name(),
		sdlErrorMsgCStr);
	
	throw SDLException (errorText.c_str());
}

} // namespace OevGLES {



