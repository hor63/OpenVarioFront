/*
 * Uuid.cpp
 *
 *  Created on: Aug 12, 2025
 *      Author: hor
 */
#include "fmt/base.h"
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <uuid.h>
#include <cstring>
#include "fmt/format.h"
#include "OVFCommon.h"
#include "ExceptionBase.h"

#include "Uuid.h"

namespace OevUtil {

Uuid::Uuid()
	:uuidString {nullUUIDString},
	 uuidBinary {0}
{
	
	static_assert(sizeof(uuid_t) == sizeof(uuidBinary));
	static_assert(numCharsUUIDString == UUID_STR_LEN);
	
}

Uuid::Uuid(char const *uuidString)
	:uuidString{uuidString},
	 uuidBinary {0}
{
	
	if (this->uuidString.length() != (numCharsUUIDString-1)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: UUID string {1} is not {2} bytes long.")),
			__PRETTY_FUNCTION__,
			this->uuidString,(numCharsUUIDString-1)
			);
		throw UuidException(errStr.c_str());
	}
	
	if (::uuid_parse(this->uuidString.c_str(), &uuidBinary[0]) != 0) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: UUID string {1} is not valid.")),
			__PRETTY_FUNCTION__,
			this->uuidString);
		throw UuidException(errStr.c_str());
	}
}

Uuid::Uuid(UuidBinaryT const & uuidBinary) :
	uuidBinary {uuidBinary},
	uuidString {nullUUIDString}
{
	::uuid_unparse(&uuidBinary[0], uuidString.data());
}

Uuid::~Uuid() {
}

Uuid::Uuid(Uuid &&other) :
	uuidBinary{other.uuidBinary},
	uuidString {std::move(other.uuidString)}
{

	other.uuidString = nullUUIDString;
	other.uuidBinary.fill(0);

}

Uuid& Uuid::operator=(Uuid &&other) {

	uuidBinary = other.uuidBinary;
	uuidString = std::move(other.uuidString);

	other.uuidString = nullUUIDString;
	other.uuidBinary.fill(0);

	return *this;
}

void Uuid::reset() {
	uuidString = nullUUIDString;
	uuidBinary.fill(0);
}

void Uuid::generateNewUUID() {
	
	::uuid_generate(&uuidBinary[0]);
	
	if (uuidString.length() != (numCharsUUIDString -1)) {
		uuidString = nullUUIDString;
	}
	::uuid_unparse(&uuidBinary[0], uuidString.data());

}
	
bool Uuid::isNull() const {
	
	return ::uuid_is_null(&uuidBinary[0]) != 0;
}
	
void Uuid::setUuidBinaryData (UuidBinaryT const & uuidBinary) {
	this->uuidBinary = uuidBinary;
	
	if (uuidString.length() != (numCharsUUIDString -1)) {
		uuidString = nullUUIDString;
	}
	::uuid_unparse(&uuidBinary[0], uuidString.data());
}
	
void Uuid::setUuidString (char const* uuidString) {

	this->uuidString = uuidString;
	
	if (this->uuidString.length() != (numCharsUUIDString-1)) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: UUID string \"{1}\" is not {2} bytes long.")),
			__PRETTY_FUNCTION__,
			this->uuidString,(numCharsUUIDString-1)
			);
		throw UuidException(errStr.c_str());
	}
	
	if (::uuid_parse(this->uuidString.c_str(), &uuidBinary[0]) != 0) {
		auto errStr = fmt::format(fmt::runtime(
			_("Error in {0}: String \"{1}\" is not a valid UUID.")),
			__PRETTY_FUNCTION__,
			this->uuidString);
		throw UuidException(errStr.c_str());
	}
}
	

} /* namespace OevUtil */
