 /*
 * GLTexture.h
 *
 *  Created on: Jun 2, 2018
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

#ifndef GLES_GLTEXTURE_H_
#define GLES_GLTEXTURE_H_

#include "GLES/TexHelper/TextureData.h"

namespace OevGLES {

class GLTexture {
public:
	enum TextureFilter {
		Nearest					= GL_NEAREST,
		Linear					= GL_LINEAR,
		NearestMipMapNearest	= GL_NEAREST_MIPMAP_NEAREST,
		NearestMipMapLinear		= GL_NEAREST_MIPMAP_LINEAR,
		LinearMipMapNearest		= GL_LINEAR_MIPMAP_NEAREST,
		LinearMipMapLinear		= GL_LINEAR_MIPMAP_LINEAR
	};

	enum TextureWrapMode {
		ClampToEdge		= GL_CLAMP_TO_EDGE,
		Repeat			= GL_REPEAT,
		MirroredRepeat	= GL_MIRRORED_REPEAT
	};

	GLTexture();
	virtual ~GLTexture();

	/** \brief set the texture data format and type from the texturedata object
	 *
	 * You can use the method to only load the highest level of a texture or subsequently the entire mipmap chain.
	 * Loading the complete mipmap chain correctly is your responsibility.
	 * Else
	 *
	 * @param textureData Texture data, format and type in a neat package. The object is only used during this call, and can be discarded afterwards.
	 * @param mipMapLevel Mip level used by glTexImage2D(). Upper, most detailed level is 0 which is the minimum, and always required.
	 */
	void setTextureData (TextureData const &textureData,GLint mipMapLevel = 0);

	/** \brief Let GL generate the mipmap chain for the texture.
	 *
	 */
	void generateMipmap();

	/** \brief Set filter for minification of the texture, i.e. texture texels are smaller than screen pixels
	 *
	 * @param filterType Any of the \ref TextureFilter enums.
	 */
	void setMinificationFilter (TextureFilter filterType);

	/** \brief Return texture minification filter
	 *
	 *  \see setMinificationFilter
	 *
	 * @return Texture minification filter
	 */
	TextureFilter getMinificationFilter() {
		return minFilterType;
	}


	/** \brief Set filter for magnification of the texture, i.e. texture texels are larger than screen pixels
	 *
	 * @param filterType Only Nearest or Linear of the \ref TextureFilter enums are allowed here. Otherwise exception.
	 * @throws \ref TextureException
	 */
	void setMagnificationFilter (TextureFilter filterType);

	/** \brief Return texture magnification filter
	 *
	 *  \see setMagnificationFilter
	 *
	 * @return Texture magnification filter
	 */
	TextureFilter getMagnificationFilter() {
		return magFilterType;
	}

	/** \brief Set the wrapping mode for the texture
	 *
	 * Default wrap mode is ClampToEdge.
	 *
	 * @param wrapS Wrap mode in the S-dimension
	 * @param wrapT Wrap mode in the T-dimension
	 */
	void setWrapMode (TextureWrapMode wrapModeS, TextureWrapMode wrapModeT);

	/** \brief Convenience operator for gl-functions which use the texture ID as parameter
	 *
	 * @return GL texture ID
	 */
	operator GLuint () {
		return textureHandle;
	}

	/** \brief Return the GL texture ID.
	 *
	 * @return GL texture ID
	 */
	GLuint getTextureHandle () {
		return textureHandle;
	}

	/** \brief Use this method in every rendering cycle.
	 *
	 * This method sets the active texture unit to textureUnit, binds the texture to the texture unit,
	 * assigns it to the uniform location which was bound by the GL program.
	 * The function also sets the filter and wrapping mode.
	 *
	 * @param textureUnit One of the constants GL_TEXTURE0, GL_TEXTURE1...
	 * @param textureUnitNo The numeric number of the texture unit. Must align with textureUnit. Pass 0 for GL_TEXTURE0, 1 for GL_TEXTURE1...
	 * @param uniformLocation Location of the sampler uniform in the GL program
	 */
	 void bindToUniformLocation (GLenum textureUnit,GLint textureUnitNo, GLint uniformLocation);



private:
	GLuint textureHandle = 0;
	TextureFilter minFilterType = Nearest;
	TextureFilter magFilterType = Nearest;

	TextureWrapMode wrapS = ClampToEdge;
	TextureWrapMode wrapT = ClampToEdge;


	/// \brief Obtain a texture handle from GLES when \ref textureHandle is not yet set
	void createTextureHandle();
};

} /* namespace OevGLES */

#endif /* GLES_GLTEXTURE_H_ */
