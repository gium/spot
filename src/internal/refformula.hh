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

/// \file internal/refformula.hh
/// \brief Reference-counted generic formulae
#ifndef SPOT_INTERNAL_REFFORMULA_HH
# define SPOT_INTERNAL_REFFORMULA_HH

#include "formula.hh"
#include <cassert>

namespace spot
{
  namespace internal
  {

    /// \brief A reference-counted generic formula.
    /// \ingroup generic_ast
    template<typename T>
    class ref_formula : public formula<T>
    {
    protected:
      virtual ~ref_formula();
      ref_formula();
      void ref_();
      bool unref_();
      /// Number of references to this formula.
      unsigned ref_count_();

    private:
      unsigned ref_counter_;
    };

  }
}

# include "refformula.hxx"

#endif // SPOT_INTERNAL_REFFORMULA_HH