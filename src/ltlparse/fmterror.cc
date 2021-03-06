// Copyright (C) 2010 Laboratoire de Recherche et Développement de
// l'Epita (LRDE).
// Copyright (C) 2003, 2004, 2005 Laboratoire d'Informatique de Paris
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

#include "public.hh"
#include <ostream>

namespace spot
{
  namespace ltl
  {

    bool
    format_parse_errors(std::ostream& os,
			const std::string& ltl_string,
			parse_error_list& error_list)
    {
      bool printed = false;
      spot::ltl::parse_error_list::iterator it;
      for (it = error_list.begin(); it != error_list.end(); ++it)
	{
	  os << ">>> " << ltl_string << std::endl;
	  ltlyy::location& l = it->first;

	  unsigned n = 1;
	  for (; n < 4 + l.begin.column; ++n)
	    os << ' ';
	  // Write at least one '^', even if begin==end.
	  os << '^';
	  ++n;
	  for (; n < 4 + l.end.column; ++n)
	    os << '^';
	  os << std::endl << it->second << std::endl << std::endl;
	  printed = true;
	}
      return printed;
    }

  }
}
