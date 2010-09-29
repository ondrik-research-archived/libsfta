/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with AbstractAutomatonFactory class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_AUTOMATON_FACTORY_HH_
#define _ABSTRACT_AUTOMATON_FACTORY_HH_

// SFTA headers
#include <sfta/abstract_automaton.hh>
#include <sfta/abstract_automaton_operation.hh>

// insert the class into proper namespace
namespace SFTA { class AbstractAutomatonFactory; }

class SFTA::AbstractAutomatonFactory
{
private:  // Private data types


	/**
	 * @brief  Abstract automaton type
	 *
	 * Data type of an abstract automaton.
	 */
	typedef SFTA::AbstractAutomaton AbstractAutomaton;


	/**
	 * @brief  Abstract automaton operation type
	 *
	 * Data type for an abstract automaton operation.
	 */
	typedef SFTA::AbstractAutomatonOperation AbstractAutomatonOperation;


public:


	/**
	 * @brief  Creates an automaton
	 *
	 * Creates proper automaton.
	 *
	 * @returns  An automaton
	 */
	virtual AbstractAutomaton* CreateAutomaton() const = 0;


	/**
	 * @brief  Creates an automaton operation
	 *
	 * Creates proper automaton operation.
	 *
	 * @returns  An automaton operation
	 */
	virtual AbstractAutomatonOperation* CreateAutomatonOperation() const = 0;


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomatonFactory()
	{ }
};

#endif
