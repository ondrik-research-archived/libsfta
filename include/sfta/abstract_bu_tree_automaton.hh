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

// insert the class into proper namespace
namespace SFTA { class AbstractBUTreeAutomaton; }


/**
 * @brief   Abstract bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Abstract class defining interface to bottom-up tree automata.
 */
class SFTA::AbstractBUTreeAutomaton
	: public AbstractAutomaton
{
};

#endif
