/*
 * TextFieldControl.h
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

#ifndef LIB_CONTROLS_TEXTFIELDCONTROL_H_
#define LIB_CONTROLS_TEXTFIELDCONTROL_H_

#include "Renderers/RendererBase.h"
#include "lib/Controls/PlainFieldControl.h"
#include "GLTextRender/GLTextRenderer.h"

namespace OevControls {

class TextFieldControl: public PlainFieldControl {
public:
	TextFieldControl(ControlsContainerWeakPtr const &parent,
		RenderContextSharedPtr const& renderContextPtr,
		OevUtil::Uuid const & uuid,
		char const* name);
	virtual ~TextFieldControl();
	TextFieldControl(const TextFieldControl &other) = delete;
	TextFieldControl(TextFieldControl &&other) = delete;
	TextFieldControl& operator=(const TextFieldControl &other) = delete;
	TextFieldControl& operator=(TextFieldControl &&other) = delete;
	

	void setText (const std::string& str) {
		text = str;
		textChanged = true;
	}
	const std::string& getText() const {
		return textRenderer.getText();
	}

	/** \brief Set the font size
	*
	* The default is being set from \p textSizePoints in \ref renderContextPtr.
	*
	* You can reset the text size to the default from \ref renderContextPtr
	* by passing a text size <= 0.0.
	*
	* \param sizePoints Font size in points (what else 🙃)
	* \see OevGLES::RenderContext::textSizePoints
	 *
	 */
	void setFontSize (double sizePoints){
		
		if (sizePoints > 0.0) {
			locTextSizePoints = sizePoints;
			textSizePointsPtr = &locTextSizePoints;
			textRenderer.setFontSize(sizePoints);
		} else {
			textSizePointsPtr = &renderContextPtr->textSizePoints;
		}
		
		textFieldAttribsChanged = true;
	}
	double getFontSize() {
		return textRenderer.getFontSize();
	}

	/** \brief Set the text foreground color, overwriting the default text color.
	 * 
	 * Default is \p textForegroundColorPtr in \ref renderContextPtr.
	 * 
	 * You can reset the text color back to the default from \ref renderContextPtr
	 * by passing an empty shared pointer.
	 * 
	 * \see OevGLES::RenderContext::textForegroundColorPtr
	 */
	void setTextColor(OevGLES::Vec4ShPtr const &textColorPtr) {
		if (textColorPtr) {
			this->textColorPtr = textColorPtr;
		} else {
			// revert to the context color.
			this->textColorPtr = renderContextPtr->textForegroundColorPtr;
		}

		textFieldAttribsChanged = true;
	}
	const OevGLES::Vec4& getTextColor() const {
		return *textColorPtr.get();
	}

	/// \see widthIsFixed_
	bool widthIsFixed() {return widthIsFixed_;}
	/// \see widthIsFixed_
	void setWidthIsFixed(bool fixedWidth = true) {
		widthIsFixed_ = fixedWidth;
		textFieldAttribsChanged = true;
	}
	
	/// \see heightIsFixed_
	bool heightIsFixed() {return heightIsFixed_;}
	void setHeightIsFixed(bool fixedHeight = true) {
		heightIsFixed_ = fixedHeight;
		textFieldAttribsChanged = true;
	}


	/** \brief Set the font name or list of font names to choose from
	 *
	 * The default is being set from the \p fontNameList in \ref renderContextPtr.
	 * You can reset the font name list to the default by passing an empty string.
	 *
	 * \param fontNames Name of the font family, or comma separated list of font families to choose from
	 * \see [Pango.FontDescription.set_family](https://docs.gtk.org/Pango/method.FontDescription.set_family.html)
	 * \see OevGLES::RenderContext::fontNameList
	 */
	void setFontNames(std::string fontNames) {
		textRenderer.setFonts(fontNames);
		textFieldAttribsChanged = true;
	}
	const std::string& getFonts() {
		return textRenderer.getFonts();
	}

	/** \brief Layout the text with the set attributes and text.
	 *
	 * Is called from \ref draw () when \ref textFieldAttribsDirty is true.\n
	 * Resets \ref textFieldAttribsDirty to \p false.
	 *
	 * \see RendererBase::setupVertexBuffers()
	 */
	virtual void setupVertexBuffers () override;

	/// \see RendererBase::draw()
	virtual void draw() override;

	/** \brief Request to re-calculate the own model matrix when the own position
	 * changed.
	 */
	virtual void onPositionChanged() override;

protected:

	OevGLES::GLTextRenderer textRenderer;

	
private:

	/** \brief Set \p true when any text attribute is changed.
	 *
	 * When it is true \ref setupVertexBuffers () is being called to layout the text again by
	 * \ref draw ().
	 *
	 * It is not changed when the text is being changed. That uses \ref textChanged
	 *
	 */
	bool textFieldAttribsChanged = true;
	
	bool textChanged = true;

	/** \brief The text control's width is set externally and fixed
	 *
	 * If \p true the width of the control is being set by \ref setSize() becomes fixed.
	 * The text will be squeezed within this width, creating line breaks when necessary.
	 *
	 * If \p false the size setting is being ignored. Instead the width is set by the size required to
	 * render the text as being set. When calling \ref setupVertexBuffers() the control is automatically re-sized.
	 */
	bool widthIsFixed_ = false;
	
	/** \brief The text control's height is set externally and fixed
	 *
	 * If \p true the height of the control is being set by \ref setSize() becomes fixed.
	 * When the text does not fit within the height the text is truncated and ellipsized.
	 *
	 * If \p false the size setting is being ignored. Instead the height is set by the size required to
	 * render the text. When calling \ref setupVertexBuffers() the control is automatically re-sized.
	 * If \ref widthIsFixed_ is true the text will extend further downward.
	 */
	bool heightIsFixed_ = false;

	/** 
	 * The text foreground color points by default to renderContextPtr->textForegroundColorPtr.
	 * The pointer can be overwritten by \ref setTextColor ().
	 *
	 * \see OevGLES::RenderContext::textForegroundColorPtr
	 */
	OevGLES::Vec4ShPtr textColorPtr;

	double locTextSizePoints = 0.0;
	double *textSizePointsPtr;
	
	std::string locFontList;
	std::string *fontListPtr;
	
	std::string text;
	
	OevGLES::RenderStandardUniforms textRenderUniforms;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_TEXTFIELDCONTROL_H_ */
