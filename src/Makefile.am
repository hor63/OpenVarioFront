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

SUBDIRS=../resources lib/util lib/GL lib/Controls

bin_PROGRAMS=OpenVarioFront$(EXEEXT)

OpenVarioFront_SOURCES=OpenVarioFront.cpp

LOCAL_LIBS_GROUP = \
	lib/Controls/libOEV_Controls.a \
	lib/GL/libOEV_GLES.a \
	lib/util/libOEV_util.a \
	../resources/libOEV_resurces.a \
	./lib/Controls/libOEV_Controls.a \
	./lib/GL/libOEV_GLES.a \
	./lib/util/libOEV_util.a \
	./../resources/libOEV_resurces.a


OpenVarioFront_LDADD = \
	$(LOCAL_LIBS_GROUP) \
	$(EXTERN_LIBRARY_LIBS)
	
#	$(LOG4CXX_LIBS) \
#	$(SDL3_LIBS) \
#	$(PANGO_FT2_LIBS) \
#	$(LIBPNG_LIBS) \
#	$(LIBJPEG_LIBS) \
#	$(GLES2_LIBS)

AM_CXXFLAGS=$(INTERNAL_CFLAGS) \
	$(EXTERN_LIBRARY_CFLAGS)

#	$(LOG4CXX_CXXFLAGS) $(FREETYPE2_CFLAGS) $(PANGO_FT2_CFLAGS) \
#	$(LIBPNG_CFLAGS) $(LIBJPEG_CFLAGS) \
#	$(SDL3_CFLAGS) $(GLES2_CFLAGS) $(PTHREAD_CFLAGS)

AM_LDFLAGS= \
	$(EXTERN_LIBRARY_LDFLAGS) 

all: $(abs_builddir)/OpenVarioFront.logger.properties

$(abs_builddir)/OpenVarioFront.logger.properties: $(srcdir)/OpenVarioFront.logger.properties
	cp $(srcdir)/OpenVarioFront.logger.properties $(abs_builddir)/OpenVarioFront.logger.properties
	
	
SUFFIXES= .h .gch

.h.gch:
	$(AM_V_CXX)$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -o $@ $<
	$(AM_V_at)$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po
	$(AM_V_at)cp -pv $@ $(srcdir)/$*.h.gch

./$(DEPDIR)/OVFCommon.Po:
	@$(MKDIR_P) $(@D)
	@echo '# dummy' >$@-t && $(am__mv) $@-t $@

include ./$(DEPDIR)/OVFCommon.Po # am--include-marker

OVFCommon.gch: $(srcdir)/OVFCommon.h 

$(OpenVarioFront_OBJECTS): OVFCommon.gch

clean-am: clean-precomp-header

clean-precomp-header:
	-echo "Remove precompiled header"
	-rm -f OVFCommon.gch $(srcdir)/OVFCommon.h.gch
