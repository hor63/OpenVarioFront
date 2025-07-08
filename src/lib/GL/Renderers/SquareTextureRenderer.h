/*
 * SquareTextureRenderer.h
 *
 *  Created on: Jun 10, 2018
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

#ifndef SQUARETEXTURERENDERER_H_
#define SQUARETEXTURERENDERER_H_


#include "GLPrograms/GLProgDiffLightTexture.h"
#include "Renderers/RendererBase.h"
#include "GLES/GLTexture.h"

namespace OevGLES {

class SquareTextureRenderer : public RendererBase {
public:

	typedef GLfloat VertexPosition [4];
	typedef GLfloat TextureCoordinate [2];
	
	struct VertexType {
		VertexPosition position;
		TextureCoordinate textureCoordinate;
	};

	SquareTextureRenderer();

	virtual ~SquareTextureRenderer();

	/** \brief Switch to external PNG file, and set the path and name of the PNG file
	 * 
	 * Using this method switches the source of PNG data to an external file.
	 * If the source had been set to in-memory data using \ref setPNGMemoryData
	 * this info is reset. \ref memLocation is reset to \p nullptr.
	 * 
	 * \param pngFileName Name of the PNG file. The name can include a relative
	 		or absolute path.
	 */
	void setPNGFileName (std::string const &pngFileName);
	
	/** \brief Return the name of the external PNG file or an empty string.
	 *
	 * When the source is set to in-memory data with \ref setPNGMemoryData
	 * the method returns an empty string.
	 *
	 * \return the path and name of the PNG file or an empty string
	 */
	std::string getPNGFileName (){
		
		if (memLocation == nullptr) {
			return std::string();
		}
		
		return fileName;
	}
	
	/** \brief Switch to in-memory PNG data, and set the memory location and length
	 * 
	 * \param memLocationPNGData Pointer to the start of the PNG data in memory
	 * \param lenPNGData Length of the PNG data 
	 * \param pngImageName Name of the image for diagnostic and debugging purposes
	 */
	void setPNGMemoryData(
		char const * memLocationPNGData,
		int lenPNGData,
		std::string const &pngImageName);

	/// \brief Return structure for \ref getMemoryDataInfo()
	struct MemoryDataInfo {
		/// Pointer to the start of the image data in memory.
		/// Is \p nullptr when the source is not in-memory.
		char const * memLocationPNGData;
		/// Length of the image data.
		/// Is 0 when the source is not in-memory
		int lenPNGData;
		/// Name of the image for information purposes only
		std::string pngImageName;
	};
	
	/** \brief Get information of in-memory source of image data
	 
	 \return \ref MemoryDataInfo
	 \see \ref MemoryDataInfo for returned information.
	 */
	MemoryDataInfo getMemoryDataInfo() {
		return MemoryDataInfo {
			.memLocationPNGData=memLocation,
			.lenPNGData=memLen,
			.pngImageName=fileName
		};
	}

	/** \brief Setup the vertex arrays, calculate normals... and setup VBOs
	 *
	 */
	virtual void setupVertexBuffers () override;

	/** \brief Draw the rendered object.
	 *
	 * \param modelMatrix Model matrix, moves the object around from model to world space
	 * \param viewMatrix View matrix, used to move from world to eye space
	 * \param ProjMatrix Projection matrix, used to create the 3-dimensional effects on a 2D screen
	 * \param MVMatrix Model-View Matrix
	 * \param MVPMatrix Model/View/Projection matrix
	 * \param lightDir Direction to the light source, normalized
	 * \param lightColor Color of the light source, normalized color values.
	 * \param ambientLightColor Color of the ambient light, normalized color values.
	 */
	virtual void draw(
			OevGLES::Mat4 const &modelMatrix,
			OevGLES::Mat4 const &viewMatrix,
			OevGLES::Mat4 const &ProjMatrix,
			OevGLES::Mat4 const &MVMatrix,
			OevGLES::Mat4 const &MVPMatrix,
			OevGLES::Vec3 const &lightDir,
			OevGLES::Vec4 const &lightColor,
			OevGLES::Vec4 const &ambientLightColor
			)  override;


private:

	// the memory layout of the vertex array is as follows:
	//	0123	45
	//	Pos		TextureCoordinates
	// The base color and the normals for the texture are constant vertex attributes, therefore not part of the vertex array

	/* The texture is rendered by 2 triangles in a fan forming a square.
	 *
	 *	V2	  V1
	 *1	-------
	 *	|\    |
	 *	| \   |
	 *	|  \  |
	 *	|   \ |
	 *	|    \|
	 *	-------
	 *0	V3	  V0
	 *	0	  1
	 */

	VertexType vertexArray[4]; // Per vertex position as Vec4 and texture coordinate as Vec2

	GLfloat textureBaseColor [4] = {1.0f,1.0f,1.0f,1.0f};
	GLfloat textureNormal [4] = {0.0f, 0.0f, 1.0f, 0.0f};

	OevGLES::GLProgDiffLightTexture* glProgram = 0;

	bool dirty = true;

	GLuint vertexBufferHandle = 0U;
	GLuint vertexArrayHandle = 0U;
	OevGLES::GLTexture varioBackgoundTexture;
	
	/** \brief Name of an external PNG file or name of the image from memory
	 *
	 * When \ref memLocation is \p nullptr it defines path and name of an
	 * external PNG file.
	 * 
	 * Else it defines the name of the image from memory, and is used for diagnostic
	 * and debugging purposes only
	 */
	std::string fileName;
	
	/** \brief Location of the PNG data in memory
	 * 
	 * When you set it you *must* also set \ref memLen. Otherwise exceptions fly.
	 */
	char* const memLocation = nullptr;

	/** \brief length of the PNG data defined in \ref memLocation
	 *
	 * 
	 */
	int memLen = 0;

};

} // namespace OevGLES

#endif /* SQUARETEXTURERENDERER_H_ */
