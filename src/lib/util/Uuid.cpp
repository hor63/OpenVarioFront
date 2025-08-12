/*
 * Uuid.cpp
 *
 *  Created on: Aug 12, 2025
 *      Author: hor
 */

#include <uuid.h>

#include "Uuid.h"
#include <cstring>

namespace OevUtil {

Uuid::Uuid()
	:uuidString {"00000000-0000-0000-0000-000000000000"},
	 uuidBinary {0}
{
	
	static_assert(sizeof(uuid_t) == sizeof(uuidBinary));
	static_assert(numCharsUUIDString == UUID_STR_LEN);
	
}

Uuid::Uuid(char const *uuidString) {
	
}

Uuid::Uuid(UuidBinaryT const & uuidBinary){
	
}

Uuid::~Uuid() {
}

Uuid::Uuid(Uuid &&other) {
	// TODO Auto-generated constructor stub

}

Uuid& Uuid::operator=(Uuid &&other) {

	return *this;
}

} /* namespace OevUtil */
