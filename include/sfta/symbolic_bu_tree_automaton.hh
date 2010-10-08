/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with SymbolicBUTreeAutomaton class.
 *
 *****************************************************************************/

#ifndef _SYMBOLIC_BU_TREE_AUTOMATON_HH_
#define _SYMBOLIC_BU_TREE_AUTOMATON_HH_

// SFTA headers
#include <sfta/abstract_bu_tree_automaton.hh>
#include <sfta/abstract_shared_mtbdd.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class MTBDDTransitionFunction,
		typename State,
		typename Symbol,
		template <typename> class LeftHandSide,
		template <typename> class InputRightHandSide,
		template <typename> class OutputRightHandSide
	>
	class SymbolicBUTreeAutomaton;
}


/**
 * @brief   Symbolically represented bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Bottom-up tree automaton with symbolically represented transition function.
 */
template
<
	class MTBDDTransitionFunction,
	typename State,
	typename Symbol,
	template <typename> class LeftHandSide,
	template <typename> class InputRightHandSide,
	template <typename> class OutputRightHandSide = InputRightHandSide
>
class SFTA::SymbolicBUTreeAutomaton
	: public SFTA::AbstractBUTreeAutomaton
		<
			State,
			Symbol,
			LeftHandSide,
			InputRightHandSide,
			OutputRightHandSide
		>
{

public:   // Public data types

	typedef State StateType;
	typedef Symbol SymbolType;

	typedef SFTA::AbstractBUTreeAutomaton
		<
			StateType,
			SymbolType,
			LeftHandSide,
			InputRightHandSide,
			OutputRightHandSide
		> ParentClass;


	class Operation
		: public ParentClass::Operation
	{
	};

private:  // Private data types

	//mapovani Symbol -> MTBDDTransitionFunction::Symbol
	//mapovani State -> MTBDDTransitionFunction::State

private:  // Private data members

	MTBDDTransitionTableWrapper* ttWrapper_;

protected:// Protected methods


	virtual MTBDDTransitionTableWrapper* getTTWrapper() const
	{
		// Assertions
		assert(ttWrapper_ != static_cast<MTBDDTransitionTableWrapper*>(0));

		return ttWrapper_;
	}


	virtual Operation* CreateOperation() const = 0;

public:   // Public methods

	/**
	 * @brief  Default constructor
	 *
	 * Default constructor.
	 */
	SymbolicBUTreeAutomaton()
		: ttWrapper_(0)
	{
		// Assertions
		assert(ttWrapper_ != static_cast<MTBDDTransitionTableWrapper*>(0));
	}

	/**
	 * @brief  Copy constructor
	 *
	 * Copy constructor of the class.
	 *
	 * @param[in]  aut  The automaton to be copied
	 */
	SymbolicBUTreeAutomaton(const SymbolicBUTreeAutomaton& aut)
		: ttWrapper_(aut.getTTWrapper())
	{
		// TODO @todo
		assert(false);
		assert(&aut != 0);
	}

};

#endif
