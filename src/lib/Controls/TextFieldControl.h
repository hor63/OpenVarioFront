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
		textRenderer.setText(str);
		textFieldAttribsDirty = true;
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
			textRenderer.setFontSize(sizePoints);
		} else {
			textRenderer.setFontSize(renderContextPtr->textSizePoints);
		}
		
		textFieldAttribsDirty = true;
	}
	double getFontSize() {
		return textRenderer.getFontSize();
	}

	/** \brief Set the text foreground color, overwriting the default text color.
	 * 
	 * Default is \p textForegroundColorPtr in \ref renderContextPtr.
	 * 
	 * You can reset the text color back to the default from \ref renderContextPtr
	 * by passing \p nullptr.
	 * 
	 * \see OevGLES::RenderContext::textForegroundColorPtr
	 */
	void setTextColor(OevGLES::Vec4 const *textColor) {
		if (textColor != nullptr) {
			textColorPtr = std::make_shared<OevGLES::Vec4>(*textColor);
			textFieldAttribsDirty = true;
		} else {
			textColorPtr = renderContextPtr->textForegroundColorPtr;
		}

		textFieldAttribsDirty = true;
	}
	const OevGLES::Vec4& getTextColor() const {
		return *textColorPtr.get();
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
	void setFonts(std::string fontNames) {
		textRenderer.setFonts(fontNames);
		textFieldAttribsDirty = true;
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

protected:

	OevGLES::GLTextRenderer textRenderer;

	/** 
	 * The text foreground color points by default to renderContextPtr->textForegroundColorPtr.
	 * The pointer can be overwritten, e.g. with the \ref OevGLES::RenderContext::buttonForegroundColorPtr
	 * or any other color shared pointer.
	 *
	 * \see OevGLES::RenderContext::textForegroundColorPtr
	 */
	OevGLES::Vec4ShPtr textColorPtr;

	
private:

	/** \brief Set \p true when any text attribute or the text itself is changed.
	 *
	 * When it is true \ref setupVertexBuffers () is being called to layout the text again when
	 * \ref draw () is being called.
	 */
	bool textFieldAttribsDirty = true;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_TEXTFIELDCONTROL_H_ */
