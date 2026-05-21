/*
 * PosPixel.h
 *
 *  Created on: Apr 27, 2026
 *      Author: hor
 */

#ifndef LIB_GUI_CONTROLS_POSPIXEL_H_
#define LIB_GUI_CONTROLS_POSPIXEL_H_


struct PosPixel {
	/// \brief x goes to the left
	int xPixel = 0;
	/// \brief y goes from bottom to top, as usual in OpenGL-world.
	int yPixel = 0;
	
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
}; // struct PosPixel



#endif /* LIB_GUI_CONTROLS_POSPIXEL_H_ */
