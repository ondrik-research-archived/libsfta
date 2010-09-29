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

// insert class into proper namespace
namespace SFTA { class AbstractAutomaton; }

// forward declarations
namespace SFTA { class AbstractAutomatonOperation; }

/**
 * @brief   Abstract class for automaton operations
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is an abstract class defining shared interface for various kinds of
 * automata.
 */
class SFTA::AbstractAutomaton
{
public:   // Public methods


	/**
	 * @brief  Destructor
	 *
	 * Virtual destructor.
	 */
	virtual ~AbstractAutomaton()
	{ }
};

#endif
