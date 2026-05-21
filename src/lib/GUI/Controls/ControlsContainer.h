/*
 * ControlsContainer.h
 *
 *  Created on: Aug 14, 2025
 *      Author: hor
 *
 *	Base class for any control which can host other controls, and a tab group,
 *  like dialogs, canvases, and the root control
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

#ifndef LIB_CONTROLS_CONTROLSCONTAINER_H_
#define LIB_CONTROLS_CONTROLSCONTAINER_H_

#include "ControlBase.h"

namespace OevControls {

class ControlsContainer: public ControlBase {
public:

	using ControlBaseSharedPointerListT = std::list<ControlBaseSharedPtr>;
	using ControlBaseWeakPtrListT = std::list<ControlBaseWeakPtr>;

	ControlsContainer(ControlsContainerWeakPtr  const &parent,
		std::weak_ptr<ControlsContainer> pointerToSelf,
		RenderContextSharedPtr const& controlsContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name = "")
		:ControlBase(parent,
			pointerToSelf,
			controlsContextPtr,
			uuid,
			name)
	{}
	virtual ~ControlsContainer();
	ControlsContainer(const ControlsContainer &other) = delete;
	ControlsContainer(ControlsContainer &&other) = delete;
	ControlsContainer& operator=(const ControlsContainer &other) = delete;
	ControlsContainer& operator=(ControlsContainer &&other) = delete;
	
	void addControl(ControlBaseSharedPtr const &controlPtr);
	void appendControlToTabGroup (ControlBaseWeakPtr const &controlWeakPtr);
	void insertControlInTabGroupBefore (ControlBaseWeakPtrListT::iterator ref,
		ControlBaseWeakPtr const &controlWeakPtr);
	
	virtual void setupVertexBuffers () override;
	virtual void draw() override;

	// overridden callbacks upon changes or actions
	
	/** \brief Request to re-calculate the own model matrix when the own position
	 * changed.
	 */
	virtual void onPositionChanged() override;
	/** \brief Request to re-calculate the own model matrix when the position of
	 * the parent changed.
	 *
	 * Assume that the shared pointers of the parent's projection, view and model matrix remain
	 * un-changed. Their values may of course change.
	 */
	virtual void onParentPositionChanged() override;

	/**
	 * One or more shared pointers of the parent uniforms have changed. Take over
	 * the shared ones from the parent, or re-calculate your own ones from the parent.
	 */
	virtual void onResetParentRenderUniforms(
		OevGLES::RenderStandardUniforms const &parentUniforms) override;
	
	/// \see processControlsTree()
	enum TreeProcessOrder {ChildrenFirst,SelfFirst} ;
	
	/** \brief Execute a functor \p f on the controls tree.
	 *
	 * Traverse the entire control tree. Call the functor \p f
	 * on each control until the functor returns \p true.
	 *
	 * \param f Functor or lambda with return type bool, and a parameter of type \ref ControlBase*.
	 	When the functor returns \p true the tree traversing stops, and this function returns \p true.
	 * \param processingOrder When \ref ChildrenFirst the function descends recursively down the tree first.
	 *	When \ref SelfFirst the functor \p f is called for \p this first before descending the controls tree.
	 *
	 * \return \p true when traversing the tree shall stop. \p false when traversing the tree shall continue.
	 *	The return value is determined by the return value of the functor \p f on the controls.
	 */
	template <typename Functor>
	auto processControlsTree (Functor &f,TreeProcessOrder processingOrder) {
		struct {bool processingIsDone = false;std::weak_ptr<ControlBase> controlThatProcessed;} result;
		
		if (processingOrder == SelfFirst && f(this)) {
			result.processingIsDone = true;
			result.controlThatProcessed = pointerToSelf;
		} else {
			for (auto &i: controlsList) {
				auto childContainer = 
					dynamic_cast<ControlsContainer*>(i.get());
					
				if (childContainer != nullptr) {
					result = childContainer->processControlsTree(f,processingOrder);
					if (result.processingIsDone) {
						break;
					} else {
						if (f(i.get())) {
							result.processingIsDone = true;
							result.controlThatProcessed = i->getPointerToSelf();
							break;
						}
					}
				}
			}
		}
		
		if (!result.processingIsDone && processingOrder == ChildrenFirst) {
			if (f(this)) {
				result.processingIsDone = true;
				result.controlThatProcessed = pointerToSelf;
			}
		}
		
		return result;
	}

protected:

/** \brief List of controls owned by this \p ControlsContainer.
 */
ControlBaseSharedPointerListT controlsList;

/** \brief List of controls in a tab group within this container.
 *
 * Only controls which can receive input focus are being listed here.
 */
ControlBaseWeakPtrListT tabGroup;

}; // class ControlsContainer

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLSCONTAINER_H_ */
