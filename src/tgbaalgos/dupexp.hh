// Copyright (C) 2003, 2004, 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
// d�partement Syst�mes R�partis Coop�ratifs (SRC), Universit� Pierre
// et Marie Curie.
//
// This file is part of Spot, a model checking library.
//
// Spot is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Spot is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
// or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#ifndef SPOT_TGBAALGOS_DUPEXP_HH
# define SPOT_TGBAALGOS_DUPEXP_HH

# include "tgba/tgbaexplicit.hh"

namespace spot
{
  /// \brief Build an explicit automata from all states of \a aut,
  /// numbering states in bread first order as they are processed.
  /// \ingroup tgba_misc
  tgba_explicit* tgba_dupexp_bfs(const tgba* aut);
  /// \brief Build an explicit automata from all states of \a aut,
  /// numbering states in depth first order as they are processed.
  /// \ingroup tgba_misc
  tgba_explicit* tgba_dupexp_dfs(const tgba* aut);
}

#endif // SPOT_TGBAALGOS_DUPEXP_HH
