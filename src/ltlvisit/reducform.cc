// Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
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

#include "reducform.hh"
#include "ltlast/allnodes.hh"
#include <cassert>

#include "lunabbrev.hh"
#include "nenoform.hh"
#include "ltlvisit/destroy.hh"

namespace spot
{
  namespace ltl
  {


    //class unabbreviate_FG_visitor::unabbreviate_logic_visitor()

    class reduce_form_visitor : public visitor
    {
    public:

      reduce_form_visitor()
      {
	/*
	  eventuality_ = false;
	  universality_ = false;
	*/
      }

      virtual ~reduce_form_visitor()
      {
      }

      formula*
      result() const
      {
	return result_;
      }

      /*
	bool
	is_eventuality()
	{
	return eventuality_;
	}

	bool
	is_universality()
	{
	return universality_;
	}
      */

      void
      visit(atomic_prop* ap)
      {
	formula* f = ap->ref();
	result_ = f;
      }

      void
      visit(constant* c)
      {
	switch (c->val())
	  {
	  case constant::True:
	    result_ = constant::true_instance();
	    return;
	  case constant::False:
	    result_ = constant::false_instance();
	    return;
	  }
	/* Unreachable code.  */
	assert(0);
      }

      void
      visit(unop* uo)
      {
	result_ = recurse(uo->child());

	switch (uo->op())
	  {
	  case unop::Not:
	    result_ = unop::instance(unop::Not, result_);
	    return;

	  case unop::X:
	    result_ = unop::instance(unop::X, result_);
	    return;

	  case unop::F:
	    /* if f is class of eventuality then F(f)=f */
	    if (!is_eventual(result_)) {
	      result_ = unop::instance(unop::F, result_);
	    }
	    return;

	  case unop::G:
	    /* if f is class of universality then G(f)=f */
	    if (!is_universal(result_)) {
	      result_ = unop::instance(unop::G, result_);
	    }
	    return;
	  }
	/* Unreachable code.  */
	assert(0);
      }

      void
      visit(binop* bo)
      {
	formula* f2 = recurse(bo->second());

	/* if b is of class eventuality then a U b = b
	   if b is of class universality then a R b = b */
	if ( ( is_eventual(f2) && ( (bo->op()) == binop::U )) ||
	     ( is_universal(f2) && ( (bo->op()) == binop::R )) )
	  {
	    result_ = f2;
	    return;
	  }

	/* case of implies */
	formula* f1 = recurse(bo->first());
	bool inf = inf_form(f1,f2);
	bool infinv = inf_form(f2,f1);
	//binop* ftmp = NULL;
	bool infnegleft = infneg_form(f2,f1,0);
	bool infnegright = infneg_form(f2,f1,1);

	switch (bo->op())
	  {
	  case binop::Xor:
	  case binop::Equiv:
	  case binop::Implies:
	    result_ = binop::instance(bo->op(), f1, f2);
	    return;

	  case binop::U:
	    /* a < b => a U b = b */
	    if (inf)
	      {
		result_ = f2;
		spot::ltl::destroy(f1);
		return;
	      }
	    /* !b < a => a U b = Fb */
	    if (infnegleft)
	      {
		result_ = unop::instance(unop::F, f2);
		spot::ltl::destroy(f1);
		return;
	      }
	    /* a < b => a U (b U c) = (b U c) */
	    if (node_type(f2) == node_type_form_visitor::Binop)
	      if (((binop*)f2)->op() == binop::U)
		if (inf_form(f1,((binop*)f2)->first())){
		  result_ = f2;
		  spot::ltl::destroy(f1);
		  return;
		}
	    result_ = binop::instance(binop::U,
				      f1, f2);
	    return;

	  case binop::R:
	    /* b < a => a R b = b */
	    if (infinv)
	      {
		result_ = f2;
		spot::ltl::destroy(f1);
		return;
	      }
	    /* b < !a => a R b = Gb */
	    if (infnegright)
	      {
		result_ = unop::instance(unop::G, f2);
		spot::ltl::destroy(f1);
		return;
	      }
	    /* b < a => a R (b R c) = b R c */
	    if (node_type(f2) == node_type_form_visitor::Binop)
	      if (((binop*)f2)->op() == binop::R)
		if (inf_form(((binop*)f2)->first(),f1)){
		  result_ = f2;
		  spot::ltl::destroy(f1);
		  return;
		}

	    result_ = binop::instance(binop::R,
				      f1, f2);
	    return;
	  }
	/* Unreachable code.  */
	assert(0);
      }

      void
      visit(multop* mo)
      {
	formula* f1 = NULL;
	formula* f2 = NULL;
	unsigned mos = mo->size();
	multop::vec* res = new multop::vec;
	multop::vec::iterator index;
	multop::vec::iterator indextmp;

	for (unsigned i = 0; i < mos; ++i)
	  res->push_back(recurse(mo->nth(i)));

      	switch (mo->op())
	  {

	  case multop::Or:
	    index = indextmp = res->begin();
	    if (index != res->end())
	      {
		f1 = *index;
		index++;
	      }
	    for (; index != res->end();index++)
	      {
		f2 = *index;
		/* a < b => a + b = b */
		if (inf_form(f1,f2)) // f1 < f2
		  {
		    f1 = f2;
		    spot::ltl::destroy(*indextmp);
		    res->erase(indextmp);
		    indextmp = index;
		    index--;
		  }
		else if (inf_form(f2,f1)) // f2 < f1
		  {
		    spot::ltl::destroy(*index);
		    res->erase(index);
		    index--;
		  }
	      }
	    break;

	  case multop::And:
	    index = indextmp = res->begin();
	    if (mos)
	      {
		f1 = mo->nth(0);
		index++;
	      }
	    for (; index != res->end(); index++)
	      {
		f2 = *index;
		/* a < b => a & b = a */
		if (inf_form(f1,f2)) // f1 < f2
		  {
		    spot::ltl::destroy(*index);
		    res->erase(index);
		    index--;
		  }
		else if (inf_form(f2,f1)) // f2 < f1
		  {
		    f1 = f2;
		    spot::ltl::destroy(*indextmp);
		    res->erase(indextmp);
		    indextmp = index;
		    index--;
		  }
	      }
	    break;
	  }

	/* f1 < !f2 => f1 & f2 = false
	   !f1 < f2 => f1 | f2 = true */

	for (index = res->begin(); index != res->end(); index++){
	  for (indextmp = res->begin(); indextmp != res->end(); indextmp++){
	    if (index != indextmp){
	      if (infneg_form(*index,*indextmp, (mo->op() == multop::Or) ? 0 : 1)) {
		for (multop::vec::iterator j = res->begin(); j != res->end(); j++){
		  spot::ltl::destroy(*j);
		}
		if (mo->op() == multop::Or)
		  result_ = constant::true_instance();
		else
		  result_ = constant::false_instance();
		return;
	      }

	      /*
	      if ((constant*)(*index) != NULL)
		if (((((constant*)(*index))->val() == constant::True) &&
		     (mo->op() == multop::Or)) ||
		    (((((constant*)(*index))->val() == constant::False))
		     && (mo->op() == multop::And))
		    ) {
		  //std::cout << "constant" << std::endl;
		  for (multop::vec::iterator j = res->begin(); j != res->end(); j++){
		    spot::ltl::destroy(*j);
		  }
		  if (mo->op() == multop::Or)
		    result_ = constant::true_instance();
		  else
		    result_ = constant::false_instance();

		  std::cout << "2" << std::endl;
		  spot::ltl::dump(std::cout,mo);
		  return;
		}
	      */

	    }
	  }
	}

	if (res->size()) {
	  result_ = multop::instance(mo->op(),res);
	  return;
	}
	assert(0);
      }

      formula*
      recurse(formula* f)
      {
	return reduce_form(f);
      }

    protected:
      formula* result_;
      /*
	bool eventuality_;
	bool universality_;
      */
    };

    formula*
    reduce_form(const formula* f)
    {
      spot::ltl::formula* ftmp1;
      spot::ltl::formula* ftmp2;
      reduce_form_visitor v;

      ftmp1 = spot::ltl::basic_reduce_form(f);
      const_cast<formula*>(ftmp1)->accept(v);
      ftmp2 = spot::ltl::basic_reduce_form(v.result());

      spot::ltl::destroy(ftmp1);
      spot::ltl::destroy(v.result());

      return ftmp2;
    }

    formula*
    reduce(const formula* f)
    {
      spot::ltl::formula* ftmp1;
      spot::ltl::formula* ftmp2;
      spot::ltl::formula* ftmp3;

      ftmp1 = spot::ltl::unabbreviate_logic(f);
      ftmp2 = spot::ltl::negative_normal_form(ftmp1);
      ftmp3 = spot::ltl::reduce_form(ftmp2);

      spot::ltl::destroy(ftmp1);
      spot::ltl::destroy(ftmp2);

      return ftmp3;

    }

  }
}