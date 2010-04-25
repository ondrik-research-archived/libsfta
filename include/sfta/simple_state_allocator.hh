/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with SimpleStateAllocator class.
 *
 *****************************************************************************/

#ifndef _SFTA_SIMPLE_STATE_ALLOCATOR_HH_
#define _SFTA_SIMPLE_STATE_ALLOCATOR_HH_

// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			typename State
		>
		class SimpleStateAllocator;
	}
}


template
<
	typename State
>
class SFTA::Private::SimpleStateAllocator
{
public:   // Public data types

	typedef State StateType;

private:  // Private data members

	StateType nextState_;

protected:// Protected data members

	const static StateType SINK_STATE = 0;

public:   // Public methods

	SimpleStateAllocator()
		: nextState_()
	{ }

	inline StateType AllocateState()
	{
		StateType oldValue = nextState_;
		++nextState_;
		return oldValue;
	}

	inline size_t CountOfStates() const
	{
		return static_cast<size_t>(nextState_);
	}
};

#endif
