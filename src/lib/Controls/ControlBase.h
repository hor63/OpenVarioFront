/*
 * ControlBase.h
 *
 *  Created on: Jul 2, 2025
 *      Author: hor
 */

#ifndef LIB_CONTROLS_CONTROLBASE_H_
#define LIB_CONTROLS_CONTROLBASE_H_

#include <cstdint>

#include <Renderers/RendererBase.h>

namespace OevControls {


class ControlBase : public OevGLES::RendererBase {
public:

	struct Pos {
		/// \brief x goes to the left
		int32_t x = 0;
		/// \brief y goes from bottom to top, as usual in GL-world.
		int32_t y = 0;
	};

	struct Size {
		int32_t width = 0;
		int32_t height = 0;
	};

	ControlBase();
	virtual ~ControlBase();

	auto getPosition () {
		return position;
	}
	/// Moves \ref topRight accordingly but leaves
	/// \ref size unchanged.
	void setPosition (Pos newPosition);

	auto getSize() {
		return size;
	}
	/// Leaves \ref pos unchanged, but adjusts \ref topRight accordingly
	void setSize (Size newSize);

	auto getTopRight() {
		return topRight;
	}

	auto getX() {
		return position.x;
	}
	auto getY() {
		return position.y;
	}
	auto getRight() {
		return topRight.x;
	}
	auto getTop() {
		return topRight.y;
	}
	auto getWidth() {
		return size.width;
	}
	auto getHeight() {
		return size.height;
	}

	/// Adjusts \ref size accordingly, but leaves \ref pos unchanged.
	void setTopRight (Pos newTopRight);

	auto isPosOrSizeDirty() {
		return posOrSizeDirty;

	}


private:

	/// \brief Official (bottom right) position of the control
	Pos position;
	/// \brief the bounding box around the control
	Size size;
	/// \brief Derived and redundant convenience coordinates based on \ref position and \ref size
	Pos topRight;
	
	bool posOrSizeDirty = true;
	
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLBASE_H_ */
