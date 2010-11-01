/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for BUTreeAutomatonCover class.
 *
 *****************************************************************************/

#ifndef _BU_TREE_AUTOMATON_COVER_HH_
#define _BU_TREE_AUTOMATON_COVER_HH_

// Standard library headers
#include <string>


// SFTA header files
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/mtbdd_transition_table_wrapper.hh>
#include <sfta/nd_symbolic_bu_tree_automaton.hh>
#include <sfta/set.hh>
#include <sfta/vector.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		size_t SymbolLength = 64
	>
	class BUTreeAutomatonCover;
}


/**
 * @brief   Cover class for bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Class that provides nice interface to nondeterministic bottom-up finite
 * tree automaton.
 */
template
<
	size_t SymbolLength
>
class SFTA::BUTreeAutomatonCover
{
public:   // Public data types

	typedef std::string StateType;
	typedef std::string SymbolType;

	typedef SFTA::Vector<StateType> LeftHandSideType;
	typedef SFTA::Set<StateType> RightHandSideType;

private:  // Private data types

	typedef unsigned InternalStateType;
	typedef SFTA::Private::CompactVariableAssignment<SymbolLength>
		InternalSymbolType;
	typedef SFTA::Vector<InternalStateType> InternalLeftHandSideType;
	typedef SFTA::OrderedVector<InternalStateType> InternalRightHandSideType;
	typedef unsigned MTBDDRootType;


	typedef SFTA::CUDDSharedMTBDD
	<
			MTBDDRootType,
			InternalRightHandSideType,
			InternalSymbolType,
			SFTA::Private::DualMapLeafAllocator,
			SFTA::Private::MapRootAllocator
	> SharedMTBDD;
	

	typedef SFTA::MTBDDTransitionTableWrapper
	<
		InternalStateType,
		SharedMTBDD
	> MTBDDTransitionTableWrapper;

	typedef SFTA::NDSymbolicBUTreeAutomaton
	<
		MTBDDTransitionTableWrapper,
		InternalStateType,
		InternalSymbolType,
		SFTA::OrderedVector
	> NDSymbolicBUTreeAutomaton;

	typedef typename NDSymbolicBUTreeAutomaton::TTWrapperPtrType TTWrapperPtr;


private:  // Private data members

	NDSymbolicBUTreeAutomaton automaton_;


public:   // Public methods

	BUTreeAutomatonCover()
		: automaton_()
	{ }

	explicit BUTreeAutomatonCover(TTWrapperPtr wrapper)
		: automaton_(wrapper)
	{ }

	void AddState(const StateType& state)
	{
//		if (state2internalState.insert(std::make_pair(state, nextStateNum_))
//		++nextStateNum_;
		assert(false);
	}

	void AddSymbol(const SymbolType& symbol)
	{
		assert(false);
	}

	void AddTransition(const LeftHandSideType& lhs, const SymbolType& symbol,
		const RightHandSideType& rhs)
	{
		assert(false);
	}

	void SetStateFinal(const StateType& state)
	{
		assert(false);
	}

	inline TTWrapperPtr GetTTWrapper()
	{
		return automaton_.GetTTWrapper();
	}


	std::string ToString() const
	{
		std::string result;

		assert(false);

		return result;
	}
};
#endif
