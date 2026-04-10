/*
 * ControlBase.h
 *
 *  Created on: Jul 2, 2025
 *      Author: hor
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

#ifndef LIB_CONTROLS_CONTROLBASE_H_
#define LIB_CONTROLS_CONTROLBASE_H_

#include <memory>

#include "Uuid.h"
#include "Renderers/RendererBase.h"
#include "GLES/SDL/SDLRenderSurface.h"


// Forward declarations
namespace OevGLES {

/** \brief Stores all kind of useful resources for the appearance of controls and dialogs, and "global" pointers

	\see Find the definition in \ref RenderContext.h
 */
struct RenderContext;

} // namespace OevGLES

namespace OevControls {

using RenderContextSharedPtr = std::shared_ptr<OevGLES::RenderContext>;

// forward declarations
class ControlBase;
class ControlsContainer;


// smart pointer declarations
using ControlBasePtr     = std::shared_ptr<ControlBase>;
using ControlBaseWeakPtr = std::weak_ptr<ControlBase>;

using ControlsContainerPtr     = std::shared_ptr<ControlsContainer>;
using ControlsContainerWeakPtr = std::weak_ptr<ControlsContainer>;

class ControlBase /*: public OevGLES::RendererBase*/ {
public:

	struct PosPixel {
		/// \brief x goes to the left
		int xPixel = 0;
		/// \brief y goes from bottom to top, as usual in GL-world.
		int yPixel = 0;
		
		PosPixel operator + (const PosPixel& pos1) const {
			return PosPixel	{
				.xPixel = xPixel + pos1.xPixel,
				.yPixel = yPixel + pos1.yPixel
			};
		}
		PosPixel const & operator += (const PosPixel& pos1) {
			xPixel += pos1.xPixel;
			yPixel += pos1.yPixel;
			return *this;
		}
	};

	using SizePixel = OevGLES::SDLRenderSurface::SizePixel;

	ControlBase(ControlsContainerWeakPtr const &parent,
		std::weak_ptr<ControlBase> pointerToSelf,
		RenderContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name = ""
	);

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
		return relativePosition;
	}
	/// Moves \ref topRight accordingly but leaves
	/// \ref size unchanged.
	/// \see \ref position
	void setPosition (PosPixel const& position);

	/// \see \ref size
	auto getSize() const {
		return size;
	}
	/// Leaves \ref position unchanged, but adjusts \ref topRight accordingly
	/// \see \ref size
	void setSize (SizePixel const& size);

	/// \see \ref topRight
	auto getTopRight() const {
		return topRight;
	}

	/// \see \ref position
	auto getX() const {
		return relativePosition.xPixel;
	}
	/// \see \ref position
	auto getY() const {
		return relativePosition.yPixel;
	}
	/// \see \ref topRight
	auto getRight() const {
		return topRight.xPixel;
	}
	/// \see \ref topRight
	auto getTop() const {
		return topRight.yPixel;
	}
	/// \see \ref size
	auto getWidth() const {
		return size.widthPixel;
	}
	/// \see \ref size
	auto getHeight() const {
		return size.heightPixel;
	}

	/// Adjusts \ref size accordingly, but leaves \ref position unchanged.
	/// \see \ref topRight
	void setTopRight (PosPixel const& topRight);

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

	/// \see \ref isDefaultControl_
	auto isDefaultControl() {
		return isDefaultControl_;
	}
	/// \see \ref isDefaultControl_
	void setDefaultControl (bool isDefaultControl = false);

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
	void setTabPredecessor (ControlBaseWeakPtr const& tabPredecessor);
	
	/// \see \ref tabSuccessor
	auto getTabSuccessor () {
		return tabSuccessor;
	}
	/// \see \ref tabSuccessor
	void setTabSuccessor (ControlBaseWeakPtr const& tabSuccessor);
	
	/// \see \ref tabContainer
	auto getTabContainer () const {
		return tabContainer;
	}
	/// \see \ref tabContainer
	void setTabContainer (ControlsContainerWeakPtr const& tabContainer);

	bool hasFrame() const {return hasFrame_;}
	void setHasFrame(bool hasFrame);

	bool isRootControl() {
		return isRootControl_;
	}

	/** 
	 * You can modify the content of the shared pointer. You just cannot reset the
	 * share pointer in \p this.
	 */
	RenderContextSharedPtr const& getControlsContextPtr() const {
		return renderContextPtr;
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

	OevGLES::RenderStandardUniforms const& getRenderUniforms() {
		return renderUniforms;
	}

	/**
	 * One or more shared pointers of the parent uniforms have changed. Take over
	 * the shared ones from the parent, or re-calculate your own ones from the parent.
	 */
	virtual void onResetParentRenderUniforms(
		OevGLES::RenderStandardUniforms const &parentUniforms);

	/** \brief Recalculate the modelMatrix \ref renderUniforms when \ref posOrSizeDirty is true.
	 
	 When you override this method make sure to call the base class method here.
	 
	 Here modelMatrix of \ref renderUniforms is being re-calculated.
	 
	 \ref posOrSizeDirty is being reset.
	 */
	virtual void recalcSizePositionMatrix();
	
	/** \brief Setup the vertex arrays, calculate normals... and setup VBOs
	 *
	 * Pure virtual interface
	 *
	 */
	virtual void setupVertexBuffers () = 0;

	/** \brief Draw the rendered object.
	 *
	 * Pure virtual interface
	 *
	 */
	virtual void draw() = 0;

	/** \brief Re-calculate the absolute position
	 * 
	 * Add the own relative position and the parent's absolute position.
	 */
	void recalcAbsPosition();

protected:
	
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

	/** \brief A weak pointer to itself.
	 *
	 * Allows the control to pass a weak pointer to itself to others.
	 * Primary use it to pass a weak pointer to itself to event handlers.
	 */
	std::weak_ptr<ControlBase> pointerToSelf;
	
	/// \brief Relative (bottom right) position of the control relative to its \ref parent
	PosPixel relativePosition;
	/** \brief Absolute (bottom right) position of the control
	  *
	  * The absolute position is relative to the to the \ref OevGLES::SDLRenderSurface
	  * which defines the absolute 0,0 position at the bottom right of the drawable area.
	  */
	PosPixel absolutePosition = {0,0};
	
	/// \brief the bounding box around the control
	SizePixel size = {1,1};
	/// \brief Derived and redundant convenience coordinates based on \ref position and \ref size
	///
	/// \p topRight is like \ref position also relative to position of \ref parent.
	PosPixel topRight = {1,1};
	
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
	 * Used to quickly recalculate the position of the local model matrix from the parent's
	 * model matrix' positon. 
	 * 
	 * It contains the translation by the control's position.
	 */
	OevGLES::Vec3 locControlPosition = OevGLES::Vec3::Zero();
	
	/**
	 * Copy of \ref renderUniforms. The Model matrix is reduced on every side.
	 */
	OevGLES::RenderStandardUniforms renderFrameUniforms;
	
	/** \brief Remember the parent's model matrix.
	*/
	OevGLES::RenderStandardUniforms::Mat4WithChangeCounterPtr parentModelMatrixPtr;
	
	/// \brief A complete re-draw is due because content, position, size or visual attributes changed.
	bool dirty = true;
	
	/// \brief Control has the keyboard input focus
	bool hasFocus_ = false;
	
	bool visible = true;
	
	/** \brief The control is operable
	 *
	 * Inactive controls are visible (governed by \ref visible)
	 * but cannot get the input focus, do not react to mouse clicks,
	 * and appear inactive (typically greyish or mute colors)
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

	/** \brief Control has a 1-pixel wire-frame in the opposite fill color.
	 *
	 * Between wire frame and edge is one pixel, between frame and inner useful space is another pixel.
	 */
	bool hasFrame_ = false;
	
	/// \brief \p true when \ref hasFrame_ is true and when the control is large enough to fit a frame.
	bool doDrawFrame = false;

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
	
	/** \brief shared pointer to the context for all controls based on one \ref RootControl
	 * control for a \ref OevGLES::SDLRenderSurface.
	 *
	 */
	RenderContextSharedPtr renderContextPtr;
};

static constexpr auto s = sizeof(ControlBase);

template <typename ControlType>
std::shared_ptr<ControlType> makeControl (ControlsContainerWeakPtr const &parent,
		RenderContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name = ""
	) {
std::shared_ptr<ControlType> newControlPtr;

	// Now things are getting tricky:
	// I need a pointer of the object before it is constructed
	// because the parent of the root control is the root control itself.
	// Bring in the allocators
	using ControlAllocator = std::allocator<ControlType>;
	using ControlAllocatorTraits = std::allocator_traits<ControlAllocator>;
	ControlAllocator controlAllocator;
	
	ControlType* rawPtr = controlAllocator.allocate(1);

	newControlPtr.reset(rawPtr);
	
	ControlAllocatorTraits::construct(controlAllocator,rawPtr, parent,
										  newControlPtr, renderContextPtr,
										  uuid, name);

	return newControlPtr;

}

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLBASE_H_ */
