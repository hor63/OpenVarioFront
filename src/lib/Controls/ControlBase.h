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

#include "Renderers/RendererBase.h"
#include "VecMat.h"

namespace OevControls {

// forward declaration
class ControlsContainer;

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

	/// \see \ref name
	auto const& getName() const {
		return name;
	};
	/// \see \ref name
	void setName(std::string const & name);

	/// \see \ref uuid
	auto const getUuid() const {
		return uuid;
	}
	/// \see \ref uuid
	void setUuid (uuid_t const uuid);

	/// \see \ref parent
	ControlsContainer const* getParent() const {
		return parent;
	}
	/// \see \ref parent
	void setParent (ControlsContainer *parent);

	/// \see \ref position
	auto getPosition () const {
		return position;
	}
	/// Moves \ref topRight accordingly but leaves
	/// \ref size unchanged.
	/// \see \ref position
	void setPosition (Pos position);

	/// \see \ref size
	auto getSize() const {
		return size;
	}
	/// Leaves \ref pos unchanged, but adjusts \ref topRight accordingly
	/// \see \ref size
	void setSize (Size size);

	/// \see \ref topRight
	auto getTopRight() const {
		return topRight;
	}

	/// \see \ref position
	auto getX() const {
		return position.x;
	}
	/// \see \ref position
	auto getY() const {
		return position.y;
	}
	/// \see \ref topRight
	auto getRight() const {
		return topRight.x;
	}
	/// \see \ref topRight
	auto getTop() const {
		return topRight.y;
	}
	/// \see \ref size
	auto getWidth() const {
		return size.width;
	}
	/// \see \ref size
	auto getHeight() const {
		return size.height;
	}

	/// Adjusts \ref size accordingly, but leaves \ref pos unchanged.
	/// \see \ref topRight
	void setTopRight (Pos topRight);

	/// \see \ref foregroundColor
	auto const & getForegroundColor () const {
		return foregroundColor;
	}
	/// \see \ref foregroundColor
	void setForegroundColor (OevGLES::Vec4 const& foregroundColor);
	
	/// \see \ref backgroundColor
	auto const & getBackgroundColor () const {
		return backgroundColor;
	}
	/// \see \ref backgroundColor
	void setBackgroundColor (OevGLES::Vec4 const& backgroundColor);
	
	/// \see \ref posOrSizeDirty
	auto isPosOrSizeDirty() const {
		return posOrSizeDirty;
	}
	/// \see \ref posOrSizeDirty
	void setPosOrSizeDirty(bool posOrSizeDirty = true);

	/// \see \ref dirty
	auto isDirty() const {
		return dirty;
	}
	/// \see \ref dirty
	void setDirty(bool dirty = true);
	
	/// \see \ref hasFocus_
	auto hasFocus() const {
		return hasFocus_;
	}
	/// \see \ref hasFocus_
	void setHasFocus(bool hasFocus = true);
	
	/// \see \ref visible
	auto isVisible() const {
		return visible;
	}
	/// \see \ref visible
	void setVisible (bool visible = true);
	

	/// \see \ref active
	auto isActive() const {
		return active;
	}
	/// \see \ref active
	void setActive (bool active = true);
	
	/// \see \ref takesFocus
	auto canTakeFocus() const {
		return takesFocus;
	}
	/// \see \ref takesFocus
	void setCanTakeFocus (bool takesFocus = false);

	/// \see \ref defaultControl
	auto isDefaultControl() {
		return defaultControl;
	}
	/// \see \ref defaultControl
	void setDefaultControl (bool defaultControl = false);

	/// \see \ref tabSequence
	auto getTabSequence() {
		return tabSequence;
	}
	/// \see \ref tabSequence
	void setTabSequence(int tabSequence);
	
	/// \see \ref tabPredecessor
	auto getTabPredecessor() {
		return tabPredecessor;
	}
	/// \see \ref tabPredecessor
	void setTabPredecessor (ControlBase *tabPredecessor);
	
	/// \see \ref tabSuccessor
	auto getTabSuccessor () {
		return tabSuccessor;
	}
	/// \see \ref tabSuccessor
	void setTabSuccessor (ControlBase *tabSuccessor);
	
	/// \see \ref tabContainer
	auto getTabContainer () const {
		return tabContainer;
	}
	/// \see \ref tabContainer
	void setTabContainer (ControlsContainer *tabContainer);

	
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
	ControlsContainer *parent = nullptr;

	
	/// \brief Official (bottom right) position of the control
	Pos position;
	/// \brief the bounding box around the control
	Size size;
	/// \brief Derived and redundant convenience coordinates based on \ref position and \ref size
	Pos topRight;
	
	/// \brief Default is black on white
	OevGLES::Vec4 foregroundColor = {0,0,0,1};
	/// \brief Default is black on white
	OevGLES::Vec4 backgroundColor = {1,1,1,1};
	
	/// \brief Only position or size changed, but not content.
	bool posOrSizeDirty = true;
	
	/// \brief A complete re-draw is due because content, position, size or visual attributes changed.
	bool dirty = true;
	
	/// \brief Control has the keyboard input focus
	bool hasFocus_ = false;
	
	bool visible = true;
	
	/** \brief The control is operable
	 *
	 * Inactive controls are visible (governed by \ref visible)
	 * but cannot get the input focus, do not react to mouse clicks,
	 * and appear incactive (typically greyish or mute colors)
	 *
	 */
	bool active = true;
	
	/** \brief Can this control take the focus?
	 *
	 * If the control can take the focus it can also become part of the
	 * tab group.
	 */
	bool takesFocus = false;
	
	/// \brief This control receives the equivalent of a click when you hit enter in a dialog.
	bool defaultControl = false;

	/** \brief Sequence of controls to receive the focus when switching with the TAB key.
	 *
	 * A tab sequence = 0 means the control is not part of the tab group
	 * and thus will never receive the focus when you switch focus with the TAB key.
	 */
	
	int tabSequence = 0;
	ControlBase *tabPredecessor = nullptr;
	ControlBase *tabSuccessor = nullptr;
	ControlsContainer *tabContainer = nullptr;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLBASE_H_ */
