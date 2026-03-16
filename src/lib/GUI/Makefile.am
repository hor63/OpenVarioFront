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

noinst_LIBRARIES = libOEV_GUI.a
libOEV_GUI_a_SOURCES = \
	Controls/ControlBase.cpp Controls/ControlsContainer.cpp Controls/RootControl.cpp Controls/PlainFieldControl.cpp \
	Controls/TextFieldControl.cpp Controls/ButtonControl.cpp

AM_CXXFLAGS=$(INTERNAL_CFLAGS) \
	$(EXTERN_LIBRARY_CFLAGS)

#	$(LOG4CXX_CXXFLAGS) $(FREETYPE2_CFLAGS) $(PANGO_FT2_CFLAGS) \
#	$(LIBPNG_CFLAGS) $(LIBJPEG_CFLAGS) \
#	$(SDL3_CFLAGS) $(GLES2_CFLAGS) $(PTHREAD_CFLAGS)


AM_LDFLAGS= $(EXTERN_LIBRARY_LDFLAGS)


# Re-compile the precompiled header
PRECOMP_HEADER_DIR = $(abs_top_builddir)/src
PRECOMP_HEADER = OVFCommon.gch
-include $(PRECOMP_HEADER_DIR)/.deps/OVFCommon.Po # am--include-marker

$(libOEV_GUI_a_OBJECTS): $(PRECOMP_HEADER_DIR)/$(PRECOMP_HEADER)

$(PRECOMP_HEADER_DIR)/$(PRECOMP_HEADER):
	$(MAKE) -C $(PRECOMP_HEADER_DIR) $(PRECOMP_HEADER)
