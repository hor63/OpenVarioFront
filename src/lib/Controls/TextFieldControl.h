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
	
	// Pass through a good deal of the the GLTextRenderer public interface

	void setText (const std::string& str) {
		textRenderer.setText(str);
	}

	const std::string& getText() const {
		return textRenderer.getText();
	}

	/** \brief Set the font size
	 *
	 * \param sizePoints Font size in points (what else 🙃)
	 */
	void setFontSize (double sizePoints){
		textRenderer.setFontSize(sizePoints);
	}

	/** \brief Set the font name or list of font names to choose from
	 *
	 * \param fontNames Name of the font family, or comma separated list of font families to choose from
	 * \see [Pango.FontDescription.set_family](https://docs.gtk.org/Pango/method.FontDescription.set_family.html)
	 */
	void setFonts(std::string fontNames) {
		textRenderer.setFonts(fontNames);
	}

	double getFontSize() {
		return textRenderer.getFontSize();
	}
	const std::string& getFonts() {
		return textRenderer.getFonts();
	}

	/// \see RendererBase::setupVertexBuffers()
	virtual void setupVertexBuffers () override;

	/// \see RendererBase::draw()
	virtual void draw() override;

	bool isDrawBackground() const {
		return textRenderer.isDrawBackground();
	}

	const OevGLES::Vec4& getTextColor() const {
		return textRenderer.getTextColor();
	}

	/** \brief Default is the controls foreground color. 
	 */
	void setTextColor(OevGLES::Vec4 const &textColor) {
		textRenderer.setTextColor(textColor);
	}


protected:

	OevGLES::GLTextRenderer textRenderer;
};

} /* namespace OevControls */

#endif /* LIB_CONTROLS_TEXTFIELDCONTROL_H_ */
