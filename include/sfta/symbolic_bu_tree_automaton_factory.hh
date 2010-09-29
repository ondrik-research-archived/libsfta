/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for SymbolicBUTreeAutomatonFactory class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_BU_TREE_AUTOMATON_FACTORY_HH_
#define _SYMBOLIC_BU_TREE_AUTOMATON_FACTORY_HH_

// SFTA headers
#include <sfta/abstract_bu_tree_automaton_factory.hh>
#include <sfta/symbolic_bu_tree_automaton.hh>
#include <sfta/symbolic_bu_tree_automaton_operation.hh>

namespace SFTA { class SymbolicBUTreeAutomatonFactory; }

class SFTA::SymbolicBUTreeAutomatonFactory
	: public SFTA::AbstractBUTreeAutomatonFactory
{
public:

	virtual SymbolicBUTreeAutomaton* CreateAutomaton() const
	{
		return new SymbolicBUTreeAutomaton();
	}

	virtual SymbolicBUTreeAutomatonOperation* CreateAutomatonOperation() const
	{
		return new SymbolicBUTreeAutomatonOperation();
	}
};

#endif
