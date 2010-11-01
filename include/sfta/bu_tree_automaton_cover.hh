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


	typedef std::map<StateType, InternalStateType> StateToInternalStateMap;
	typedef std::map<InternalStateType, StateType> InternalStateToStateMap;

	typedef std::map<SymbolType, InternalSymbolType> SymbolToInternalSymbolMap;
	typedef std::map<InternalSymbolType, SymbolType> InternalSymbolToSymbolMap;

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<InternalStateType> InternalStateVector;

public:   // Public data types

	typedef typename NDSymbolicBUTreeAutomaton::TTWrapperPtrType TTWrapperPtr;


private:  // Private data members

	NDSymbolicBUTreeAutomaton automaton_;

	StateToInternalStateMap state2internalStateMap_;
	InternalStateToStateMap internalState2stateMap_;

	SymbolToInternalSymbolMap symbol2internalSymbolMap_;
	InternalSymbolToSymbolMap internalSymbol2symbolMap_;

	InternalSymbolType nextSymbol_;

private:  // Private methods

	std::string statesToString(const InternalStateVector& vec) const
	{
		std::string result;

		for (typename InternalStateVector::const_iterator itStates = vec.begin();
			itStates != vec.end(); ++itStates)
		{
			typename InternalStateToStateMap::const_iterator itMap;
			if ((itMap = internalState2stateMap_.find(*itStates)) ==
				internalState2stateMap_.end())
			{
				throw std::runtime_error(__func__ +
					std::string(": request for invalid state = ") +
					Convert::ToString(*itStates));
			}

			result += " " + Convert::ToString(itMap->second);
		}

		return result;
	}

public:   // Public methods

	BUTreeAutomatonCover()
		: automaton_(),
			state2internalStateMap_(),
			internalState2stateMap_(),
			symbol2internalSymbolMap_(),
			internalSymbol2symbolMap_(),
			nextSymbol_(0)
	{ }

	explicit BUTreeAutomatonCover(TTWrapperPtr wrapper)
		: automaton_(wrapper),
			state2internalStateMap_(),
			internalState2stateMap_(),
			symbol2internalSymbolMap_(),
			internalSymbol2symbolMap_(),
			nextSymbol_(0)
	{ }

	void AddState(const StateType& state)
	{
		InternalStateType internalState = automaton_.AddState();

		if (!state2internalStateMap_.insert(
			std::make_pair(state, internalState)).second)
		{	// in case there has already been something in the place
			throw std::runtime_error(__func__ +
				std::string(": inserting already existing state " +
				Convert::ToString(state)));
		}

		if (!internalState2stateMap_.insert(
			std::make_pair(internalState, state)).second)
		{	// in case there has already been something in the place
			throw std::runtime_error(__func__ +
				std::string(": inserting already existing internal state " +
				Convert::ToString(internalState)));
		}
	}

	void AddSymbol(const SymbolType& symbol)
	{
		assert(false);
	}

	void AddTransition(const LeftHandSideType& lhs, const SymbolType& symbol,
		const RightHandSideType& rhs)
	{
		InternalLeftHandSideType internalLhs;
		for (typename LeftHandSideType::const_iterator itLhs = lhs.begin();
			itLhs != lhs.end(); ++itLhs)
		{
			typename StateToInternalStateMap::const_iterator itStates;
			if ((itStates = state2internalStateMap_.find(*itLhs)) ==
				state2internalStateMap_.end())
			{	// in case the state is unknown
				throw std::runtime_error(__func__ +
					std::string(": unknown state in a left-hand side = " +
					Convert::ToString(*itLhs)));
			}
			else
			{	// in case we know the state
				internalLhs.push_back(itStates->second);
			}
		}


		typename SymbolToInternalSymbolMap::const_iterator itSymbols;
		if ((itSymbols = symbol2internalSymbolMap_.find(symbol)) == 
			symbol2internalSymbolMap_.end())
		{	// in case the symbol is unknown
			std::pair<typename SymbolToInternalSymbolMap::iterator, bool> insertResult;
			insertResult = symbol2internalSymbolMap_.insert(
				std::make_pair(symbol, nextSymbol_));

			// Assertion
			assert(insertResult.second);

			itSymbols = insertResult.first;

			if (!(internalSymbol2symbolMap_.insert(
				std::make_pair(nextSymbol_, symbol)).second))
			{	// in case the inverse mapping is already taken
				throw std::runtime_error(__func__ +
					std::string(": inserting already existing symbol = " +
					Convert::ToString(nextSymbol_)));
			}

			++nextSymbol_;
		}

		InternalSymbolType internalSymbol = itSymbols->second;

		// retrieve the original right-hand side
		InternalRightHandSideType origRhs =
			automaton_.GetTransition(internalLhs, internalSymbol);

		// add new states
		for (typename RightHandSideType::const_iterator itRhs = rhs.begin();
			itRhs != rhs.end(); ++itRhs)
		{
			typename StateToInternalStateMap::const_iterator itStates;
			if ((itStates = state2internalStateMap_.find(*itRhs)) == 
				state2internalStateMap_.end())
			{	// in case some state is unknown
				throw std::runtime_error(__func__ +
					std::string(": transition to unknown symbol = " +
					Convert::ToString(*itRhs)));
			}
			origRhs.insert(itStates->second);
		}

		// update the right-hand side
		automaton_.AddTransition(internalLhs, internalSymbol, origRhs);
	}

	void SetStateFinal(const StateType& state)
	{
		typename StateToInternalStateMap::const_iterator itStates;
		if ((itStates = state2internalStateMap_.find(state)) ==
			state2internalStateMap_.end())
		{	// in case the state is unknown
			throw std::runtime_error(__func__ +
				std::string(": setting unknown state as final = " +
				Convert::ToString(state)));
		}
		else
		{	// in case we know the state
			automaton_.SetStateFinal(itStates->second);
		}
	}

	inline TTWrapperPtr GetTTWrapper()
	{
		return automaton_.GetTTWrapper();
	}


	std::string ToString() const
	{
		std::string result;
		
		result += "States";
		result += statesToString(automaton_.GetVectorOfStates());
		result += "\n";
		result += "\n";
		result += "Final States";
		result += statesToString(automaton_.GetVectorOfFinalStates());
		result += "\n";
		result += "\n";
		result += "Transitions";
		result += "\n";

		return result;
	}
};
#endif
