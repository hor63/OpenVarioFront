/*
 * ControlBase.cpp
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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include "OVFCommon.h"

#include "ExceptionBase.h"
#include "fmt/base.h"
#include "fmt/format.h"

#include "ControlBase.h"
#include "ControlsContainer.h"
#include "Renderers/RenderContext.h"

namespace OevControls {

ControlBase::ControlBase(ControlsContainerWeakPtr const & parent,
		RenderContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name
	) :
		parent {parent},
		renderContextPtr{renderContextPtr},
		uuid {uuid},
		name {name},
		renderUniforms ( // Use a lambda here 
			[&,this,parent]() -> OevGLES::RenderStandardUniforms {
				auto parentPtr = this->parent.lock();
	
				if (!parentPtr) {
					throw ControlsFatalException (
						"Error in ControlBase::ControlBase: parent must be a valid pointer"
						"to an existing ControlsContainer");
				}
				// The distinctive property of the root control is that its parent points to itself.
				if (reinterpret_cast<void const*>(parentPtr.get())
					== reinterpret_cast<void const*>(this)) {
					// The root control has some quirks, as all other controls inherit renderUniforms from their parent,
					// but the "parent" of the root control is the root control itself. Thus the root control needs to
					// pull itself out of the morass by itself. This means a new RenderStandardUniforms object is
					// created from scratch.
					return OevGLES::RenderStandardUniforms();
				} else {
					return parentPtr->getRenderUniforms();
				}
	
			} ()//here call the lambda to initialize the member renderUniforms!
		),
		renderFrameUniforms {renderUniforms}
{
	auto parentPtr = this->parent.lock();
	
	if (!parentPtr) {
		throw ControlsFatalException (
			"Error in ControlBase::ControlBase: parent must be a valid pointer"
			"to an existing ControlsContainer");
	}

	// The distincive property of the root control is that its parent points to itself.
	if (reinterpret_cast<void const*>(parentPtr.get())
		== reinterpret_cast<void const*>(this)) {
	
		// Create parentModelMatrixPtr as unity matrix
		// renderUniforms were created fresh by the lambda in the initializer list.
		parentModelMatrixPtr = std::make_shared<OevGLES::RenderStandardUniforms::Mat4WithChangeCounter>();

		isRootControl_ = true;
	} else {
		// Store the parent's model matrix for later.
		parentModelMatrixPtr = renderUniforms.getModelMatrixPtr();
		// You always have your own model matrix, alone to set your position.
		renderUniforms.resetModelMatrixPtr();
		renderUniforms.getModelMatrix() = OevGLES::Mat4::Identity();
		renderFrameUniforms.setModelMatrixPtr(
		renderUniforms.getModelMatrixPtr());

	} // not root control
	
	renderFrameUniforms.resetModelMatrixPtr();
	renderFrameUniforms.getModelMatrix() = renderUniforms.getModelMatrixC();

}

ControlBase::~ControlBase() {
}

void ControlBase::setName(std::string const & name) {
	this->name = name;
}

void ControlBase::setPosition (PosPixel const& position) {
	if (this->position.xPixel != position.xPixel ||
		this->position.yPixel != position.yPixel) {
		this->position = position;
		
		locControlPosition(0) = position.xPixel;
		locControlPosition(1) = position.yPixel;

		posOrSizeDirty = true;

		onPositionChanged();
	}
}

void ControlBase::setSize (SizePixel const& size) {
	if (size.heightPixel <= 0 || size.widthPixel <= 0) {
		auto errTxt = fmt::format (
			fmt::runtime(_(
				"Control {0}:{1}: Error in ControlBase::setSize(): width and size must be > 0. "
				"newSize is {2}x{3}")),
				uuid.getUuidString(),name,size.widthPixel,size.heightPixel);
		throw ControlsFatalException(_(
			errTxt.c_str()));
	}
	
	if (this->size.heightPixel != size.heightPixel ||
		this->size.widthPixel != size.widthPixel) {
		this->size = size;
		
		topRight.xPixel = position.xPixel + size.widthPixel;
		topRight.yPixel = position.yPixel + size.heightPixel;

		auto & modelMatrix = renderUniforms.getModelMatrix();
		modelMatrix(0,0) = size.widthPixel;
		modelMatrix(1,1) = size.heightPixel;
		
		auto & frameModelMatrix = renderFrameUniforms.getModelMatrix();
		if (size.widthPixel >= 4 && size.heightPixel >= 4) {
			frameModelMatrix(0,0) = size.widthPixel - 4;
			frameModelMatrix(1,1) = size.heightPixel - 4;
			doDrawFrame = hasFrame_;
		} else {
			// too small to fit a frame inside
			frameModelMatrix(0,0) = size.widthPixel;
			frameModelMatrix(1,1) = size.heightPixel;
			doDrawFrame = false;
		}

		posOrSizeDirty = true;

		onSizeChanged();
	}
}

void ControlBase::setTopRight (PosPixel const& topRight) {
	if (topRight.xPixel <= position.xPixel ||
		topRight.yPixel <= position.yPixel) {
		auto errTxt = fmt::format (
			fmt::runtime(_(
				"Control {0}:{1}: Error in ControlBase::setTopRight(): . "
				"newTopRight is {2}x{3}, which is left or below position")),
				uuid.getUuidString(),name,topRight.xPixel,topRight.yPixel);
		throw ControlsFatalException(_(
			errTxt.c_str()));
	}
	
	if (this->topRight.xPixel != topRight.xPixel ||
		this->topRight.yPixel != topRight.yPixel) {
		this->topRight = topRight;
		
		size.widthPixel  = topRight.xPixel - position.xPixel ;
		size.heightPixel = topRight.yPixel - position.yPixel;
		
		auto & modelMatrix = renderUniforms.getModelMatrix();
		modelMatrix(0,0) = size.widthPixel;
		modelMatrix(1,1) = size.heightPixel;

		auto & frameModelMatrix = renderFrameUniforms.getModelMatrix();
		if (size.widthPixel >= 4 && size.heightPixel >= 4) {
			frameModelMatrix(0,0) = size.widthPixel - 4;
			frameModelMatrix(1,1) = size.heightPixel - 4;
			doDrawFrame = hasFrame_;
		} else {
			// too small to fit a frame inside
			frameModelMatrix(0,0) = size.widthPixel;
			frameModelMatrix(1,1) = size.heightPixel;
			doDrawFrame = false;
		}

		posOrSizeDirty = true;

		onSizeChanged();
	}
}

void ControlBase::setPosOrSizeDirty(bool posOrSizeDirty) {
	this->posOrSizeDirty = posOrSizeDirty;
}

void ControlBase::setDirty(bool dirty) {
	this->dirty = dirty;
}

void ControlBase::setHasFocus(bool hasFocus) {
	this->hasFocus_ = hasFocus;
}

void ControlBase::setVisible (bool visible) {
	this->visible = visible;
}

void ControlBase::setActive (bool active) {
	this->active = active;

}

void ControlBase::setCanTakeFocus (bool takesFocus) {
	this->takesFocus = takesFocus;
}

void ControlBase::setDefaultControl (bool isDefaultControl) {
	this->isDefaultControl_ = isDefaultControl;
}

void ControlBase::setTabSequence(int tabSequence) {

	this->tabSequence = tabSequence;
}

void ControlBase::setTabPredecessor (ControlBaseWeakPtr const& tabPredecessor) {
	this->tabPredecessor = tabPredecessor;
}

void ControlBase::setTabSuccessor (ControlBaseWeakPtr const& tabSuccessor) {
	this->tabSuccessor = tabSuccessor;
}

void ControlBase::setTabContainer (ControlsContainerWeakPtr const& tabContainer) {
	this->tabContainer = tabContainer;
}

void ControlBase::onSizeChanged() {
}

void ControlBase::onPositionChanged() {
}

void ControlBase::onParentPositionChanged() {
}

void ControlBase::onResetParentRenderUniforms(
	OevGLES::RenderStandardUniforms const &parentUniforms) {

	auto saveModelMatrixPtr = renderUniforms.getModelMatrixPtr();
	
	parentModelMatrixPtr = parentUniforms.getModelMatrixPtr();
	renderUniforms = parentUniforms;

	renderUniforms.setModelMatrixPtr(saveModelMatrixPtr);

	posOrSizeDirty = true;

	// Probably parent position changed too.
	onParentPositionChanged();
}

void ControlBase::recalcSizePositionMatrix() {
	auto &modelMatrix = renderUniforms.getModelMatrix();
	auto &parentModelMatrix = parentModelMatrixPtr->matrix4;
	
	// By default I am only inheriting the parent position on top of my own position.
	// Sizes are absolute. These are dialog controls, not a universal Model hierarchy.
	// The local scale factor serves to scale a 1x1 quad in the RenderCodext up to the desired size
	// of my control.
	for (int i = 0; i<3; ++i){
		modelMatrix(i,3) = parentModelMatrix(i,3) + locControlPosition(i);
	}
	

	auto & frameModelMatrix = renderFrameUniforms.getModelMatrix();
	if (size.widthPixel >= 6 && size.heightPixel >= 6) {
		frameModelMatrix(0,3) = modelMatrix(0,3) + 2;
		frameModelMatrix(1,3) = modelMatrix(1,3) + 2;
		doDrawFrame = hasFrame_;
	} else {
		// too small to fit a frame inside
		frameModelMatrix(0,3) = modelMatrix(0,3);
		frameModelMatrix(1,3) = modelMatrix(1,3);
		doDrawFrame = false;
	}

	posOrSizeDirty = false;
}

void ControlBase::setHasFrame(bool hasFrame) {
	hasFrame_ = hasFrame;
	
	doDrawFrame = hasFrame_ && (size.widthPixel >= 4 && size.heightPixel >= 4);
}

} /* namespace OevControls */
