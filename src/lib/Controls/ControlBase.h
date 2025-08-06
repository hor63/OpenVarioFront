/*
 * ControlBase.h
 *
 *  Created on: Jul 2, 2025
 *      Author: hor
 */

#ifndef LIB_CONTROLS_CONTROLBASE_H_
#define LIB_CONTROLS_CONTROLBASE_H_

#include <cstdint>
#include <string>
#include <uuid.h>

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

	/// \see \ref posOrSizeDirty
	auto isPosOrSizeDirty() {
		return posOrSizeDirty;
	}
	/// \see \ref posOrSizeDirty
	void setPosOrSizeDirty(bool isPosOrSizeDirty = true);

	/// \see \ref dirty
	auto isDirty(){
		return dirty;
	}
	/// \see \ref dirty
	void setDirty(bool isDirty = true);
	
	
private:
	
	/// \brief The name can be used for anything the control wishes to do with it
	std::string name;

	/// \brief UUID for import and export
	uuid_t uuid = {0};

	/** \brief Parent and owner of the control
	 *
	 * The root control points to itself, i.e. this == this->parent is
	 * the root control
	 */
	ControlBase *parent = nullptr;

	
	/// \brief Official (bottom right) position of the control
	Pos position;
	/// \brief the bounding box around the control
	Size size;
	/// \brief Derived and redundant convenience coordinates based on \ref position and \ref size
	Pos topRight;
	
	/// \brief Only position or size changed, but not content.
	bool posOrSizeDirty = true;
	
	/// \brief A complete re-draw is due because content, position, size or visual attributes changed.
	bool dirty = true;
	
	/// \brief Control has the keyboard input focus
	bool hasFocus = false;
	
	bool visible = true;
	
	/** \brief The control is operable
	 *
	 * Inactive controls are visible (governed by \ref visible)
	 * but cannot get the input focus, do not react to mouse clicks,
	 * and appear incactive (typically greyish or mute colors)
	 *
	 */
	bool active = true;

	int tabOrder = 0;
	ControlBase *tabPredecessor = nullptr;
	ControlBase *tabSuccessor = nullptr;
	ControlBase *tabContainer = nullptr;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLBASE_H_ */
