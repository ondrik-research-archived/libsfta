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
namespace SFTA
{
	template
	<
		class AbstractBUTreeAutomaton
	>
	class AbstractBUTreeAutomatonOperation;
}


template
<
	class AbstractBUTreeAutomaton
>
class SFTA::AbstractBUTreeAutomatonOperation
	: public SFTA::AbstractAutomatonOperation
		<
			typename AbstractBUTreeAutomaton::ParentClass
		>
{
public:   // Public data types

	typedef AbstractBUTreeAutomaton AbstractBUTreeAutomatonType;

	typedef typename AbstractBUTreeAutomatonType::HierarchyRoot AbstractAutomatonType;


public:   // Public methods

	virtual AbstractBUTreeAutomatonType* Union(const AbstractAutomatonType* a1,
		const AbstractAutomatonType* a2) const = 0;

};

#endif
