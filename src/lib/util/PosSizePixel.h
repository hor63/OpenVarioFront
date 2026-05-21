/*
 * PosSizePixel.h
 *
 *  Created on: Apr 27, 2026
 *      Author: hor
 */

#ifndef LIB_UTIL_POS_SIZE_PIXEL_H_
#define LIB_UTIL_POS_SIZE_PIXEL_H_

#include <cstdint>

namespace OevGLES {

struct SizePixel {
	int32_t widthPixel = 0;
	std::int32_t heightPixel = 0;
	
	SizePixel operator + (SizePixel const & size1) const {
		return SizePixel {
			.widthPixel = widthPixel + size1.widthPixel,
			.heightPixel = heightPixel + size1.heightPixel
		};
	}
	SizePixel operator - (SizePixel const & size1) const {
		return SizePixel {
			.widthPixel = widthPixel - size1.widthPixel,
			.heightPixel = heightPixel - size1.heightPixel
		};
	}
	SizePixel & operator += (SizePixel const & size1) {
		widthPixel += size1.widthPixel;
		heightPixel += size1.heightPixel;
		return *this;
	}
	SizePixel & operator -= (SizePixel const & size1) {
		widthPixel -= size1.widthPixel;
		heightPixel -= size1.heightPixel;
		return *this;
	}
};

struct PosPixel {
	/// \brief x goes to the left
	int32_t xPixel = 0;
	/// \brief y goes from bottom to top, as usual in OpenGL-world.
	int32_t yPixel = 0;
	
	PosPixel operator + (const PosPixel& pos1) const {
		return PosPixel	{
			.xPixel = xPixel + pos1.xPixel,
			.yPixel = yPixel + pos1.yPixel
		};
	}
	PosPixel operator - (const PosPixel& pos1) const {
		return PosPixel	{
			.xPixel = xPixel - pos1.xPixel,
			.yPixel = yPixel - pos1.yPixel
		};
	}
	PosPixel & operator += (const PosPixel& pos1) {
		xPixel += pos1.xPixel;
		yPixel += pos1.yPixel;
		return *this;
	}
	PosPixel & operator -= (const PosPixel& pos1) {
		xPixel -= pos1.xPixel;
		yPixel -= pos1.yPixel;
		return *this;
	}

	// Algebra with sizes.
	PosPixel operator + (const SizePixel& size1) const {
		return PosPixel	{
			.xPixel = xPixel + size1.widthPixel,
			.yPixel = yPixel + size1.heightPixel
		};
	}
	PosPixel operator - (const SizePixel& size1) const {
		return PosPixel	{
			.xPixel = xPixel - size1.widthPixel,
			.yPixel = yPixel - size1.heightPixel
		};
	}
	
	PosPixel & operator += (const SizePixel& size1) {
		xPixel += size1.widthPixel;
		yPixel += size1.heightPixel;
		return *this;
	}
	PosPixel operator -= (const SizePixel& size1) {
		xPixel -= size1.widthPixel;
		yPixel -= size1.heightPixel;
		return *this;
	}
	

}; // struct PosPixel

} // namespace OevGLES

namespace OevControls {

	using OevGLES::PosPixel;
	using OevGLES::SizePixel;
} // namespace OevControls

#endif /* LIB_UTIL_POS_SIZE_PIXEL_H_ */
