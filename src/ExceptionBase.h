/*
 *  ExceptionBase.h
 *
 *  Created on: Apr 24, 2018
 *      Author: kai_horstmann
 *
 *  Exception classes which can be thrown throughout this program
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

#ifndef SRC_EXCEPTIONBASE_H_
#define SRC_EXCEPTIONBASE_H_

#include <exception>
#include <string>

namespace OpenVarioFront {

class ExceptionBase :public std::exception {
public:
	ExceptionBase(char const *description)
		:description {description}
		{}
	virtual ~ExceptionBase();

	 virtual const char*
	    what() const noexcept override;

private:
	 std::string description;
};

} /* namespace OpenVarioFront */

#endif /* SRC_EXCEPTIONBASE_H_ */
