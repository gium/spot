## Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
## d�partement Syst�mes R�partis Coop�ratifs (SRC), Universit� Pierre
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

AM_CPPFLAGS = -I$(srcdir)/.. $(BUDDY_CPPFLAGS)
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

evtgbaalgosdir = $(pkgincludedir)/evtgbaalgos

evtgbaalgos_HEADERS = \
  dotty.hh \
  reachiter.hh \
  save.hh \
  tgba2evtgba.hh

noinst_LTLIBRARIES = libevtgbaalgos.la
libevtgbaalgos_la_SOURCES = \
  dotty.cc \
  reachiter.cc \
  save.cc \
  tgba2evtgba.cc
