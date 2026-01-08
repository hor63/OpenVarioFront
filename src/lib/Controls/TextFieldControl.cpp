/*
 * TextFieldControl.cpp
 *
 *  Created on: Dec 31, 2025
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
 #include "GLTextRender/GLTextRenderer.h"
#include "PlainFieldControl.h"
#ifdef HAVE_CONFIG_H
 #  include <config.h>
 #endif

#include "OVFCommon.h"

#include "lib/Controls/TextFieldControl.h"

namespace OevControls {

TextFieldControl::TextFieldControl(	ControlsContainerWeakPtr const &parent,
			RenderContextSharedPtr const& renderContextPtr,
			OevUtil::Uuid const & uuid,
			char const* name)
	: PlainFieldControl(parent,renderContextPtr,uuid,name),
	// the base class ControlBase asserts that renderContextPtr is not empty.
	textRenderer(renderContextPtr),
	textSizePointsPtr{&this->renderContextPtr->textSizePoints},
	fontListPtr{&this->renderContextPtr->fontNameList}
{
	// I am drawing the background myself by using the base class PlainFieldControl::draw() call.
	textRenderer.setDrawBackground(false);
	textRenderer.setTextColor(*renderContextPtr->textForegroundColorPtr);
	// set the background color to the textbackground color.
	fillColorPtr = renderContextPtr->textBackgroundColorPtr;
	textSizePointsPtr = &renderContextPtr->textSizePoints;
}

TextFieldControl::~TextFieldControl() {}

void TextFieldControl::setupVertexBuffers () {
	PlainFieldControl::setupVertexBuffers();
	
	if (textFieldAttribsChanged) {
		textRenderer.setFontSize(*textSizePointsPtr);
		textRenderer.setFonts(*fontListPtr);
		textRenderer.setTextColor(*textColorPtr.get());
		textFieldAttribsChanged = false;
	}

	if (textChanged) {
		textRenderer.setText(text);
	}

	if (textChanged || textFieldAttribsChanged) {
		textRenderer.renderLayout();
		textRenderer.setupVertexBuffers();
		textChanged = false;
		textFieldAttribsChanged = false;
	}

}


void TextFieldControl::draw() {
	if (textChanged || textFieldAttribsChanged) {
		setupVertexBuffers();
	}
	PlainFieldControl::draw();
	
	textRenderer.draw(renderUniforms);
}


} /* namespace OevControls */
