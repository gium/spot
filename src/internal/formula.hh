// Copyright (C) 2003, 2004, 2005, 2008 Laboratoire d'Informatique de
// Paris 6 (LIP6), d�partement Syst�mes R�partis Coop�ratifs (SRC),
// Universit� Pierre et Marie Curie.
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

/// \file internal/formula.hh
/// \brief Generic formula interface
#ifndef SPOT_INTERNAL_FORMULA_HH
# define SPOT_INTERNAL_FORMULA_HH

# include "predecl.hh"
# include "baseformula.hh"

namespace spot
{
  namespace internal
  {
    /// \brief A Generic formula.
    /// \ingroup generic_essential
    /// \ingroup generic_ast
    ///
    /// The only way you can work with a formula is to
    /// build a T::visitor or T::const_visitor.
    template<typename T>
    class formula : public T, public base_formula
    {
    public:
      typedef typename T::visitor visitor;
      typedef typename T::const_visitor const_visitor;

      virtual base_formula* clone() const;
      virtual std::ostream& to_string(std::ostream& os) const;
      virtual void destroy() const;

      /// Entry point for T::visitor instances.
      virtual void accept(visitor& v) = 0;
      /// Entry point for T::const_visitor instances.
      virtual void accept(const_visitor& v) const = 0;

      /// \brief clone this node
      ///
      /// This increments the reference counter of this node (if one is
      /// used).  You should almost never use this method directly as
      /// it doesn't touch the children.  If you want to clone a
      /// whole formula, use a clone visitor instead.
      formula<T>* ref();
      /// \brief release this node
      ///
      /// This decrements the reference counter of this node (if one is
      /// used) and can free the object.  You should almost never use
      /// this method directly as it doesn't touch the children.  If you
      /// want to release a whole formula, use a destroy() visitor instead.
      static void unref(formula<T>* f);
    protected:
      virtual ~formula();

      /// \brief increment reference counter if any
      virtual void ref_();
      /// \brief decrement reference counter if any, return true when
      /// the instance must be deleted (usually when the counter hits 0).
      virtual bool unref_();
    };
  }
}

# include "formula.hxx"

#endif // SPOT_INTERNAL_FORMULA_HH