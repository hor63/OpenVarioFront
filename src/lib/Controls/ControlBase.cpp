/*
 * ControlBase.cpp
 *
 *  Created on: Jul 2, 2025
 *      Author: hor
 */
#include <utility>
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "OVFCommon.h"

#include "ExceptionBase.h"
#include "fmt/base.h"
#include "fmt/format.h"

#include "lib/Controls/ControlBase.h"

namespace OevControls {

ControlBase::ControlBase(ControlsContainerWeakPtr const & parent,
		OevUtil::Uuid const & uuid,
		char const* name)
		:parent {parent},
		uuid {uuid},
		name {name}
{
	auto parentPtr = this->parent.lock();
	
	if (!parentPtr) {
		throw ControlsException (
			"Error in ControlBase::ControlBase: parent must be a valid pointer"
			"to an existing ControlsContainer");
	}
	
	// The distincive property of the root control is that its parent points to itself.
	if (reinterpret_cast<void const*>(parentPtr.get())
		== reinterpret_cast<void const*>(this)) {
		isRootControl_ = true;
	}
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
		throw ControlsException(_(
			errTxt.c_str()));
	}
	
	if (this->size.heightPixel != size.heightPixel ||
		this->size.widthPixel != size.widthPixel) {
		this->size = size;
		
		topRight.xPixel = position.xPixel + size.widthPixel;
		topRight.yPixel = position.yPixel + size.heightPixel;
		
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
		throw ControlsException(_(
			errTxt.c_str()));
	}
	
	if (this->topRight.xPixel != topRight.xPixel ||
		this->topRight.yPixel != topRight.yPixel) {
		this->topRight = topRight;
		
		size.widthPixel  = topRight.xPixel - position.xPixel ;
		size.heightPixel = topRight.yPixel - position.yPixel;
		
		onSizeChanged();
	}
}

void ControlBase::setForegroundColor (OevGLES::Vec4 const& foregroundColor) {
	this->foregroundColor = foregroundColor;
}

void ControlBase::setBackgroundColor (OevGLES::Vec4 const& backgroundColor) {
	this->backgroundColor = backgroundColor;
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
	// Nothing to the here for me.
}

void ControlBase::onPositionChanged() {
	locTranslationMatrix(0,3) = position.xPixel;
	locTranslationMatrix(1,3) = position.yPixel;
	
	renderUniforms.getModelMatrix() =
		parentModelMatrixPtr->matrix4 * locTranslationMatrix;
	
}

void ControlBase::onParentPositionChanged() {

	renderUniforms.getModelMatrix() =
		parentModelMatrixPtr->matrix4 * locTranslationMatrix;
}

void ControlBase::onResetParentRenderUniforms(
	OevGLES::RenderStandardUniforms &parentUniforms) {

	auto saveModelMatrixPtr = renderUniforms.getModelMatrixPtr();
	
	parentModelMatrixPtr = parentUniforms.getModelMatrixPtr();
	renderUniforms = parentUniforms;

	renderUniforms.setModelMatrixPtr(saveModelMatrixPtr);

	renderUniforms.getModelMatrix() =
		parentModelMatrixPtr->matrix4 * locTranslationMatrix;

}


} /* namespace OevControls */
