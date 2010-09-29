/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractBUTreeAutomatonFactory class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_BU_TREE_AUTOMATON_FACTORY_HH_
#define _ABSTRACT_BU_TREE_AUTOMATON_FACTORY_HH_

// SFTA headers
#include <sfta/abstract_automaton_factory.hh>
#include <sfta/abstract_bu_tree_automaton.hh>
#include <sfta/abstract_bu_tree_automaton_operation.hh>

// insert the class into proper namespace
namespace SFTA { class AbstractBUTreeAutomatonFactory; }

class SFTA::AbstractBUTreeAutomatonFactory
	: public SFTA::AbstractAutomatonFactory
{
public:

	virtual AbstractBUTreeAutomaton* CreateAutomaton() const = 0;
	virtual AbstractBUTreeAutomatonOperation* CreateAutomatonOperation() const = 0;
};

#endif
