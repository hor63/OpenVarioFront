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


# Add assembler sources to force the declaration of the assembler command and
# options which are being used to invoke the assembler to package the binary
# file into an object file
SUFFIXES = .png .jpg .jpeg .s .S

BINC = $(srcdir)/binary2obj.sh

.png.o:
	CCAS="$(CCAS)" CCASFLAGS="$(CCASFLAGS)" $(BINC) $@ $< $*

.jpg.o:
	CCAS="$(CCAS)" CCASFLAGS="$(CCASFLAGS)" $(BINC) $@ $< $*

.jpeg.o:
	CCAS="$(CCAS)" CCASFLAGS="$(CCASFLAGS)" $(BINC) $@ $< $*


noinst_LIBRARIES = libOEV_resurces.a
libOEV_resurces_a_SOURCES = Vario5m.png


AM_LDFLAGS= $(LOG4CXX_LDFLAGS)

