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

// SFTA headers
#include <sfta/base_transition_table_wrapper.hh>
#include <sfta/convert.hh>

// Standard library headers
#include <cassert>


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
		> Type;


	typedef Type HierarchyRoot;

	typedef typename SFTA::BaseTransitionTableWrapper
		<
			StateType
		> BaseTTWrapperType;

	/**
	 * @brief   Abstract class for automata operations
	 *
	 * This class represents an abstract interface to operations that are used by
	 * all automata, such as language union, intersection and so on.
	 */
	class Operation
	{
	public:   // Public methods


		/**
		 * @brief  Union of two automata
		 *
		 * This method returns an automaton that accepts a language that is the
		 * union of languages of the input automata.
		 *
		 * @param[in]  a1  First input automaton
		 * @param[in]  a2  Second input automaton
		 *
		 * @returns  Union automaton
		 */
		virtual Type* Union(const Type* a1, const Type* a2) const = 0;


		/**
		 * @brief  Destructor
		 *
		 * Virtual destructor.
		 */
		virtual ~Operation()
		{ }
	};


protected:// Protected methods

	/**
	 * @brief  Creates the operation object
	 *
	 * This method creates proper operation.
	 *
	 * @returns  Proper operation
	 */
	virtual Operation* createOperation() const = 0;

public:   // Public methods


	AbstractAutomaton()
	{ }


	AbstractAutomaton(const AbstractAutomaton& aut)
	{
		// Assertions
		assert(&aut != static_cast<AbstractAutomaton*>(0));
	}



	/**
	 * @brief  Gets operation
	 *
	 * This methods gets proper operation object of the automaton. This is
	 * a factory method.
	 *
	 * @returns  Operation object for given automaton
	 */
	Operation* GetOperation() const
	{
		return createOperation();
	}


	virtual void CopyStates(const AbstractAutomaton& aut) = 0;


	virtual StateType AddState() = 0;


	virtual std::string ToString() const = 0;

	/**
	 * @brief  Returns a transition table wrapper
	 *
	 * This method returns given transition table wrapper of the automaton.
	 *
	 * @returns  Transition table wrapper
	 */
	virtual BaseTTWrapperType* GetTTWrapper() const = 0;


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomaton()
	{ }
};

#endif
