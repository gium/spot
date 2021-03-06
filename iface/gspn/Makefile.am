## Copyright (C) 2008 Laboratoire de Recherche et Développement
## de l'Epita (LRDE).
## Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
## département Systèmes Répartis Coopératifs (SRC), Université Pierre
## et Marie Curie.
##
## This file is part of Spot, a model checking library.
##
## Spot is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## Spot is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Spot; see the file COPYING.  If not, write to the Free
## Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

AM_CPPFLAGS = -I$(top_srcdir)/src $(BUDDY_CPPFLAGS) $(LIBGSPN_CPPFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

gspndir = $(pkgincludedir)/iface/gspn

gspn_HEADERS = \
  common.hh \
  gspn.hh

lib_LTLIBRARIES = libspotgspn.la libspotgspnssp.la
libspotgspn_la_LIBADD = $(top_builddir)/src/libspot.la
libspotgspn_la_SOURCES = \
  common.cc \
  gspn.cc

noinst_PROGRAMS = \
  dottygspn-rg \
  dottygspn-srg \
  ltlgspn-rg \
  ltlgspn-srg

if WITH_GSPN_SSP
gspn_HEADERS += ssp.hh
noinst_PROGRAMS += \
  dottygspn-ssp \
  ltlgspn-ssp

libspotgspnssp_la_LIBADD = $(top_builddir)/src/libspot.la
libspotgspnssp_la_CPPFLAGS = -DESYMBOLIC $(AM_CPPFLAGS)
libspotgspnssp_la_SOURCES = \
  common.cc \
  ssp.cc
endif


dottygspn_rg_SOURCES = dottygspn.cc
dottygspn_rg_LDADD = libspotgspn.la $(LIBGSPNRG_LDFLAGS)

dottygspn_srg_SOURCES = dottygspn.cc
dottygspn_srg_LDADD = libspotgspn.la $(LIBGSPNSRG_LDFLAGS)

dottygspn_ssp_SOURCES = dottyssp.cc
dottygspn_ssp_LDADD = libspotgspnssp.la $(LIBGSPNSSP_LDFLAGS)

ltlgspn_rg_SOURCES = ltlgspn.cc
ltlgspn_rg_LDADD = libspotgspn.la $(LIBGSPNRG_LDFLAGS)

ltlgspn_srg_SOURCES = ltlgspn.cc
ltlgspn_srg_LDADD = libspotgspn.la $(LIBGSPNSRG_LDFLAGS)

ltlgspn_ssp_SOURCES = ltlgspn.cc
ltlgspn_ssp_LDADD = libspotgspnssp.la $(LIBGSPNSSP_LDFLAGS)
ltlgspn_ssp_CPPFLAGS = -DSSP=1 $(AM_CPPFLAGS)

EXTRA_DIST = \
  examples/DCSwave/DCSWave.def \
  examples/DCSwave/DCSWave.net \
  examples/DCSwave/DCSWave.tobs \
  examples/simple/simple.def \
  examples/simple/simple.net \
  examples/simple/simple.tobs \
  examples/udcs/udcs.def \
  examples/udcs/udcs.net \
  examples/udcs/udcs.tobs \
  $(TESTS)

TESTS = \
  simple.test \
  dcswave.test \
  dcswaveltl.test \
  dcswavefm.test \
  dcswaveeltl.test \
  udcsltl.test \
  udcseltl.test \
  udcsfm.test \
  udcsefm.test

# Each test case depends on defs.
check_SCRIPTS = defs

distclean-local:
	-rm -rf testSubDir
