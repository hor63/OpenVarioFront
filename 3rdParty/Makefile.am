#    This file is part of OpenVarioFront, an electronic variometer for glider planes
#    Copyright (C) 2024  Kai Horstmann
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

ALL_TARGETS=Makefile
CLEAN_TARGETS=Makefile
INSTALL_TARGETS=Makefile

if WITH_SDL
ALL_TARGETS += SDL-build
CLEAN_TARGETS += SDL-clean
INSTALL_TARGETS += SDL-install
endif


if WITH_PANGO
ALL_TARGETS += pango-build
CLEAN_TARGETS += pango-clean
INSTALL_TARGETS += pango-install
endif	

all: $(ALL_TARGETS)

clean: $(CLEAN_TARGETS)

install: $(INSTALL_TARGETS)

if WITH_SDL
SDL-build:
	$(MAKE) -C SDL all VERBOSE=$(V)
	$(MAKE) -C SDL install VERBOSE=$(V)

SDL-clean:
	$(MAKE) -C SDL clean VERBOSE=$(V)

SDL-install:
	$(MAKE) -C SDL install
	
endif

if WITH_PANGO
pango-build:
	if [ "$(V)" == "1" ]; then \
		cd pango; ninja -v ; \
	else \
		cd pango; ninja; \
	fi
	cd pango; ninja install

pango-clean:
	cd pango; ninja clean -v

pango-install:
	cd pango; ninja install
	
endif
