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
#include <sfta/vector.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		typename Symbol,
		class RightHandSide
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
	class RightHandSide
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
			RightHandSide
		> Type;

	typedef typename ParentClass::StateType StateType;
	typedef typename ParentClass::SymbolType SymbolType;

	typedef typename SFTA::Vector<StateType> LeftHandSideType;

	typedef RightHandSide RightHandSideType;

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

	AbstractBUTreeAutomaton()
	{ }

	AbstractBUTreeAutomaton(const AbstractBUTreeAutomaton& aut)
		: ParentClass(aut)
	{ }

	virtual void AddTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol, const RightHandSideType& rhs) = 0;

	virtual RightHandSideType GetTransition(const LeftHandSideType& lhs,
		const SymbolType& symbol) = 0;

	virtual void SetStateFinal(const StateType& state) = 0;

	virtual bool IsStateFinal(const StateType& state) const = 0;

	virtual std::vector<StateType> GetVectorOfFinalStates() const = 0;

};

#endif
