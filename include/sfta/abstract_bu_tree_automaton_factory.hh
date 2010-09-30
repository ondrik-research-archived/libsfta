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
namespace SFTA
{
	template
	<
		class AbstractBUTreeAutomaton
	>
	class AbstractBUTreeAutomatonFactory;
}

/**
 * @brief  Abstract factory for bottom-up tree automata
 *
 * Abstract factory for bottom-up tree automata.
 *
 * @tparam  AbstractAutomaton  The abstract product of the factory.
 */
template
<
	class AbstractBUTreeAutomaton
>
class SFTA::AbstractBUTreeAutomatonFactory
	: public SFTA::AbstractAutomatonFactory
		<
			typename AbstractBUTreeAutomaton::ParentClass
		>
{
public:   // Public data types

	typedef AbstractBUTreeAutomaton AbstractBUTreeAutomatonType;

	typedef SFTA::AbstractBUTreeAutomatonOperation
		<
			AbstractBUTreeAutomatonType
		>
		AbstractBUTreeAutomatonOperation;


public:   // Public methods

	virtual AbstractBUTreeAutomatonType* CreateAutomaton() const = 0;
	virtual AbstractBUTreeAutomatonOperation* CreateAutomatonOperation() const = 0;
};

#endif
