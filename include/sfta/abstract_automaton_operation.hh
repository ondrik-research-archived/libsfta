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
namespace SFTA { class AbstractAutomatonOperation; }


/**
 * @brief   Abstract class for automata operations
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class represents an abstract interface to operations that are used by
 * all automata, such as language union, intersection and so on.
 */
class SFTA::AbstractAutomatonOperation
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
	virtual SFTA::AbstractAutomaton* Union(const SFTA::AbstractAutomaton* a1,
		const SFTA::AbstractAutomaton* a2) const = 0;


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomatonOperation()
	{ }
};

#endif
