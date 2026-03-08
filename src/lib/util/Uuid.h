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

namespace OevUtil {


/** \brief Normal library functions are not constexpr.
 *
 * This function however is. Since it is being call at compile time,
 * screw performance concerns.
 */

constexpr int hexCharToNum (char c) {
	
	int res = -1;
	
	if (c >= '0' && c <= '9') {
		res = c - '0';
	}
	if (c >= 'a' && c <= 'f') {
		res = c - 'a' + 10;
	}
	if (c >= 'A' && c <= 'F') {
		res = c - 'A' + 10;
	}

	return res;
}


/** \brief Wrapper around [libuuid](https://linux.die.net/man/3/libuuid)
 *
 * This class completely hides the hideous API of libuuid with its use of a raw
 * array of unsigned char.
 *
 * I am letting the system choose the best algorithm for generating new IIUDs.
 * If you want more control you can use libuuid directly, and use one of the
 * constructors with a defined UUID value.
 *
 */
class Uuid final {
public:


	static constexpr int NumBytesUUID = 16;
	/// \brief The number of bytes in the UUID string including the terminating'\0'
	static constexpr int NumCharsUUIDString = 37;
		
	using UuidBinaryT = std::array<unsigned char,NumBytesUUID>;
	using UuidStringArrayT = std::array<char,NumCharsUUIDString>;

	/// \brief constructs a NULL UUID
	Uuid() = default;
	/** \brief Constructs a UUID from a string in the form
	 * xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx
	 */
	Uuid(char const *uuidString);
	Uuid(const Uuid &other) = default;
	/// \brief Move constructor; other gets the NULL-UUID assigned.
	Uuid(Uuid &&other);
	/// \brief Constructs a UUIID from binary UUID data
	Uuid(UuidBinaryT const & uuidBinary);
	Uuid& operator=(const Uuid &other) = default;
	/// \brief Move assignment; other gets the NULL-UUID assigned.
	Uuid& operator=(Uuid &&other);
	
	/// \brief Reset the UUID to a NULL-UUID, i.e. to
	/// "00000000-0000-0000-0000-000000000000"
	void reset();
	/// \brief generate a new UUID for this.
	void generateNewUUID();
	
	/// \brief Is the UUID 0-values, i.e. is it the NULL-UUID = 
	/// "00000000-0000-0000-0000-000000000000"?
	bool isNull() const;
	
	UuidBinaryT const & getUuidBinaryData() const {
		return uuidBinary;
	}
	
	/// Automatically updates \ref uuidString too.
	void setUuidBinaryData (UuidBinaryT const & uuidBinary);
	
	std::string const& getUuidString() const {
		return uuidString;
	}
	
	void setUuidString (char const* uuidString);
	
	bool operator == (Uuid const& u1) const{
		return uuidBinary == u1.uuidBinary;
	}

	/// \brief can be used to produce the binary UUID raw data
	/// from a UUID string which you may produce with uuidgen.
	static constexpr Uuid::UuidBinaryT strToUuidBinary (char const *str) {
		Uuid::UuidBinaryT res;
		
		int k = 0;
		
		for (int i = 0; i < Uuid::NumCharsUUIDString -1;) {
			if (i == 8 || i == 13 || i == 18 || i == 23){
				// Here are the '-' separators in a UUID string.
				// so I am skipping over these.
				++i;
			} else {
				res[k] = hexCharToNum(str[i]) * 16;
				++i;
				res[k] = res[k] + hexCharToNum(str[i]);
				++k;
				++i;
			}
		}
		
		return res;
	}

	/// \brief check if
	static constexpr bool checkUuidString (char const *str) {
		bool res = true;
		int i = 0;
		for (; i < Uuid::NumCharsUUIDString -1;) {
			if (i == 8 || i == 13 || i == 18 || i == 23){
				res = res && (str[i] == '-');
				++i;
			} else {
				res = res && (hexCharToNum(str[i]) >= 0);
				++i;
			}
		}
	
		res = res && (str[i] == '\0');
	
		return res;
	}
	
private:
	std::string uuidString = "00000000-0000-0000-0000-000000000000";
	UuidBinaryT uuidBinary = {0};
};

} /* namespace OevUtil */

template<>
struct std::hash<OevUtil::Uuid>
{
    std::size_t operator()(const OevUtil::Uuid& uuid) const noexcept
    {
        std::size_t h = std::hash<std::string_view>{}(uuid.getUuidString());
        return h;
    }
};

#endif /* LIB_UTIL_UUID_H_ */
