/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractAutomaton class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_AUTOMATON_HH_
#define _ABSTRACT_AUTOMATON_HH_

// insert class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol
	>
	class AbstractAutomaton;
}

/**
 * @brief   Abstract class for automaton operations
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is an abstract class defining shared interface for various kinds of
 * automata.
 *
 * @tparam  State   Data type denoting automaton state.
 * @tparam  Symbol  Data type for automaton symbol.
 */
template
<
	typename State,
	typename Symbol
>
class SFTA::AbstractAutomaton
{
public:   // Public data types

	/**
	 * @brief  State
	 *
	 * Data type for state.
	 */
	typedef State StateType;


	/**
	 * @brief  Symbol
	 *
	 * Data type for symbol.
	 */
	typedef Symbol SymbolType;


	typedef typename SFTA::AbstractAutomaton
		<
			StateType,
			SymbolType
		> HierarchyRoot;

public:   // Public methods


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomaton()
	{ }
};

#endif
