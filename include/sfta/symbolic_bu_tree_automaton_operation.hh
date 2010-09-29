/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for SymbolicBUTreeAutomatonOperation class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_BU_TREE_AUTOMATON_OPERATION_HH_
#define _SYMBOLIC_BU_TREE_AUTOMATON_OPERATION_HH_

// SFTA headers
#include <sfta/abstract_bu_tree_automaton_operation.hh>

// insert the class into proper namespace
namespace SFTA { class SymbolicBUTreeAutomatonOperation; }

class SFTA::SymbolicBUTreeAutomatonOperation
	: public AbstractBUTreeAutomatonOperation
{
public:

	virtual SFTA::SymbolicBUTreeAutomaton* Union(const SFTA::AbstractAutomaton* a1,
		const SFTA::AbstractAutomaton* a2) const
	{
		assert(false);
		return new SymbolicBUTreeAutomaton();
	}
};

#endif
