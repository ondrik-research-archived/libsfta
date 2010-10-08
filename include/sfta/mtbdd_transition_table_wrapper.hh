/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for MTBDDTransitionTableWrapper class.
 *
 *****************************************************************************/

#ifndef _MTBDD_TRANSITION_TABLE_WRAPPER_HH_
#define _MTBDD_TRANSITION_TABLE_WRAPPER_HH_

// SFTA header
#include <sfta/base_transition_table_wrapper.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename State,
		class AbstractSharedMTBDD
	>
	class MTBDDTransitionTableWrapper;
}


template
<
	typename State,
	class AbstractSharedMTBDD
>
class SFTA::MTBDDTransitionTableWrapper
	: public SFTA::BaseTransitionTableWrapper
		<
			State
		>
{
public:   // Public data types

	typedef State StateType;

	typedef AbstractSharedMTBDD AbstractSharedMTBDDType;

	// TODO @todo: static check of some types
	//LOKI_STATIC_CHECK();

private:  // Private data members

	AbstractSharedMTBDDType* mtbdd_;

public:   // Public methods

	inline AbstractSharedMTBDDType* GetMTBDD() const
	{
		// Assertions
		assert(mtbdd_ != static_cast<AbstractSharedMTBDDType*>(0));

		return mtbdd_;
	}

	virtual ~MTBDDTransitionTableWrapper()
	{
		// Assertions
		assert(mtbdd_ != static_cast<AbstractSharedMTBDDType*>(0));

		delete mtbdd_;
	}
};

#endif
