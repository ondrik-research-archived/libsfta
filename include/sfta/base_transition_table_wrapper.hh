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

	StateType CreateState()
	{
		return nextState_++;
	}

	virtual ~BaseTransitionTableWrapper()
	{ }
};

#endif
