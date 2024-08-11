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

# SUBDIRS= ../GLES
	

noinst_LIBRARIES = libOEV_GLTextRender.a
libOEV_GLTextRender_a_SOURCES = GLTextGlobals.cpp GLTextRenderer.cpp


AM_CXXFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/GLES -I $(top_srcdir)/3rdParty/eigen/Eigen $(LOG4CXX_CXXFLAGS) \
	$(PANGO_FT2_CFLAGS) $(SDL3_CFLAGS) $(PTHREAD_CFLAGS)

AM_LDFLAGS= $(LOG4CXX_LDFLAGS)

