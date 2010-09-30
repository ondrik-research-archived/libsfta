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

namespace SFTA
{
	template
	<
		class SymbolicBUTreeAutomaton
	>
	class SymbolicBUTreeAutomatonFactory;
}


template
<
	class SymbolicBUTreeAutomaton
>
class SFTA::SymbolicBUTreeAutomatonFactory
	: public SFTA::AbstractBUTreeAutomatonFactory
		<
			typename SymbolicBUTreeAutomaton::ParentClass
		>
{
public:

	typedef SymbolicBUTreeAutomaton SymbolicBUTreeAutomatonType;

	typedef SymbolicBUTreeAutomatonOperation
		<
			SymbolicBUTreeAutomatonType
		>
		SymbolicBUTreeAutomatonOperationType;


public:

	virtual SymbolicBUTreeAutomatonType* CreateAutomaton() const
	{
		return new SymbolicBUTreeAutomatonType();
	}

	virtual SymbolicBUTreeAutomatonOperationType* CreateAutomatonOperation() const
	{
		return new SymbolicBUTreeAutomatonOperationType();
	}
};

#endif
