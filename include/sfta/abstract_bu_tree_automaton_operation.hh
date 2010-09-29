/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractBUTreeAutomatonOperation class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_BU_TREE_AUTOMATON_OPERATION_HH_
#define _ABSTRACT_BU_TREE_AUTOMATON_OPERATION_HH_

// SFTA headers
#include <sfta/abstract_automaton_operation.hh>

// insert the class into proper namespace
namespace SFTA { class AbstractBUTreeAutomatonOperation; }

class SFTA::AbstractBUTreeAutomatonOperation
	: public SFTA::AbstractAutomatonOperation
{
public:

	virtual SFTA::AbstractBUTreeAutomaton* Union(const SFTA::AbstractAutomaton* a1,
		const SFTA::AbstractAutomaton* a2) const = 0;

};

#endif
