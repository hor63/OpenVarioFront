/*
 * Uuid.h
 *
 *  Created on: Aug 12, 2025
 *      Author: hor
 *
 *	Wrap libuuid into a type-safe and user friendly class
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

#ifndef LIB_UTIL_UUID_H_
#define LIB_UTIL_UUID_H_

#include <string>
#include <array>

namespace OevUtil {

class Uuid final {
public:


	static constexpr int numBytesUUID = 16;
	static constexpr int numCharsUUIDString = 37;

	using UuidBinaryT = std::array<unsigned char,numBytesUUID>;

	Uuid();
	Uuid(char const *uuidString);
	Uuid(UuidBinaryT const & uuidBinary);
	virtual ~Uuid();
	Uuid(const Uuid &other) = delete;
	Uuid(Uuid &&other);
	Uuid& operator=(const Uuid &other) = delete;
	Uuid& operator=(Uuid &&other);
	
private:

	std::string uuidString;
	UuidBinaryT uuidBinary;
};

} /* namespace OevUtil */

#endif /* LIB_UTIL_UUID_H_ */
