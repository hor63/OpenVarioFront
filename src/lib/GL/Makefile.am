#    This file is part of OpenVarioFront, an electronic variometer for glider planes
#    Copyright (C) 2018  Kai Horstmann
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

noinst_LIBRARIES = libOEV_GLES.a
libOEV_GLES_a_SOURCES = GLES/GLFramework.cpp \
	GLES/SDL/sysSDLWindow.cpp GLES/SDL/SDLRenderSurface.cpp \
	GLES/GLShader.cpp GLES/GLProgram.cpp \
	GLES/GLTexture.cpp GLES/GLObjectWrappers.cpp \
	GLES/TexHelper/TextureData.cpp \
	GLES/TexHelper/ImageReaderBase.cpp \
	GLES/TexHelper/PngReader.cpp GLES/TexHelper/JpegReader.cpp \
	GLPrograms/GLProgBase.cpp GLPrograms/GLProgDiffuseLight.cpp \
	GLPrograms/GLProgDiffLightTexture.cpp GLPrograms/GLProgTextTexture.cpp \
	GLPrograms/GLProgDiffLightCircle.cpp GLPrograms/GLProgSimpleFill.cpp \
	GLPrograms/GLProgControlSimpleFill.cpp \
	Renderers/RenderContext.cpp \
	Renderers/RendererBase.cpp Renderers/AnalogHandRenderer.cpp \
	Renderers/SquareTextureRenderer.cpp Renderers/CirclePolygonVertexContainer.cpp \
	Renderers/CircleBaseRenderer.cpp Renderers/CircleFilledRenderer.cpp \
	Renderers/CirclePartialArcRenderer.cpp \
	GLTextRender/GLTextGlobals.cpp GLTextRender/GLTextRenderer.cpp \
	GLTextRender/GLTextFontCache.cpp GLTextRender/GLTextPangoCPPWrappers.cpp \
	GLTextRender/GLTextFontTexture.cpp

AM_CXXFLAGS=$(INTERNAL_CFLAGS) \
	$(EXTERN_LIBRARY_CFLAGS)

#	$(LOG4CXX_CXXFLAGS) $(FREETYPE2_CFLAGS) $(PANGO_FT2_CFLAGS) \
#	$(LIBPNG_CFLAGS) $(LIBJPEG_CFLAGS) \
#	$(SDL3_CFLAGS) $(GLES2_CFLAGS) $(PTHREAD_CFLAGS)


AM_LDFLAGS= $(EXTERN_LIBRARY_LDFLAGS)

