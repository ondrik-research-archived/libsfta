/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for TDTreeAutomatonCover class.
 *
 *****************************************************************************/

#ifndef _TD_TREE_AUTOMATON_COVER_HH_
#define _TD_TREE_AUTOMATON_COVER_HH_

// SFTA header files
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/mtbdd_transition_table_wrapper.hh>
#include <sfta/nd_symbolic_td_tree_automaton.hh>
#include <sfta/set.hh>
#include <sfta/sfta.hh>
#include <sfta/symbol_dictionary.hh>


// insert the class into proper namespace
namespace SFTA
{
	class TDTreeAutomatonCover;
}


/**
 * @brief   Cover class for top-down tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Class that provides nice interface to nondeterministic top-down finite
 * tree automaton.
 */
class SFTA::TDTreeAutomatonCover
{
public:   // Public data types

	typedef TDTreeAutomatonCover Type;

	typedef std::string StateType;
	typedef std::string SymbolType;

	typedef StateType LeftHandSideType;
	typedef SFTA::Set<SFTA::Vector<StateType> > RightHandSideType;

private:  // Private data types

	typedef unsigned InternalStateType;
	typedef SFTA::Private::CompactVariableAssignment InternalSymbolType;
	typedef InternalStateType InternalLeftHandSideType;

	typedef SFTA::Private::ElemOrVector<InternalStateType> InternalDualStateType;

	typedef SFTA::OrderedVector<InternalDualStateType> InternalRightHandSideType;
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

	typedef SFTA::NDSymbolicTDTreeAutomaton
	<
		MTBDDTransitionTableWrapper,
		InternalStateType,
		InternalSymbolType,
		SFTA::OrderedVector
	> NDSymbolicTDTreeAutomaton;


	typedef std::map<StateType, InternalStateType> StateToInternalStateMap;

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<InternalStateType> InternalStateVector;

	typedef typename NDSymbolicTDTreeAutomaton::TransitionType
		InternalTransitionType;


public:   // Public data types

	typedef typename NDSymbolicTDTreeAutomaton::TTWrapperPtrType TTWrapperPtr;

	typedef SFTA::SymbolDictionary
		<
			SymbolType,
			InternalSymbolType
		> SymbolDictionaryType;

	typedef Loki::SmartPtr<SymbolDictionaryType> SymbolDictionaryPtrType;


	/**
	 * @brief  Class with operations
	 *
	 * This class aggregates operations on the automaton cover into single
	 * location.
	 */
	class Operation
	{
	public:   // Public methods

		Type* Union(Type* lhs, Type* rhs) const;

		Type* Intersection(Type* lhs, Type* rhs) const;
	};

private:  // Private data members

	std::auto_ptr<NDSymbolicTDTreeAutomaton> automaton_;

	StateToInternalStateMap state2internalStateMap_;

	SymbolDictionaryPtrType symbolDict_;

	size_t bddSize_;

	InternalSymbolType nextSymbol_;


private:  // Private methods

	inline const std::auto_ptr<NDSymbolicTDTreeAutomaton>& getAutomaton() const
	{
		return automaton_;
	}

	inline StateType translateInternalStateToState(
		const InternalStateType& internalState) const
	{
		return "q" + Convert::ToString(internalState);
	}

	std::vector<SymbolType> translateInternalSymbolToSymbols(
		const InternalSymbolType& internalSymbol) const;

	std::string statesToString(const InternalStateVector& vec) const;

	std::string initialStatesToString(const InternalStateVector& vec) const;

	static std::string symbolsToString(const std::vector<SymbolType>& vec);

public:   // Public methods

	TDTreeAutomatonCover(size_t bddSize)
		: automaton_(new NDSymbolicTDTreeAutomaton()),
			state2internalStateMap_(),
			symbolDict_(),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }

	TDTreeAutomatonCover(size_t bddSize, TTWrapperPtr wrapper, SymbolDictionaryPtrType symbolDict)
		: automaton_(new NDSymbolicTDTreeAutomaton(wrapper)),
			state2internalStateMap_(),
			symbolDict_(symbolDict),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }

	TDTreeAutomatonCover(size_t bddSize, NDSymbolicTDTreeAutomaton* automaton, SymbolDictionaryPtrType symbolDict)
		: automaton_(automaton),
			state2internalStateMap_(),
			symbolDict_(symbolDict),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }


	void AddState(const StateType& state);

	void AddSymbol(const SymbolType& symbol);

	void AddTransition(const LeftHandSideType& lhs, const SymbolType& symbol,
		const RightHandSideType& rhs);

	void SetStateInitial(const StateType& state);

	inline size_t GetBDDSize() const
	{
		return bddSize_;
	}

	inline TTWrapperPtr GetTTWrapper()
	{
		return automaton_->GetTTWrapper();
	}

	inline Operation* GetOperation() const
	{
		return new Operation();
	}

	inline SymbolDictionaryPtrType GetSymbolDictionary() const
	{
		return symbolDict_;
	}

	std::string ToString() const;
};

#endif
