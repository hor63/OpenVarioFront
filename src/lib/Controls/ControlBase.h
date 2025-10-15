/*
 * ControlBase.h
 *
 *  Created on: Jul 2, 2025
 *      Author: hor
 */

#ifndef LIB_CONTROLS_CONTROLBASE_H_
#define LIB_CONTROLS_CONTROLBASE_H_

#include <GLES2/gl2.h>
#include <cstdint>
#include <string>
#include <memory>

#include "Renderers/RendererBase.h"
#include "VecMat.h"
#include "Uuid.h"

namespace OevControls {

// forward declarations
class ControlBase;
class ControlsContainer;

// smart pointer declarations
using ControlBasePtr     = std::shared_ptr<ControlBase>;
using ControlBaseWeakPtr = std::weak_ptr<ControlBase>;

using ControlsContainerPtr     = std::shared_ptr<ControlsContainer>;
using ControlsContainerWeakPtr = std::weak_ptr<ControlsContainer>;

class ControlBase : public OevGLES::RendererBase {
public:

	struct Pos {
		/// \brief x goes to the left
		GLfloat x = 0;
		/// \brief y goes from bottom to top, as usual in GL-world.
		GLfloat y = 0;
	};

	struct Size {
		GLfloat width = 0;
		GLfloat height = 0;
	};

	ControlBase(ControlsContainerWeakPtr const &parent,
		OevUtil::Uuid const & uuid,
		char const* name = "");

	virtual ~ControlBase();
	// Copy constructors and assignment operators are explicitly prohibited. 
	ControlBase(const ControlBase &other) = delete;
	ControlBase(ControlBase &&other) = delete;
	ControlBase& operator=(const ControlBase &other) = delete;
	ControlBase& operator=(ControlBase &&other) = delete;

	/// \see \ref name
	auto const& getName() const {
		return name;
	};
	/// \see \ref name
	void setName(std::string const & name);

	/// \see \ref uuid
	auto const &getUuid() const {
		return uuid;
	}

	/// \see \ref parent
	auto const &getParent() const {
		return parent;
	}

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
	/// Leaves \ref position unchanged, but adjusts \ref topRight accordingly
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

	/// Adjusts \ref size accordingly, but leaves \ref position unchanged.
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
		return isDefaultControl_;
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
	void setTabPredecessor (ControlBaseWeakPtr tabPredecessor);
	
	/// \see \ref tabSuccessor
	auto getTabSuccessor () {
		return tabSuccessor;
	}
	/// \see \ref tabSuccessor
	void setTabSuccessor (ControlBaseWeakPtr tabSuccessor);
	
	/// \see \ref tabContainer
	auto getTabContainer () const {
		return tabContainer;
	}
	/// \see \ref tabContainer
	void setTabContainer (ControlsContainerWeakPtr tabContainer);

	bool isRootControl() {
		return isRootControl_;
	}

	// Callbacks upon changes or actions

	/** \brief Request to re-calculate the own model matrix or vertex arrays
	 * when the own size changed.
	 *
	 * For control containers like dialogs the layout of the child controls is
	 * warranted.
	 */
	virtual void onSizeChanged();
	/** \brief Request to re-calculate the own model matrix when the own position
	 * changed.
	 */
	virtual void onPositionChanged();
	/** \brief Request to re-calculate the own model matrix when the position of
	 * the parent changed.
	 *
	 * Assume that the shared pointers of the parent's projection, view and model matrix remain
	 * un-changed. Their values may of course change.
	 */
	virtual void onParentPositionChanged();

	/**
	 * One or more shared pointers of the parent uniforms have changed. Take over
	 * the shared ones from the parent, or re-calculate your own ones from the parent.
	 */
	virtual void onResetParentRenderUniforms(
		OevGLES::RenderStandardUniforms &parentUniforms);

private:
	
	/// \brief The name can be used for anything the control wishes to do with it
	std::string name;

	/// \brief UUID for import and export
	OevUtil::Uuid uuid;

	/** \brief Parent and owner of the control
	 *
	 * The root control points to itself, i.e. this == this->parent is
	 * the root control
	 */
	ControlsContainerWeakPtr parent;

	
	/// \brief Official (bottom right) position of the control relative to its \ref parent
	Pos position = {0,0};
	/// \brief the bounding box around the control
	Size size = {1,1};
	/// \brief Derived and redundant convenience coordinates based on \ref position and \ref size
	///
	/// \p topRight is like \ref position also relative to position of \ref parent.
	Pos topRight = {1,1};
	
	// Helpers for rendering
	
	/**
	 * Perspective and view matrixes are supposed to be shared across the entire
	 * controls hierarchy.
	 *
	 * The model matrix remains local. It is calculated from the parent's model matrix
	 * translated by the own position.
	 */ 
	OevGLES::RenderStandardUniforms renderUniforms;
	
	/**
	 * Used to quickly recalculate the local model matrix from the parent's
	 * model matrix. 
	 */
	OevGLES::Mat4 locTranslationMatrix = OevGLES::Mat4::Identity();
	
	/** \brief Remember the parent's model matrix.
	*/
	OevGLES::RenderStandardUniforms::Mat4WithChangeCounterPtr parentModelMatrixPtr;
	
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
	bool isDefaultControl_ = false;


	/// \brief is this control the root control?
	bool isRootControl_ = false;
	
	/** \brief Sequence of controls to receive the focus when switching with the TAB key.
	 *
	 * A tab sequence = 0 means the control is not part of the tab group
	 * and thus will never receive the focus when you switch focus with the TAB key.
	 */
	int tabSequence = 0;
	ControlBaseWeakPtr tabPredecessor;
	ControlBaseWeakPtr tabSuccessor;
	ControlsContainerWeakPtr tabContainer;
};

static constexpr auto s = sizeof(ControlBase);

} /* namespace OevControls */



#endif /* LIB_CONTROLS_CONTROLBASE_H_ */
