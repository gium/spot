## Copyright (C) 2011  Laboratoire d'Informatique de Paris 6 (LIP6),
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

nodist_noinst_SCRIPTS = spot.py
EXTRA_DIST = $(srcdir)/spot.in README ltl2tgba.html css/ltl2tgba.css	\
  css/tipTip.css js/jquery.tipTip.minified.js logos/lip6sys64.png	\
  logos/lrde64.png logos/spot64s.png logos/mail.png			\
  css/ui-lightness/jquery-ui-1.8.8.custom.css				\
  css/ui-lightness/images/ui-icons_222222_256x240.png			\
  css/ui-lightness/images/ui-bg_glass_100_f6f6f6_1x400.png		\
  css/ui-lightness/images/ui-bg_glass_100_fdf5ce_1x400.png		\
  css/ui-lightness/images/ui-bg_diagonals-thick_18_b81900_40x40.png	\
  css/ui-lightness/images/ui-icons_ffd27a_256x240.png			\
  css/ui-lightness/images/ui-icons_ffffff_256x240.png			\
  css/ui-lightness/images/ui-icons_228ef1_256x240.png			\
  css/ui-lightness/images/ui-bg_flat_10_000000_40x100.png		\
  css/ui-lightness/images/ui-bg_highlight-soft_100_eeeeee_1x100.png	\
  css/ui-lightness/images/ui-icons_ef8c08_256x240.png			\
  css/ui-lightness/images/ui-bg_gloss-wave_35_f6a828_500x100.png	\
  css/ui-lightness/images/ui-bg_glass_65_ffffff_1x400.png		\
  css/ui-lightness/images/ui-bg_diagonals-thick_20_666666_40x40.png	\
  css/ui-lightness/images/ui-bg_highlight-soft_75_ffe45c_1x100.png

CLEANFILES = $(nodist_noinst_SCRIPTS)

spot.py: $(srcdir)/spot.in Makefile
	sed -e 's|[@]PYTHON[@]|@PYTHON@|g' \
	    -e 's|[@]pythondir[@]|@pythondir@|g' \
	    -e 's|[@]srcdir[@]|@srcdir@|g' \
	    -e 's|[@]top_builddir[@]|@top_builddir@|g' \
	    -e 's|[@]PACKAGE_VERSION[@]|@PACKAGE_VERSION@|g' \
	    -e 's|[@]DOT[@]|@DOT@|g' \
	    <$(srcdir)/spot.in >spot.tmp
	chmod +x spot.tmp
	mv -f spot.tmp $@

clean-local:
	rm -rf spotimg
