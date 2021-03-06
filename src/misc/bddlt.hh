// Copyright (C) 2003, 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#ifndef SPOT_MISC_BDDLT_HH
# define SPOT_MISC_BDDLT_HH

# include <bdd.h>
# include <functional>

namespace spot
{
  /// \brief Comparison functor for BDDs.
  /// \ingroup misc_tools
  struct bdd_less_than :
    public std::binary_function<const bdd&, const bdd&, bool>
  {
    bool
    operator()(const bdd& left, const bdd& right) const
    {
      return left.id() < right.id();
    }
  };
}

#endif // SPOT_MISC_BDDLT_HH
