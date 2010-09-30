/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with AbstractAutomatonOperation class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_AUTOMATON_OPERATION_HH_
#define _ABSTRACT_AUTOMATON_OPERATION_HH_


// SFTA headers
#include <sfta/abstract_automaton.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class AbstractAutomaton
	>
	class AbstractAutomatonOperation;
}


/**
 * @brief   Abstract class for automata operations
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class represents an abstract interface to operations that are used by
 * all automata, such as language union, intersection and so on.
 *
 * @tparam  AbstractAutomaton  The data type of the automaton upon which the
 *                             operations work.
 */
template
<
	class AbstractAutomaton
>
class SFTA::AbstractAutomatonOperation
{
public:   // Public data types


	/**
	 * @brief  Abstract automaton type
	 *
	 * The type of the abstract automaton.
	 */
	typedef AbstractAutomaton AbstractAutomatonType;


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
	virtual AbstractAutomatonType* Union(const AbstractAutomatonType* a1,
		const AbstractAutomatonType* a2) const = 0;


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomatonOperation()
	{ }
};

#endif
