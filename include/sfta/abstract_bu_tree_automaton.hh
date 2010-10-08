/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractBUTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_BU_TREE_AUTOMATON_HH_
#define _ABSTRACT_BU_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_automaton.hh>
#include <sfta/abstract_bu_tree_automaton_transition_function.hh>
#include <sfta/vector.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide
	>
	class AbstractBUTreeAutomaton;
}


/**
 * @brief   Abstract bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class defining interface to bottom-up tree automata.
 */
template
<
	typename State,
	typename Symbol,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide = InputRightHandSide
>
class SFTA::AbstractBUTreeAutomaton
	: public SFTA::AbstractAutomaton
		<
			State,
			Symbol
		>
{
public:   // Public data types

	typedef SFTA::AbstractAutomaton
		<
			State,
			Symbol
		> ParentClass;

	typedef AbstractBUTreeAutomaton
		<
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::SymbolType SymbolType;

	typedef typename SFTA::Vector<StateType> LeftHandSideType;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	class Operation
		: public ParentClass::Operation
	{
	};

private:  // Private data types


	/**
	 * @brief  The transition function
	 *
	 * The data type for the transition function.
	 */
	typedef SFTA::AbstractBUTreeAutomatonTransitionFunction
		<
			// TODO change
			unsigned,
			// TODO change
			unsigned,
			InputRightHandSide,
			OutputRightHandSide
		>
		AbstractBUTreeAutomatonTransitionFunction;


protected:// Protected data members

	//AbstractBUTreeAutomatonTransitionFunction* transFunc_;


protected:// Protected methods

	virtual Operation* CreateOperation() const = 0;


};

#endif
