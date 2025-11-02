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

#include <unordered_map>
#include <list>

#include "ControlBase.h"
#include "Uuid.h"

namespace OevControls {

class ControlsContainer: public ControlBase {
public:

	using ControlsMapT = std::unordered_map<OevUtil::Uuid, ControlBasePtr>;
	using ControlsWeakListT = std::list<ControlBaseWeakPtr>;

	ControlsContainer(ControlsContainerWeakPtr  const &parent,
		RendererContextSharedPtr const& controlsContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name = "")
		:ControlBase(parent,
			controlsContextPtr,
			uuid,
			name)
	{}
	virtual ~ControlsContainer();
	ControlsContainer(const ControlsContainer &other) = delete;
	ControlsContainer(ControlsContainer &&other) = delete;
	ControlsContainer& operator=(const ControlsContainer &other) = delete;
	ControlsContainer& operator=(ControlsContainer &&other) = delete;
	
	void addControl(ControlBasePtr const &controlPtr);
	void appendControlToTabGroup (ControlBaseWeakPtr const &controlWeakPtr);
	void insertControlInTabGroupBefore (ControlsWeakListT::iterator ref,
		ControlBaseWeakPtr const &controlWeakPtr);
	
	void drawChildren (OevGLES::RenderStandardUniforms const &stdUniformData);
	
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
		OevGLES::RenderStandardUniforms &parentUniforms) override;

protected:

ControlsMapT controlsMap;

ControlsWeakListT tabGroup;

};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_CONTROLSCONTAINER_H_ */
