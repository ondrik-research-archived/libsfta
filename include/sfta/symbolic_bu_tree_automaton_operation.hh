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
namespace SFTA
{
	template
	<
		class SymbolicBUTreeAutomaton
	>
	class SymbolicBUTreeAutomatonOperation;
}


template
<
	class SymbolicBUTreeAutomaton
>
class SFTA::SymbolicBUTreeAutomatonOperation
	: public AbstractBUTreeAutomatonOperation
		<
			typename SymbolicBUTreeAutomaton::ParentClass
		>
{
public:   // Public data types


	typedef SymbolicBUTreeAutomaton SymbolicBUTreeAutomatonType;

	typedef typename SymbolicBUTreeAutomatonType::HierarchyRoot AbstractAutomatonType;


public:   // Public methods

	virtual SymbolicBUTreeAutomatonType* Union(const AbstractAutomatonType* a1,
		const AbstractAutomatonType* a2) const
	{
		// Assertions
		assert(a1 != static_cast<AbstractAutomatonType*>(0));
		assert(a2 != static_cast<AbstractAutomatonType*>(0));

		assert(false);
		return new SymbolicBUTreeAutomaton();
	}
};

#endif
