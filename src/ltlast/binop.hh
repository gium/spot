// Copyright (C) 2009, 2010 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris
// 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
// Université Pierre et Marie Curie.
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

/// \file ltlast/binop.hh
/// \brief LTL binary operators
///
/// This does not include \c AND and \c OR operators.  These are
/// considered to be multi-operand operators (see spot::ltl::multop).
#ifndef SPOT_LTLAST_BINOP_HH
# define SPOT_LTLAST_BINOP_HH

#include <map>
#include <iosfwd>
#include "refformula.hh"

namespace spot
{
  namespace ltl
  {

    /// \brief Binary operator.
    /// \ingroup ltl_ast
    class binop : public ref_formula
    {
    public:
      /// Different kinds of binary opertaors
      ///
      /// And and Or are not here.  Because they
      /// are often nested we represent them as multops.
      enum type { Xor,
		  Implies,
		  Equiv,
		  U, //< until
		  R, //< release (dual of until)
		  W, //< weak until
		  M  //< strong release (dual of weak until)
      };

      /// Build an unary operator with operation \a op and
      /// children \a first and \a second.
      static binop* instance(type op, formula* first, formula* second);

      virtual void accept(visitor& v);
      virtual void accept(const_visitor& v) const;

      /// Get the first operand.
      const formula* first() const;
      /// Get the first operand.
      formula* first();
      /// Get the second operand.
      const formula* second() const;
      /// Get the second operand.
      formula* second();

      /// Get the type of this operator.
      type op() const;
      /// Get the type of this operator, as a string.
      const char* op_name() const;

      /// Return a canonic representation of the atomic proposition
      virtual std::string dump() const;

      /// Number of instantiated binary operators.  For debugging.
      static unsigned instance_count();

      /// Dump all instances.  For debugging.
      static std::ostream& dump_instances(std::ostream& os);

    protected:
      typedef std::pair<formula*, formula*> pairf;
      typedef std::pair<type, pairf> pair;
      typedef std::map<pair, binop*> map;
      static map instances;

      binop(type op, formula* first, formula* second);
      virtual ~binop();

    private:
      type op_;
      formula* first_;
      formula* second_;
    };

  }
}

#endif // SPOT_LTLAST_BINOP_HH
