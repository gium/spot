## Copyright (C) 2010, 2011 Laboratoire de Recherche et Development de
## l'Epita (LRDE).
## Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

SUBDIRS = . cgi-bin ajax tests

AM_CPPFLAGS = -I$(PYTHONINC) -I$(top_srcdir)/src $(BUDDY_CPPFLAGS) \
	      -DSWIG_TYPE_TABLE=spot

EXTRA_DIST = spot.i buddy.i
python_PYTHON = $(srcdir)/spot.py $(srcdir)/buddy.py
pyexec_LTLIBRARIES = _spot.la _buddy.la

MAINTAINERCLEANFILES = \
  $(srcdir)/spot_wrap.cxx $(srcdir)/spot.py \
  $(srcdir)/buddy_wrap.cxx $(srcdir)/buddy.py

## spot

_spot_la_SOURCES = $(srcdir)/spot_wrap.cxx $(srcdir)/spot_wrap.h
_spot_la_LDFLAGS = -avoid-version -module
_spot_la_LIBADD = $(top_builddir)/src/libspot.la

$(srcdir)/spot_wrap.cxx: $(srcdir)/spot.i
	$(SWIG) -c++ -python -I$(srcdir) -I$(top_srcdir)/src $(srcdir)/spot.i

$(srcdir)/spot.py: $(srcdir)/spot.i
	$(MAKE) $(AM_MAKEFLAGS) spot_wrap.cxx


## buddy

_buddy_la_SOURCES = $(srcdir)/buddy_wrap.cxx
_buddy_la_LDFLAGS = -avoid-version -module $(BUDDY_LDFLAGS)

$(srcdir)/buddy_wrap.cxx: $(srcdir)/buddy.i
	$(SWIG) -c++ -python $(BUDDY_CPPFLAGS) $(srcdir)/buddy.i

$(srcdir)/buddy.py: $(srcdir)/buddy.i
	$(MAKE) $(AM_MAKEFLAGS) buddy_wrap.cxx
