/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for AbstractTDTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_TD_TREE_AUTOMATON_HH_
#define _ABSTRACT_TD_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_automaton.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol,
		class InputLeftHandSide,
		class OutputLeftHandSide
	>
	class AbstractTDTreeAutomaton;
}


/**
 * @brief   Abstract top-down tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class defining interface to top-down tree automata.
 */
template
<
	typename State,
	typename Symbol,
	class InputRightHandSide,
	class OutputRightHandSide = InputRightHandSide
>
class SFTA::AbstractTDTreeAutomaton
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

	typedef AbstractTDTreeAutomaton
		<
			State,
			Symbol,
			InputRightHandSide,
			OutputRightHandSide
		> Type;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::SymbolType SymbolType;

	typedef StateType LeftHandSideType;

	typedef InputRightHandSide InputRightHandSideType;
	typedef OutputRightHandSide OutputRightHandSideType;

	typedef typename ParentClass::HierarchyRoot HierarchyRoot;

	/**
	 * @brief  @copybrief SFTA::AbstractAutomaton::Operation
	 *
	 * @copydetails SFTA::AbstractAutomaton::Operation
	 */
	class Operation
		: public ParentClass::Operation
	{
	};


protected:// Protected methods

	virtual Operation* createOperation() const = 0;


public:   // Public methods

	virtual void SetStateInitial(const StateType& state) = 0;

	virtual bool IsStateInitial(const StateType& state) const = 0;

	virtual std::vector<StateType> GetVectorOfInitialStates() const = 0;
};

#endif
