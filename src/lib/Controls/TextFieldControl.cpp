/*
 * TextFieldControl.cpp
 *
 *  Created on: Dec 31, 2025
 *      Author: hor
 *
 *   This file is part of OpenVarioFront, an electronic variometer display for glider planes
 *   Copyright (C) 2026  Kai Horstmann
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

#include "lib/Controls/TextFieldControl.h"

#if defined HAVE_LOG4CXX_H
	static log4cxx::LoggerPtr logger;
#endif

namespace OevControls {

TextFieldControl::TextFieldControl(	ControlsContainerWeakPtr const &parent,
			RenderContextSharedPtr const& renderContextPtr,
			OevUtil::Uuid const & uuid,
			char const* name)
	: PlainFieldControl(parent,renderContextPtr,uuid,name),
	// the base class ControlBase asserts that renderContextPtr is not empty.
	textRenderer(renderContextPtr),
	textSizePointsPtr{&this->renderContextPtr->textSizePoints},
	fontListPtr{&this->renderContextPtr->fontNameList},
	textRenderUniforms{renderUniforms}
{
	#if defined HAVE_LOG4CXX_H
			// Get the logger if necessary
			if (!logger) {
				logger = log4cxx::Logger::getLogger("OpenVarioFront.Controls.TextFieldControl");
			}
	#endif

	// I am drawing the background myself by using the base class PlainFieldControl::draw() call.
	textRenderer.setDrawBackground(false);
	textRenderer.setTextColor(renderContextPtr->textForegroundColorPtr);
	// set the background color to the textbackground color.
	fillColorPtr = renderContextPtr->textBackgroundColorPtr;
	textSizePointsPtr = &renderContextPtr->textSizePoints;
	
	// Detach the model matrix from the control's model matrix.
	// The control model matrix scale is the actual size of the control.
	// For a text renderer the scale needs to be 1.0.
	// So I set the scale factor to 1.0 here.
	textRenderUniforms.resetModelMatrixPtr();
	auto &modelmatrix = textRenderUniforms.getModelMatrix();
	for (int i = 0;i < 4; ++i){
		modelmatrix(i,i) = 1.0f;
	}
}

TextFieldControl::~TextFieldControl() {}

void TextFieldControl::setupVertexBuffers () {
	PlainFieldControl::setupVertexBuffers();
	
	if (textFieldAttribsChanged) {
		textRenderer.setFontSize(*textSizePointsPtr);
		textRenderer.setFonts(*fontListPtr);
		textRenderer.setTextColor(textColorPtr);
		pango_layout_set_line_spacing (textRenderer.getPangoLayout(),1.0f);

		if (widthIsFixed_) {
			textRenderer.setWidth(getWidth());
		} else {
			textRenderer.setWidthSubpixel(-1);
		}

		if (heightIsFixed_) {
			textRenderer.setHeight(getHeight());
			pango_layout_set_ellipsize (textRenderer.getPangoLayout(),PANGO_ELLIPSIZE_END);
		} else {
			textRenderer.setHeightSubpixel(-1);
		}
		
	}

	if (textChanged) {
		textRenderer.setText(text);
	}

	if (textChanged || textFieldAttribsChanged) {
		// Leave a bit space to the left of the background box.
		int leftOffset, topOffset;
		if (hasFrame_) {
			leftOffset = 4;
			topOffset = 2;
		} else {
			leftOffset = 2;
			topOffset = 0;
		}
		textRenderer.renderLayout(leftOffset,topOffset);
		textRenderer.setupVertexBuffers();
		textChanged = false;
		textFieldAttribsChanged = false;
		
		auto boxSize = textRenderer.getTextBoxSize();
		int rightOffset,bottomOffset;
		if (hasFrame_) {
			rightOffset = 8;
			bottomOffset = 6;
		} else {
			rightOffset = 4;
			bottomOffset = 2;
		}

		// Set the height and/or width to the manually set sizes when the text box is smaller.
		// When the text box is larger leave that size in order to provide a background for the entire text.
		// Particularly when you set the height small, Pango will always render at least one line. The height of
		// that rendered text will surely be higher than the previously set height.		
		if (widthIsFixed_ &&
			(boxSize.width + rightOffset < getWidth())) {
				boxSize.width = getWidth() - rightOffset;
		}
		if (heightIsFixed_ &&
			(boxSize.height + bottomOffset < getHeight())) {
				boxSize.height = getHeight() - rightOffset;
		}
		
		setSize({static_cast<int>(boxSize.width + 0.5f)+rightOffset,
				 static_cast<int>(boxSize.height + 0.5f)+bottomOffset});
		LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
			<< ": Size of text box = " << boxSize.width << 'x' << boxSize.height
			<< " at " << boxSize.right << ',' << boxSize.top
			<< ", text spacing = " << pango_layout_get_spacing(textRenderer.getPangoLayout()) / 1024.0
			<< ", line spacing factor = " << pango_layout_get_line_spacing(textRenderer.getPangoLayout())
			);
	}
}

void TextFieldControl::draw() {
	if (textChanged || textFieldAttribsChanged) {
		setupVertexBuffers();
	}
	PlainFieldControl::draw();
	
	textRenderer.draw(textRenderUniforms);
	//textRenderer.draw(renderUniforms);
	
}

void TextFieldControl::onPositionChanged() {
	
	// Take over the position into the model matrix of the text render uniforms.
	auto const &controlModelMatrix = renderUniforms.getModelMatrix();
	auto &textModelMatrix = textRenderUniforms.getModelMatrix();
	for (int i = 0; i < 2; ++i) {
		textModelMatrix(i,3) = controlModelMatrix(i,3);
	}
	// The text position is the top right, not the bottom right.
	// Therefore add the control height.
	textModelMatrix(1,3) += controlModelMatrix(1,1);
	
	LOG4CXX_DEBUG(logger, __PRETTY_FUNCTION__
		<< ": controlModelMatrix = \n " << controlModelMatrix
		<< ", textModelMatrix = \n" << textModelMatrix);
}

} /* namespace OevControls */
