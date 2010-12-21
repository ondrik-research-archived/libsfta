/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for BaseTransitionTableWrapper class.
 *
 *****************************************************************************/

#ifndef _BASE_TRANSITION_TABLE_WRAPPER_HH_
#define _BASE_TRANSITION_TABLE_WRAPPER_HH_

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State
	>
	class BaseTransitionTableWrapper;
}


/**
 * @brief   Base class for transition table wrapper
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class serves as the base class for wrappers of transition tables. 
 *
 * @tparam  State   Data type used for representation of states.
 */
template
<
	typename State
>
class SFTA::BaseTransitionTableWrapper
{
public:   // public data types

	typedef State StateType;

private:  // private data members

	StateType nextState_;


public:   // public methods

	BaseTransitionTableWrapper()
		: nextState_()
	{ }

	StateType CreateState()
	{
		return nextState_++;
	}

	virtual ~BaseTransitionTableWrapper()
	{ }
};

#endif
