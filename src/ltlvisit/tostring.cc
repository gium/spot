// Copyright (C) 2008, 2010 Laboratoire de Recherche et Développement
// de l'Epita (LRDE).
// Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
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
// or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU General Public
// License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Spot; see the file COPYING.  If not, write to the Free
// Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
// 02111-1307, USA.

#include <cassert>
#include <sstream>
#include <ctype.h>
#include <ostream>
#include <cstring>
#include "tostring.hh"
#include "ltlast/visitor.hh"
#include "ltlast/allnodes.hh"


namespace spot
{
  namespace ltl
  {
    namespace
    {
      static bool
      is_bare_word(const char* str)
      {
	// Bare words cannot be empty, start with the letter of a unary
	// operator, or be the name of an existing constant.  Also they
	// should start with an letter.
	if (!*str
	    || *str == 'F'
	    || *str == 'G'
	    || *str == 'X'
	    || !(isalpha(*str) || *str == '_')
	    || !strcasecmp(str, "true")
	    || !strcasecmp(str, "false"))
	  return false;
	// The remaining of the word must be alphanumeric.
	while (*++str)
	  if (!(isalnum(*str) || *str == '_'))
	    return false;
	return true;
      }

      class to_string_visitor: public const_visitor
      {
      public:
	to_string_visitor(std::ostream& os, bool full_parent = false)
	  : os_(os), top_level_(true), full_parent_(full_parent)
	{
	}

	virtual
	~to_string_visitor()
	{
	}

	void
	visit(const atomic_prop* ap)
	{
	  std::string str = ap->name();
	  if (full_parent_)
	    os_ << "(";
	  if (!is_bare_word(str.c_str()))
	    {
	      os_ << '"' << str << '"';
	    }
	  else
	    {
	      os_ << str;
	    }
	  if (full_parent_)
	    os_ << ")";
	}

	void
	visit(const constant* c)
	{
	  if (full_parent_)
	    os_ << "(";
	  os_ << c->val_name();
	  if (full_parent_)
	    os_ << ")";
	}

	void
	visit(const binop* bo)
	{
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";

	  bo->first()->accept(*this);

	  switch (bo->op())
	    {
	    case binop::Xor:
	      os_ << " ^ ";
	      break;
	    case binop::Implies:
	      os_ << " => ";
	      break;
	    case binop::Equiv:
	      os_ << " <=> ";
	      break;
	    case binop::U:
	      os_ << " U ";
	      break;
	    case binop::R:
	      os_ << " R ";
	      break;
	    case binop::W:
	      os_ << " W ";
	      break;
	    case binop::M:
	      os_ << " M ";
	      break;
	    }

	  bo->second()->accept(*this);
	  if (!top_level)
	    os_ << ")";
	}

	void
	visit(const unop* uo)
	{
	  // The parser treats F0, F1, G0, G1, X0, and X1 as atomic
	  // propositions.  So make sure we output F(0), G(1), etc.
	  bool need_parent = !!dynamic_cast<const constant*>(uo->child());
	  bool top_level = top_level_;

	  if (full_parent_)
	    {
	      need_parent = false; // These will be printed by each subformula

	      if (!top_level)
		os_ << "(";
	    }

	  switch (uo->op())
	    {
	    case unop::Not:
	      os_ << "!";
	      need_parent = false;
	      break;
	    case unop::X:
	      os_ << "X";
	      break;
	    case unop::F:
	      os_ << "F";
	      break;
	    case unop::G:
	      os_ << "G";
	      break;
	    case unop::Finish:
	      os_ << "finish";
	      need_parent = true;
	      break;
	    }

	  top_level_ = false;
	  if (need_parent)
	    os_ << "(";
	  uo->child()->accept(*this);
	  if (need_parent)
	    os_ << ")";

	  if (full_parent_ && !top_level)
	    os_ << ")";
	}

	void
	visit(const automatop* ao)
	{
	  // Warning: this string isn't parsable because the automaton
	  // operators used may not be defined.
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";
	  os_ << ao->get_nfa()->get_name() << "(";
	  unsigned max = ao->size();
	  ao->nth(0)->accept(*this);
	  for (unsigned n = 1; n < max; ++n)
	    {
	      os_ << ",";
	      ao->nth(n)->accept(*this);
	    }
	  os_ << ")";
	  if (!top_level)
	    os_ << ")";
	}

	void
	visit(const multop* mo)
	{
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";
	  unsigned max = mo->size();
	  mo->nth(0)->accept(*this);
	  const char* ch = " ";
	  switch (mo->op())
	    {
	    case multop::Or:
	      ch = " | ";
	      break;
	    case multop::And:
	      ch = " & ";
	      break;
	    }

	  for (unsigned n = 1; n < max; ++n)
	    {
	      os_ << ch;
	      mo->nth(n)->accept(*this);
	    }
	  if (!top_level)
	    os_ << ")";
	}
      protected:
	std::ostream& os_;
	bool top_level_;
	bool full_parent_;
      };

      class to_spin_string_visitor : public to_string_visitor
      {
      public:
	to_spin_string_visitor(std::ostream& os, bool full_parent = false)
	  : to_string_visitor(os, full_parent)
	{
	}

	virtual
	~to_spin_string_visitor()
	{
	}

	void
	visit(const binop* bo)
	{
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";

	  switch (bo->op())
	    {
	    case binop::Xor:
	      os_ << "(!";
	      bo->first()->accept(*this);
	      os_ << " && ";
	      bo->second()->accept(*this);
	      os_ << ") || (";
	      bo->first()->accept(*this);
	      os_ << " && !";
	      bo->second()->accept(*this);
	      os_ << ")";
	      break;
	    case binop::Implies:
	      os_ << "!";
	      bo->first()->accept(*this);
	      os_ << " || ";
	      bo->second()->accept(*this);
	      break;
	    case binop::Equiv:
	      os_ << "(";
	      bo->first()->accept(*this);
	      os_ << " && ";
	      bo->second()->accept(*this);
	      os_ << ") || (!";
	      bo->first()->accept(*this);
	      os_ << " && !";
	      bo->second()->accept(*this);
	      os_ << ")";
	    case binop::U:
	      bo->first()->accept(*this);
	      os_ << " U ";
	      bo->second()->accept(*this);
	      break;
	    case binop::R:
	      bo->first()->accept(*this);
	     os_ << " V ";
	      bo->second()->accept(*this);
	      break;
	      /* W and M are not supported by Spin */
	    case binop::W:
	      bo->first()->accept(*this);
	      os_ << " W ";
	      bo->second()->accept(*this);
	      break;
	    case binop::M:
	      bo->first()->accept(*this);
	     os_ << " M ";
	      bo->second()->accept(*this);
	      break;
	    }

	  if (!top_level)
	    os_ << ")";
	}

	void
	visit(const unop* uo)
	{
	  bool top_level = top_level_;
	  if (full_parent_ && !top_level)
	    os_ << "(";
	  bool need_parent = false;
	  switch (uo->op())
	    {
	    case unop::Not:
	      os_ << "!";
	      break;
	    case unop::X:
	      // The parser treats X0, and X1 as atomic propositions.	 So
	      // make sure we output X(0) and X(1).
	      need_parent = !!dynamic_cast<const constant*>(uo->child());
	      if (full_parent_)
		need_parent = false;
	      os_ << "X";
	      break;
	    case unop::F:
	      os_ << "<>";
	      break;
	    case unop::G:
	      os_ << "[]";
	      break;
	    case unop::Finish:
	      os_ << "finish";
	      need_parent = true;
	      break;
	    }

	  top_level_ = false;
	  if (need_parent)
	    os_ << "(";
	  uo->child()->accept(*this);
	  if (need_parent)
	    os_ << ")";
	  if (full_parent_ && !top_level)
	    os_ << ")";
	}

	void
	visit(const automatop* ao)
	{
	  // Warning: this string isn't parsable because the automaton
	  // operators used may not be defined.
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";
	  os_ << ao->get_nfa()->get_name() << "(";
	  unsigned max = ao->size();
	  ao->nth(0)->accept(*this);
	  for (unsigned n = 1; n < max; ++n)
	    {
	      os_ << ",";
	      ao->nth(n)->accept(*this);
	    }
	  os_ << ")";
	  if (!top_level)
	    os_ << ")";
	}

	void
	visit(const multop* mo)
	{
	  bool top_level = top_level_;
	  top_level_ = false;
	  if (!top_level)
	    os_ << "(";
	  unsigned max = mo->size();
	  mo->nth(0)->accept(*this);
	  const char* ch = " ";
	  switch (mo->op())
	    {
	    case multop::Or:
	      ch = " || ";
	      break;
	    case multop::And:
	      ch = " && ";
	      break;
	    }

	  for (unsigned n = 1; n < max; ++n)
	    {
	      os_ << ch;
	      mo->nth(n)->accept(*this);
	    }
	  if (!top_level)
	    os_ << ")";
	}
      };

    } // anonymous

    std::ostream&
    to_string(const formula* f, std::ostream& os, bool full_parent)
    {
      to_string_visitor v(os, full_parent);
      f->accept(v);
      return os;
    }

    std::string
    to_string(const formula* f, bool full_parent)
    {
      std::ostringstream os;
      to_string(f, os, full_parent);
      return os.str();
    }

    std::ostream&
    to_spin_string(const formula* f, std::ostream& os, bool full_parent)
    {
      to_spin_string_visitor v(os, full_parent);
      f->accept(v);
      return os;
    }

    std::string
    to_spin_string(const formula* f, bool full_parent)
    {
      std::ostringstream os;
      to_spin_string(f, os, full_parent);
      return os.str();
    }
  }
}
