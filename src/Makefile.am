#    This file is part of OpenVarioFront, an electronic variometer for glider planes
#    Copyright (C) 2016  Kai Horstmann
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

# SUBDIRS=main
	

bin_PROGRAMS=OpenVarioFront$(EXEEXT)

OpenVarioFront_SOURCES=OpenVarioFront.cpp $(EGL_SYS_DIR)/sysEGLWindow.cpp
 
OpenVarioFront_LDADD= $(LOG4CXX_LIBS) $(PTHREAD_LIBS)

AM_CXXFLAGS = -I$(top_srcdir)/src $(LOG4CXX_CXXFLAGS) \
	$(PTHREAD_CFLAGS)

AM_LDFLAGS=$(LOG4CXX_LDFLAGS) $(EGL_SYS_LIBS)

