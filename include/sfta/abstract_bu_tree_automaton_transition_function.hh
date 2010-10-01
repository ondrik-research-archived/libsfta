/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractBUTreeAutomatonTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_BU_TREE_AUTOMATON_TRANSITION_FUNCTION_HH_
#define _ABSTRACT_BU_TREE_AUTOMATON_TRANSITION_FUNCTION_HH_

#include <sfta/abstract_automaton_transition_function.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol,
		template <typename> class LeftHandSide,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide
	>
	class AbstractBUTreeAutomatonTransitionFunction;
}


/**
 * @brief   Abstract class for a bottom-up tree automaton transition function
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that represents the interface used by transition functions
 * of bottom-up tree automata.
 *
 * @tparam  Symbol               The data type for a symbol.
 * @tparam  State                The data type for a state.
 * @tparam  LeftHandSide         The data type for a left-hand side of
 *                               a transition rule.
 * @tparam  InputRightHandSide   The data type for a right-hand side of
 *                               a transition rule that is used for insertion
 *                               of the rule to the transition function table.
 * @tparam  OutputRightHandSide  The data type for a right-hand side of
 *                               a transition rule that is used for retrieval
 *                               of the rule from the transition function
 *                               table.
 */
template
<
	typename State,
	typename Symbol,
	template <typename> class LeftHandSide,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide = InputRightHandSide
>
class SFTA::AbstractBUTreeAutomatonTransitionFunction
	: SFTA::AbstractAutomatonTransitionFunction
		<
			State,
			Symbol
		>
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

	/**
	 * @brief  Left-hand side data type
	 *
	 * The data type of a transition left-hand side.
	 */
	typedef LeftHandSide<StateType> LeftHandSideType;

	/**
	 * @brief  Input right-hand side date type
	 *
	 * The data type of an input transition right-hand side.
	 */
	typedef InputRightHandSide<StateType> InputRightHandSideType;

	/**
	 * @brief  Output right-hand side date type
	 *
	 * The data type of an output transition right-hand side.
	 */
	typedef OutputRightHandSide<StateType> OutputRightHandSideType;

//	struct Transition
//	{
//		LeftHandSideType lhs;
//		Symbol symbol;
//		OutputRightHandSideType rhs;
//	};


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
	 * @brief  Adds a new transition
	 *
	 * This method is used to add a new transition from given state over
	 * a symbol to some state into the transition function table.
	 *
	 * @param[in]  lhs     The left-hand side of the transition
	 * @param[in]  symbol  The symbol of the transition
	 * @param[in]  rhs     The right-hand side of the transition
	 */
	virtual void AddTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol, const InputRightHandSideType& rhs) = 0;


//	virtual OutputRightHandSideType GetTransition(const SymbolType& symbol,
//		const LeftHandSideType& lhs) = 0;

	//virtual TransitionListType GetListOfTransitions() = 0;

};

#endif
