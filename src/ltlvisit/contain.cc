// Copyright (C) 2006 Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "contain.hh"
#include "destroy.hh"
#include "clone.hh"
#include "ltlast/unop.hh"
#include "tgba/tgbaproduct.hh"
#include "tgbaalgos/gtec/gtec.hh"

namespace spot
{
  namespace ltl
  {

    language_containment_checker::language_containment_checker
      (bdd_dict* dict, bool exprop, bool symb_merge,
       bool branching_postponement, bool fair_loop_approx)
      : dict_(dict), exprop_(exprop), symb_merge_(symb_merge),
      branching_postponement_(branching_postponement),
      fair_loop_approx_(fair_loop_approx)
    {
    }

    language_containment_checker::~language_containment_checker()
    {
      while (!translated_.empty())
	{
	  trans_map::iterator i = translated_.begin();
	  delete i->second.translation;
	  const formula* f = i->first;
	  translated_.erase(i);
	  destroy(f);
	}
    }

    // Check whether L(l) is a subset of L(g).
    bool
    language_containment_checker::contained(const formula* l, const formula* g)
    {
      const record_* rl = register_formula_(l);
      const formula* ng = unop::instance(unop::Not, clone(g));
      const record_* rng = register_formula_(ng);
      destroy(ng);
      bool res = rl->incompatible.find(rng) != rl->incompatible.end();
      return res;
    }

    // Check whether L(l) = L(g).
    bool
    language_containment_checker::equal(const formula* l, const formula* g)
    {
      return contained(l,g) && contained(g,l);
    }

    const language_containment_checker::record_*
    language_containment_checker::register_formula_(const formula* f)
    {
      trans_map::iterator i = translated_.find(f);
      if (i != translated_.end())
	return &i->second;

      const tgba_explicit* e = ltl_to_tgba_fm(f, dict_, exprop_, symb_merge_,
					      branching_postponement_,
					      fair_loop_approx_);
      record_& r = translated_[clone(f)];
      r.translation = e;

      // Check the emptiness of the product of this formula with any
      // other registered formula.
      for (i = translated_.begin(); i != translated_.end(); ++i)
	{
	  if (f == i->first)
	    continue;
	  const tgba* p = new tgba_product(e, i->second.translation);
	  emptiness_check* ec = couvreur99(p);
	  emptiness_check_result* ecr = ec->check();
	  if (!ecr)
	    {
	      r.incompatible.insert(&i->second);
	      i->second.incompatible.insert(&r);
	    }
	  else
	    delete ecr;
	  delete ec;
	  delete p;
	}
      return &r;
    }
  }
}