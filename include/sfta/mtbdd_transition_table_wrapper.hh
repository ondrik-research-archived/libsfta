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
		class SharedMTBDD
	>
	class MTBDDTransitionTableWrapper;
}


/**
 * @brief   Wrapper for MTBDD transition function
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is a class that encapsulates a transition function represented using
 * a MTBDD.
 *
 * @tparam  State        Type of state of the transition function.
 * @tparam  SharedMTBDD  Type of shared MTBDD that is used to represent the
 *                       transition function.
 */
template
<
	typename State,
	class SharedMTBDD
>
class SFTA::MTBDDTransitionTableWrapper
	: public SFTA::BaseTransitionTableWrapper
		<
			State
		>
{
public:   // Public data types

	typedef MTBDDTransitionTableWrapper
		<
			State,
			SharedMTBDD
		> Type;

	typedef typename SFTA::BaseTransitionTableWrapper
		<
			State
		> ParentClass;

	typedef State StateType;

	typedef SharedMTBDD SharedMTBDDType;


private:  // Private data members

	SharedMTBDDType* mtbdd_;

private:  // Private methods

	MTBDDTransitionTableWrapper(const MTBDDTransitionTableWrapper& wrap);
	MTBDDTransitionTableWrapper& operator=(const MTBDDTransitionTableWrapper& wrap);


public:   // Public methods

	MTBDDTransitionTableWrapper()
		: mtbdd_(new SharedMTBDDType())
	{
		// Assertions
		assert(mtbdd_ != static_cast<SharedMTBDDType*>(0));
	}

	inline SharedMTBDDType* GetMTBDD() const
	{
		// Assertions
		assert(mtbdd_ != static_cast<SharedMTBDDType*>(0));

		return mtbdd_;
	}

	virtual ~MTBDDTransitionTableWrapper()
	{
		// Assertions
		assert(mtbdd_ != static_cast<SharedMTBDDType*>(0));

		delete mtbdd_;
	}
};

#endif
