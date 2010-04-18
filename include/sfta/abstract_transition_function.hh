/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the AbstractTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_TRANSITION_FUNCTION_HH_
#define _SFTA_ABSTRACT_TRANSITION_FUNCTION_HH_

// Standard library header files
#include <list>

// Loki header files
#include <loki/HierarchyGenerators.h>
#include <loki/TypelistMacros.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename State,
		template <typename> class LeftHandSide,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide
	>
	class AbstractTransitionFunction;
}


/**
 * @brief   Abstract class for a transition function
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class that represents the interface used by transition functions
 * of tree automata.
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
	typename Symbol,
	typename State,
	template <typename> class LeftHandSide,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide

>
class SFTA::AbstractTransitionFunction
{
public:   // Public data types

	typedef Symbol SymbolType;
	typedef State StateType;
	typedef LeftHandSide<StateType> LeftHandSideType;
	typedef InputRightHandSide<StateType> InputRightHandSideType;
	typedef OutputRightHandSide<StateType> OutputRightHandSideType;

	typedef Loki::Tuple<LOKI_TYPELIST_3(Symbol, LeftHandSideType,
		InputRightHandSideType)> TransitionType;
	typedef std::list<TransitionType> TransitionListType;


public:   // Public methods

	virtual void AddTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs, const InputRightHandSideType& rhs) = 0;


	virtual OutputRightHandSideType GetTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs) = 0;

	virtual TransitionListType GetListOfTransitions() = 0;

	virtual ~AbstractTransitionFunction()
	{ }


};


#endif
