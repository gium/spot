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

#include <cstring>
#include <map>
#include <cassert>
#include <gspnlib.h>
#include "ssp.hh"
#include "misc/bddlt.hh"
#include <bdd.h>
#include "tgbaalgos/gtec/explscc.hh"
#include "tgbaalgos/gtec/nsheap.hh"

namespace spot
{

  namespace
  {
    static bdd*
    bdd_realloc(bdd* t, int size, int new_size)
    {
      assert(new_size);
      bdd* tmp = new bdd[new_size];

      for(int i = 0; i < size; i++)
	tmp[i] = t[i];

      delete[] t;
      return tmp;
    }
  }

  // state_gspn_ssp
  //////////////////////////////////////////////////////////////////////

  class state_gspn_ssp: public state
  {
  public:
    state_gspn_ssp(State left, const state* right)
      : left_(left), right_(right)
    {
    }

    virtual
    ~state_gspn_ssp()
    {
      delete right_;
    }

    virtual int
    compare(const state* other) const
    {
      const state_gspn_ssp* o = dynamic_cast<const state_gspn_ssp*>(other);
      assert(o);
      int res = (reinterpret_cast<char*>(o->left())
		 - reinterpret_cast<char*>(left()));
      if (res != 0)
	return res;
      return right_->compare(o->right());
    }

    virtual size_t
    hash() const
    {
      return (reinterpret_cast<char*>(left())
	      - static_cast<char*>(0)) << 10 + right_->hash();
    }

    virtual state_gspn_ssp* clone() const
    {
      return new state_gspn_ssp(left(), right()->clone());
    }

    State
    left() const
    {
      return left_;
    }

    const state*
    right() const
    {
      return right_;
    }

  private:
    State left_;
    const state* right_;
  }; // state_gspn_ssp

  // tgba_gspn_ssp_private_
  //////////////////////////////////////////////////////////////////////

  struct tgba_gspn_ssp_private_
  {
    int refs;			// reference count

    bdd_dict* dict;
    typedef std::map<int, AtomicProp> prop_map;
    prop_map prop_dict;

    signed char* all_props;

    size_t prop_count;
    const tgba* operand;

    tgba_gspn_ssp_private_(bdd_dict* dict, const gspn_environment& env,
			     const tgba* operand)
      : refs(1), dict(dict), all_props(0),
	operand(operand)
    {
      const gspn_environment::prop_map& p = env.get_prop_map();

      try
	{
	  AtomicProp max_prop = 0;

	  for (gspn_environment::prop_map::const_iterator i = p.begin();
	       i != p.end(); ++i)
	    {
	      int var = dict->register_proposition(i->second, this);
	      AtomicProp index;
	      int err = prop_index(i->first.c_str(), &index);
	      if (err)
		throw gspn_exeption("prop_index(" + i->first + ")", err);

	      prop_dict[var] = index;

	      max_prop = std::max(max_prop, index);
	    }
	  prop_count = 1 + max_prop;
	  all_props = new signed char[prop_count];
	}
      catch (...)
	{
	  // If an exception occurs during the loop, we need to clean
	  // all BDD variables which have been registered so far.
	  dict->unregister_all_my_variables(this);
	  throw;
	}
    }

    tgba_gspn_ssp_private_::~tgba_gspn_ssp_private_()
    {
      dict->unregister_all_my_variables(this);
      if (all_props)
	delete[] all_props;
    }
  };


  // tgba_succ_iterator_gspn_ssp
  //////////////////////////////////////////////////////////////////////

  class tgba_succ_iterator_gspn_ssp: public tgba_succ_iterator
  {
  public:
    tgba_succ_iterator_gspn_ssp(Succ_* succ_tgba,
				  size_t size_tgba,
				  bdd* bdd_arry,
				  state** state_arry,
				  size_t size_states,
				  Props_* prop,
				  int size_prop)
      : successors_(succ_tgba),
	size_succ_(size_tgba),
	current_succ_(0),
	bdd_array_(bdd_arry),
	state_array_(state_arry),
	size_states_(size_states),
	props_(prop),
	size_prop_(size_prop)
    {
    }

    virtual
    ~tgba_succ_iterator_gspn_ssp()
    {

      for(size_t i = 0; i < size_states_; i++)
      	delete state_array_[i];

      delete[] bdd_array_;
      free(state_array_);

      if (props_)
	{
	  for (int i = 0; i < size_prop_; i++)
	    free(props_[i].arc);
	  free(props_);
	}

      if (size_succ_ != 0)
	succ_free(successors_);

    }

    virtual void
    first()
    {
      if(!successors_)
	return    ;
      current_succ_=0;
    }

    virtual void
    next()
    {
      current_succ_++;
    }

    virtual bool
    done() const
    {
      return current_succ_ + 1 > size_succ_;
    }

    virtual state*
    current_state() const
    {
      return
	new state_gspn_ssp(successors_[current_succ_].succ_,
			     (state_array_[successors_[current_succ_]
					   .arc->curr_state])->clone());
    }

    virtual bdd
    current_condition() const
    {
      return bddtrue;
    }

    virtual bdd
    current_acceptance_conditions() const
    {
      // There is no acceptance conditions in GSPN systems, so we just
      // return those from OPERAND_.
      // return operand_->current_acceptance_conditions();
      // bdd * ac=(bdd *)successors_[current_succ_].arc->curr_acc_conds;
      //return (*ac);
      return bdd_array_[successors_[current_succ_].arc->curr_acc_conds];
    }
  private:

    // All successors of STATE matching a selection conjunctions from
    // ALL_CONDS.
    Succ_* successors_;		/// array of successors
    size_t size_succ_;		/// size of successors_
    size_t current_succ_;		/// current position in successors_

    bdd * bdd_array_;
    state** state_array_;
    size_t size_states_;
    Props_* props_;
    int size_prop_;
  }; // tgba_succ_iterator_gspn_ssp


  // tgba_gspn_ssp
  //////////////////////////////////////////////////////////////////////

  class tgba_gspn_ssp: public tgba
  {
  public:
    tgba_gspn_ssp(bdd_dict* dict, const gspn_environment& env,
		    const tgba* operand);
    tgba_gspn_ssp(const tgba_gspn_ssp& other);
    tgba_gspn_ssp& operator=(const tgba_gspn_ssp& other);
    virtual ~tgba_gspn_ssp();
    virtual state* get_init_state() const;
    virtual tgba_succ_iterator*
    succ_iter(const state* local_state,
	      const state* global_state = 0,
	      const tgba* global_automaton = 0) const;
    virtual bdd_dict* get_dict() const;
    virtual std::string format_state(const state* state) const;
    virtual state* project_state(const state* s, const tgba* t) const;
    virtual bdd all_acceptance_conditions() const;
    virtual bdd neg_acceptance_conditions() const;
  protected:
    virtual bdd compute_support_conditions(const spot::state* state) const;
    virtual bdd compute_support_variables(const spot::state* state) const;
  private:
    tgba_gspn_ssp_private_* data_;
  };

  tgba_gspn_ssp::tgba_gspn_ssp(bdd_dict* dict, const gspn_environment& env,
				   const tgba* operand)
  {
    data_ = new tgba_gspn_ssp_private_(dict, env, operand);
  }

  tgba_gspn_ssp::tgba_gspn_ssp(const tgba_gspn_ssp& other)
    : tgba()
  {
    data_ = other.data_;
    ++data_->refs;
  }

  tgba_gspn_ssp::~tgba_gspn_ssp()
  {
    if (--data_->refs == 0)
      delete data_;
  }

  tgba_gspn_ssp&
  tgba_gspn_ssp::operator=(const tgba_gspn_ssp& other)
  {
    if (&other == this)
      return *this;
    this->~tgba_gspn_ssp();
    new (this) tgba_gspn_ssp(other);
    return *this;
  }

  state* tgba_gspn_ssp::get_init_state() const
  {
    // Use 0 as initial state for the SSP side.  State 0 does not
    // exists, but when passed to succ() it will produce the list
    // of initial states.
    return new state_gspn_ssp(0, data_->operand->get_init_state());
  }

  tgba_succ_iterator*
  tgba_gspn_ssp::succ_iter(const state* state_,
			     const state* global_state,
			     const tgba* global_automaton) const
  {
    const state_gspn_ssp* s = dynamic_cast<const state_gspn_ssp*>(state_);
    assert(s);
    (void) global_state;
    (void) global_automaton;

    bdd all_conds_;
    bdd outside_;
    bdd cond;

    Props_* props_ = 0;
    int nb_arc_props = 0;
    bdd* bdd_array = 0;
    int size_bdd = 0;
    state** state_array = 0;
    size_t size_states = 0;

    tgba_succ_iterator* i = data_->operand->succ_iter(s->right());

    for (i->first(); !i->done(); i->next())
      {
	all_conds_ = i->current_condition();
	outside_ = !all_conds_;

	if (all_conds_ != bddfalse)
	  {
	    props_ = (Props_*) realloc(props_,
				       (nb_arc_props + 1) * sizeof(Props_));

	    props_[nb_arc_props].nb_conj = 0;
	    props_[nb_arc_props].prop = 0;
	    props_[nb_arc_props].arc =
	      (Arc_Ident_*) malloc(sizeof(Arc_Ident_));

	    bdd_array = bdd_realloc(bdd_array, size_bdd, size_bdd + 1);
	    bdd_array[size_bdd] = i->current_acceptance_conditions();
	    props_[nb_arc_props].arc->curr_acc_conds = size_bdd;
	    size_bdd++;

            state_array = (state**) realloc(state_array,
					    (size_states + 1) * sizeof(state*));
	    state_array[size_states] = i->current_state();
	    props_[nb_arc_props].arc->curr_state  = size_states ;
            size_states++;

	    while (all_conds_ != bddfalse )
	      {
		cond = bdd_satone(all_conds_);
		cond = bdd_simplify(cond, cond | outside_);
		all_conds_ -= cond;

		props_[nb_arc_props].prop =
		  (signed char **) realloc(props_[nb_arc_props].prop,
					   (props_[nb_arc_props].nb_conj + 1)
					   * sizeof(signed char *));

		props_[nb_arc_props].prop[props_[nb_arc_props].nb_conj]
		  = (signed char*) calloc(data_->prop_count,
					  sizeof(signed char));
		memset(props_[nb_arc_props].prop[props_[nb_arc_props].nb_conj],
		       -1, data_->prop_count);

		while (cond != bddtrue)
		  {
		    int var = bdd_var(cond);
		    bdd high = bdd_high(cond);
		    int res;

		    if (high == bddfalse)
		      {
			cond = bdd_low(cond);
			res = 0;
		      }
		    else
		      {
			cond = high;
			res = 1;
		      }

		    tgba_gspn_ssp_private_::prop_map::iterator k
		      = data_->prop_dict.find(var);

		    if (k != data_->prop_dict.end())
		      props_[nb_arc_props]
			.prop[props_[nb_arc_props].nb_conj][k->second] = res;

		    assert(cond != bddfalse);
		  }
		++props_[nb_arc_props].nb_conj;
	      }
	    ++nb_arc_props;
	  }
      }
     Succ_* succ_tgba_ = 0;
     size_t size_tgba_ = 0;
     int j, conj;

     succ(s->left(), props_, nb_arc_props, &succ_tgba_, &size_tgba_);

     for (j = 0; j < nb_arc_props; j++)
       {
	 for (conj = 0 ; conj < props_[j].nb_conj ; conj++)
	   free(props_[j].prop[conj]);
	 free(props_[j].prop);
       }

     delete i;
     return new tgba_succ_iterator_gspn_ssp(succ_tgba_, size_tgba_,
					      bdd_array, state_array,
					      size_states, props_,
					      nb_arc_props);
  }

  bdd
  tgba_gspn_ssp::compute_support_conditions(const spot::state* state) const
  {
    (void) state;
    return bddtrue;
  }

  bdd
  tgba_gspn_ssp::compute_support_variables(const spot::state* state) const
  {
    (void) state;
    return bddtrue;
  }

  bdd_dict*
  tgba_gspn_ssp::get_dict() const
  {
    return data_->dict;
  }

  std::string
  tgba_gspn_ssp::format_state(const state* state) const
  {
    const state_gspn_ssp* s = dynamic_cast<const state_gspn_ssp*>(state);
    assert(s);
    char* str;
    State gs = s->left();
    if (gs)
      {
	int err = print_state(gs, &str);
	if (err)
	  throw gspn_exeption("print_state()", err);
	// Strip trailing \n...
	unsigned len = strlen(str);
	while (str[--len] == '\n')
	  str[len] = 0;
      }
    else
      {
	str = strdup("-1");
      }

    std::string res(str);
    free(str);
    return res + " * " + data_->operand->format_state(s->right());
  }

  state*
  tgba_gspn_ssp::project_state(const state* s, const tgba* t) const
  {
    const state_gspn_ssp* s2 = dynamic_cast<const state_gspn_ssp*>(s);
    assert(s2);
    if (t == this)
      return s2->clone();
    return data_->operand->project_state(s2->right(), t);
  }

  bdd
  tgba_gspn_ssp::all_acceptance_conditions() const
  {
    // There is no acceptance conditions in GSPN systems, they all
    // come from the operand automaton.
    return data_->operand->all_acceptance_conditions();
  }

  bdd
  tgba_gspn_ssp::neg_acceptance_conditions() const
  {
    // There is no acceptance conditions in GSPN systems, they all
    // come from the operand automaton.
    return data_->operand->neg_acceptance_conditions();
  }

  // gspn_ssp_interface
  //////////////////////////////////////////////////////////////////////

  gspn_ssp_interface::gspn_ssp_interface(int argc, char **argv,
					     bdd_dict* dict,
					     const gspn_environment& env)
    : dict_(dict), env_(env)
  {
    int res = initialize(argc, argv);
    if (res)
      throw gspn_exeption("initialize()", res);
  }

  gspn_ssp_interface::~gspn_ssp_interface()
  {
    int res = finalize();
    if (res)
      throw gspn_exeption("finalize()", res);
  }

  tgba*
  gspn_ssp_interface::automaton(const tgba* operand) const
  {
    return new tgba_gspn_ssp(dict_, env_, operand);
  }


  //////////////////////////////////////////////////////////////////////

  class connected_component_ssp: public explicit_connected_component
  {
  public:
    virtual
    ~connected_component_ssp()
    {
    }

    virtual const state*
    has_state(const state* s) const
    {
      set_type::iterator i;

      for (i = states.begin(); i !=states.end(); i++)
	{
	  const state_gspn_ssp* old_state = (const state_gspn_ssp*)(*i);
	  const state_gspn_ssp* new_state = (const state_gspn_ssp*)(s);

	  if ((old_state->right())->compare(new_state->right()) == 0
	      && old_state->left()
	      && new_state->left())
	    if (spot_inclusion(new_state->left(), old_state->left()))
	      {
		if (*i != s)
		  delete s;
		return *i;
	      }
	}
      return 0;
    }

    virtual void
    insert(const state* s)
    {
      states.insert(s);
    }

  protected:
    typedef Sgi::hash_set<const state*,
			  state_ptr_hash, state_ptr_equal> set_type;
    set_type states;
  };

  class connected_component_ssp_factory :
    public explicit_connected_component_factory
  {
  public:
    virtual connected_component_ssp*
    build() const
    {
      return new connected_component_ssp();
    }

    /// Get the unique instance of this class.
    static const connected_component_ssp_factory*
    instance()
    {
      static connected_component_ssp_factory f;
      return &f;
    }

  protected:
    virtual
    ~connected_component_ssp_factory()
    {
    }
    /// Construction is forbiden.
    connected_component_ssp_factory()
    {
    }
  };

  //////////////////////////////////////////////////////////////////////


  class numbered_state_heap_ssp_semi : public numbered_state_heap
  {
  public:
    virtual
    ~numbered_state_heap_ssp_semi()
    {
      // Free keys in H.
      hash_type::iterator i = h.begin();
      while (i != h.end())
	{
	  // Advance the iterator before deleting the key.
	  const state* s = i->first;
	  ++i;
	  delete s;
	}
    }

    virtual numbered_state_heap::state_index
    find(const state* s) const
    {
      state_index res;

      hash_type::const_iterator i;
      for (i = h.begin(); i != h.end(); ++i)
	{
	  const state_gspn_ssp* old_state =
	    dynamic_cast<const state_gspn_ssp*>(i->first);
	  const state_gspn_ssp* new_state =
	    dynamic_cast<const state_gspn_ssp*>(s);
	  assert(old_state);
	  assert(new_state);

	  if ((old_state->right())->compare(new_state->right()) == 0)
	    {
	      if (old_state->left() == new_state->left())
		break;

	      if (old_state->left()
		  && new_state->left()
		  && spot_inclusion(new_state->left(),old_state->left()))
		break;
	    }
	}

      if (i == h.end())
	{
	  res.first = 0;
	  res.second = 0;
	}
      else
	{
	  res.first = i->first;
	  res.second = i->second;

	  if (s != i->first)
	    delete s;
	}
      return res;
    }

    virtual numbered_state_heap::state_index_p
    find(const state* s)
    {
      state_index_p res;

      hash_type::iterator i;
      for (i = h.begin(); i != h.end(); ++i)
	{
	  const state_gspn_ssp* old_state =
	    dynamic_cast<const state_gspn_ssp*>(i->first);
	  const state_gspn_ssp* new_state =
	    dynamic_cast<const state_gspn_ssp*>(s);
	  assert(old_state);
	  assert(new_state);

	  if ((old_state->right())->compare(new_state->right()) == 0)
	    {
	      if (old_state->left() == new_state->left())
		break;

	      if (old_state->left()
		  && new_state->left()
		  && spot_inclusion(new_state->left(),old_state->left()))
		break;
	    }
	}

      if (i == h.end())
	{
	  res.first = 0;
	  res.second = 0;
	}
      else
	{
	  res.first = i->first;
	  res.second = &i->second;

	  if (s != i->first)
	    delete s;
	}
      return res;
    }

    virtual numbered_state_heap::state_index
    index(const state* s) const
    {
      state_index res;
      hash_type::const_iterator i = h.find(s);
      if (i == h.end())
	{
	  res.first = 0;
	  res.second = 0;
	}
      else
	{
	  res.first = i->first;
	  res.second = i->second;

	  if (s != i->first)
	    delete s;
	}
      return res;
    }

    virtual numbered_state_heap::state_index_p
    index(const state* s)
    {
      state_index_p res;
      hash_type::iterator i = h.find(s);
      if (i == h.end())
	{
	  res.first = 0;
	  res.second = 0;
	}
      else
	{
	  res.first = i->first;
	  res.second = &i->second;

	  if (s != i->first)
	    delete s;
	}
      return res;
    }

    virtual void
    insert(const state* s, int index)
    {
      h[s] = index;
    }

    virtual int
    size() const
    {
      return h.size();
    }

    virtual numbered_state_heap_const_iterator* iterator() const;

  protected:
    typedef Sgi::hash_map<const state*, int,
			  state_ptr_hash, state_ptr_equal> hash_type;
    hash_type h;		///< Map of visited states.

    friend class numbered_state_heap_ssp_const_iterator;
    friend class emptiness_check_shy_ssp;
  };


  class numbered_state_heap_ssp_const_iterator :
    public numbered_state_heap_const_iterator
  {
  public:
    numbered_state_heap_ssp_const_iterator
      (const numbered_state_heap_ssp_semi::hash_type& h)
	: numbered_state_heap_const_iterator(), h(h)
    {
    }

    ~numbered_state_heap_ssp_const_iterator()
    {
    }

    virtual void
    first()
    {
      i = h.begin();
    }

    virtual void
    next()
    {
      ++i;
    }

    virtual bool
    done() const
    {
      return i == h.end();
    }

    virtual const state*
    get_state() const
    {
      return i->first;
    }

    virtual int
    get_index() const
    {
      return i->second;
    }

  private:
    numbered_state_heap_ssp_semi::hash_type::const_iterator i;
    const numbered_state_heap_ssp_semi::hash_type& h;
  };


  numbered_state_heap_const_iterator*
  numbered_state_heap_ssp_semi::iterator() const
  {
    return new numbered_state_heap_ssp_const_iterator(h);
  }


  /// \brief Factory for numbered_state_heap_ssp_semi
  ///
  /// This class is a singleton.  Retrieve the instance using instance().
  class numbered_state_heap_ssp_factory_semi:
    public numbered_state_heap_factory
  {
  public:
    virtual numbered_state_heap_ssp_semi*
    build() const
    {
      return new numbered_state_heap_ssp_semi();
    }

    /// Get the unique instance of this class.
    static const numbered_state_heap_ssp_factory_semi*
    instance()
    {
      static numbered_state_heap_ssp_factory_semi f;
      return &f;
    }

  protected:
    virtual
    ~numbered_state_heap_ssp_factory_semi()
    {
    }

    numbered_state_heap_ssp_factory_semi()
    {
    }
  };


  class emptiness_check_shy_ssp : public emptiness_check_shy
  {
  public:
    emptiness_check_shy_ssp(const tgba* a)
      : emptiness_check_shy(a,
			    numbered_state_heap_ssp_factory_semi::instance())
    {
    }

  protected:
    virtual int*
    find_state(const state* s)
    {
      typedef numbered_state_heap_ssp_semi::hash_type hash_type;
      hash_type& h = dynamic_cast<numbered_state_heap_ssp_semi*>(ecs_->h)->h;

      hash_type::iterator i;
      for (i = h.begin(); i != h.end(); ++i)
	{
	  const state_gspn_ssp* old_state =
	    dynamic_cast<const state_gspn_ssp*>(i->first);
	  const state_gspn_ssp* new_state =
	    dynamic_cast<const state_gspn_ssp*>(s);
	  assert(old_state);
	  assert(new_state);

	  if ((old_state->right())->compare(new_state->right()) == 0)
	    {
	      if (old_state->left() == new_state->left())
		break;

	      if (old_state->left() && new_state->left())
		{
		  if (i->second == -1)
		    {
		      if (spot_inclusion(new_state->left(), old_state->left()))
			break;
		    }
		  else
		    {
		      if (spot_inclusion(old_state->left(), new_state->left()))
			{
			  State* succ_tgba_ = NULL;
			  size_t size_tgba_ = 0;
			  succ_queue& queue = todo.top().second;

			  Diff_succ(old_state->left(), new_state->left(),
				    &succ_tgba_, &size_tgba_);

			  for (size_t i = 0; i < size_tgba_; i++)
			    {
			      state_gspn_ssp* s =
				new state_gspn_ssp
				  (succ_tgba_[i],
				   old_state->right()->clone());
			      queue.push_back(successor(queue.begin()->acc, s));
			    }
			  if (size_tgba_ != 0)
			    diff_succ_free(succ_tgba_);
			  break;
			}
		    }
		}
	    }
	}
      if (i == h.end())
	return 0;
      return &i->second;
    }
  };



  emptiness_check*
  emptiness_check_ssp_semi(const tgba* ssp_automata)
  {
    assert(dynamic_cast<const tgba_gspn_ssp*>(ssp_automata));
    return
      new emptiness_check(ssp_automata,
			  numbered_state_heap_ssp_factory_semi::instance());
  }

  emptiness_check*
  emptiness_check_ssp_shy_semi(const tgba* ssp_automata)
  {
    assert(dynamic_cast<const tgba_gspn_ssp*>(ssp_automata));
    return
      new emptiness_check_shy
        (ssp_automata,
	 numbered_state_heap_ssp_factory_semi::instance());
  }

  emptiness_check*
  emptiness_check_ssp_shy(const tgba* ssp_automata)
  {
    assert(dynamic_cast<const tgba_gspn_ssp*>(ssp_automata));
    return new emptiness_check_shy_ssp(ssp_automata);
  }

  counter_example*
  counter_example_ssp(const emptiness_check_status* status)
  {
    return new counter_example(status,
			       connected_component_ssp_factory::instance());
  }
}