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
	auto parentPtr = parent.lock();
	
	if (parentPtr) {
		throw ControlsException (
			"Error in ControlBase::ControlBase: parent must be a valid pointer"
			"to an existing ControlsContainer");
	}
	
	// The distincive property of the root control is that its parent points to itself.
	if (reinterpret_cast<void const*>(parentPtr.get())
		== reinterpret_cast<void const*>(this)) {
		rootControl = true;
	}
}

ControlBase::~ControlBase() {
}

void ControlBase::setName(std::string const & name) {
	this->name = name;
}

void ControlBase::setPosition (Pos position) {
	this->position = position;
}

void ControlBase::setSize (Size size) {
	if (size.height <= 0 || size.width <= 0) {
		auto errTxt = fmt::format (
			fmt::runtime(_(
				"Control {0}:{1}: Error in ControlBase::setSize(): width and size must be > 0. "
				"newSize is {2}x{3}")),
				uuid.getUuidString(),name,size.width,size.height);
		throw ControlsException(_(
			errTxt.c_str()));
	}
	this->size = size;
	
	topRight.x = position.x + size.width;
	topRight.y = position.y + size.height;
}

void ControlBase::setTopRight (Pos topRight) {
	if (topRight.x <= position.x ||
		topRight.y <= position.y) {
		auto errTxt = fmt::format (
			fmt::runtime(_(
				"Control {0}:{1}: Error in ControlBase::setTopRight(): . "
				"newTopRight is {2}x{3}")),
				uuid.getUuidString(),name,topRight.x,topRight.y);
		throw ControlsException(_(
			errTxt.c_str()));
	}
	
	this->topRight = topRight;
	
	size.width  = topRight.x - position.x ;
	size.height = topRight.y - position.y;
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
	this->defaultControl = isDefaultControl;
}

void ControlBase::setTabSequence(int tabSequence) {
	this->tabSequence = tabSequence;
}

void ControlBase::setTabPredecessor (ControlBaseWeakPtr tabPredecessor) {
	this->tabPredecessor = tabPredecessor;
}

void ControlBase::setTabSuccessor (ControlBaseWeakPtr tabSuccessor) {
	this->tabSuccessor = tabSuccessor;
}

void ControlBase::setTabContainer (ControlsContainerWeakPtr tabContainer) {
	this->tabContainer = tabContainer;
}

} /* namespace OevControls */
