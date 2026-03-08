/*
 * Uuid.cpp
 *
 *  Created on: Aug 12, 2025
 *      Author: hor
 */
#include "OVFCommon.h"
#include "ExceptionBase.h"

#include <uuid.h>
#include "Uuid.h"

namespace OevUtil {

static const Uuid nullUuid;

Uuid::Uuid(char const *uuidString) {
	
	if (strlen(uuidString) != (NumCharsUUIDString-1)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: UUID string \"{1}\" is not {2} bytes long.")),
			__PRETTY_FUNCTION__,
			uuidString,(NumCharsUUIDString-1)
			);
		throw UuidException(errStr.c_str());
	}
	
	if (!checkUuidString(uuidString) ||
		(::uuid_parse(uuidString, uuidBinary.data()) != 0)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: String \"{1}\" is not a valid UUID.")),
			__PRETTY_FUNCTION__,
			uuidString);
		throw UuidException(errStr.c_str());
	}
	
	this->uuidString = uuidString;
	
}


Uuid::Uuid(Uuid &&other) :
	uuidBinary{other.uuidBinary},
	uuidString{other.uuidString}
{
	other = nullUuid;
}

Uuid::Uuid(UuidBinaryT const & uuidBinary) :
	uuidBinary {uuidBinary}
{
	// these static asserts are within this method because uuidBinary is private.
	// Otherwise I would have placed them outside a method.]
	static_assert(sizeof(uuid_t) == sizeof(uuidBinary));
	static_assert(NumCharsUUIDString == UUID_STR_LEN);
	
	::uuid_unparse(uuidBinary.data(), uuidString.data());
}

Uuid& Uuid::operator=(Uuid &&other) {

	uuidString = other.uuidString;
	uuidBinary = other.uuidBinary;
	
	other = nullUuid;

	return *this;
}

void Uuid::reset() {
	
	*this = nullUuid;
}

void Uuid::generateNewUUID() {
	
	::uuid_generate(uuidBinary.data());
	
	::uuid_unparse(uuidBinary.data(), uuidString.data());

}
	
bool Uuid::isNull() const {
	
	return ::uuid_is_null(uuidBinary.data()) != 0;
}
	
void Uuid::setUuidBinaryData (UuidBinaryT const & uuidBinary) {
	this->uuidBinary = uuidBinary;
	
	::uuid_unparse(uuidBinary.data(), uuidString.data());
}
	
void Uuid::setUuidString (char const* uuidString) {

	if (strlen(uuidString) != (NumCharsUUIDString-1)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: UUID string \"{1}\" is not {2} bytes long.")),
			__PRETTY_FUNCTION__,
			uuidString,(NumCharsUUIDString-1)
			);
		throw UuidException(errStr.c_str());
	}
	
	if (!checkUuidString(uuidString) ||
		(::uuid_parse(uuidString, uuidBinary.data()) != 0)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: String \"{1}\" is not a valid UUID.")),
			__PRETTY_FUNCTION__,
			uuidString);
		throw UuidException(errStr.c_str());
	}
	
	this->uuidString = uuidString;
	
}
	

} /* namespace OevUtil */
