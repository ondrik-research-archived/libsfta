/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the AbstractAutomatonTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_AUTOMATON_TRANSITION_FUNCTION_HH_
#define _SFTA_ABSTRACT_AUTOMATON_TRANSITION_FUNCTION_HH_


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol
	>
	class AbstractAutomatonTransitionFunction;
}


/**
 * @brief   Abstract class for a transition function
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that represents the interface used by transition functions
 * of tree automata.
 *
 * @tparam  State   The data type for a state.
 * @tparam  Symbol  The data type for a symbol.
 */
template
<
	typename State,
	typename Symbol
>
class SFTA::AbstractAutomatonTransitionFunction
{
public:   // Public data types

	/**
	 * @brief  State data type
	 *
	 * The data type of a state.
	 */
	typedef State StateType;

	/**
	 * @brief  Symbol data type
	 *
	 * The data type of a symbol.
	 */
	typedef Symbol SymbolType;

public:   // Public methods


	/**
	 * @brief  Creates a new state
	 *
	 * This method creates a new state in the transition table. A unique state
	 * is returned.
	 *
	 * @returns  A new state
	 */
	virtual StateType CreateState() = 0;


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomatonTransitionFunction()
	{ }

};


#endif
