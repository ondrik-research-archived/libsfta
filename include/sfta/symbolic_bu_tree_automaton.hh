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

// insert the class into proper namespace
namespace SFTA { class SymbolicBUTreeAutomaton; }

// forward declaration
namespace SFTA
{
	//class AbstractBUTreeAutomatonOperation
	class SymbolicBUTreeAutomatonOperation;//: public AbstractBUTreeAutomatonOperation;
}


/**
 * @brief   Symbolically represented bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Bottom-up tree automaton with symbolically represented transition function.
 */
class SFTA::SymbolicBUTreeAutomaton
	: public SFTA::AbstractBUTreeAutomaton
{
private:  // Private data types

	typedef SFTA::SymbolicBUTreeAutomatonOperation SymbolicBUTreeAutomatonOperation;

private:  // Private data members

public:   // Public methods
};

#endif
