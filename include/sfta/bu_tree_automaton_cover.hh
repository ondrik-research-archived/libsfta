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
#include <sfta/dual_hash_table_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/mtbdd_transition_table_wrapper.hh>
#include <sfta/nd_symbolic_bu_tree_automaton.hh>
#include <sfta/set.hh>
#include <sfta/symbol_dictionary.hh>
#include <sfta/vector.hh>


// Loki header files
#include <loki/SmartPtr.h>


// insert the class into proper namespace
namespace SFTA
{
	class BUTreeAutomatonCover;
}


namespace boost
{
	template <class T>
	std::size_t hash_value(const SFTA::Private::ElemOrVector<T>& v)
	{
		if (v.IsElement())
		{
			const T& ref = v.GetElement();
			return boost::hash_value(ref);
		}
		else
		{
			const std::vector<T>& ref = v.GetVector();
			return boost::hash_range(ref.begin(), ref.end());
		}
	}
}


namespace std
{
	namespace tr1
	{
		GCC_DIAG_OFF(effc++)
		template <typename T>
		struct hash<SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> > >
			: public std::unary_function<SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> >, size_t>
		{
		GCC_DIAG_ON(effc++)
			std::size_t operator()(const SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> >& val) const
			{
				//return boost::hash_range(val.begin(), val.end());
				size_t seed = 0;

				for (typename SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> >::const_iterator first = val.begin(); first != val.end(); ++first)
				{
					size_t newHash;
					if (first->IsElement())
					{
						const T& ref = first->GetElement();
						newHash = boost::hash_value(ref);
					}
					else
					{
						const std::vector<T>& ref = first->GetVector();
						newHash = boost::hash_range(ref.begin(), ref.end());
					}

					boost::hash_combine(seed, newHash);
				}

				return seed;
			}
		};
	}
}



/**
 * @brief   Cover class for bottom-up tree automaton
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Class that provides nice interface to nondeterministic bottom-up finite
 * tree automaton.
 */
class SFTA::BUTreeAutomatonCover
{
public:   // Public data types

	typedef BUTreeAutomatonCover Type;

	typedef std::string StateType;
	typedef std::string SymbolType;

	typedef SFTA::Vector<StateType> LeftHandSideType;
	typedef SFTA::Set<StateType> RightHandSideType;

private:  // Private data types

	typedef unsigned InternalStateType;
	typedef SFTA::Private::CompactVariableAssignment InternalSymbolType;
	typedef SFTA::Vector<InternalStateType> InternalLeftHandSideType;

	typedef SFTA::Private::ElemOrVector<InternalStateType> InternalDualStateType;

	typedef SFTA::OrderedVector<InternalDualStateType> InternalRightHandSideType;
	typedef unsigned MTBDDRootType;



	typedef SFTA::CUDDSharedMTBDD
	<
		MTBDDRootType,
		InternalRightHandSideType,
		InternalSymbolType,
		SFTA::Private::DualHashTableLeafAllocator,
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

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<InternalStateType> InternalStateVector;

	typedef typename NDSymbolicBUTreeAutomaton::TransitionType
		InternalTransitionType;

public:   // Public data types

	typedef typename NDSymbolicBUTreeAutomaton::TTWrapperPtrType TTWrapperPtr;

	typedef std::multimap<StateType, StateType> SimulationRelationType;

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

		SimulationRelationType ComputeSimulationPreorder(const Type* aut) const;

		bool DoesLanguageInclusionHoldUpwards(const Type* lhs, const Type* rhs) const;

		bool DoesLanguageInclusionHoldDownwards(const Type* lhs, const Type* rhs) const;

		bool DoesLanguageInclusionHoldDownwardsSimBoth(const Type* lhs,
			const Type* rhs) const;

		bool DoesLanguageInclusionHoldDownwardsSimBothNoSimTime(const Type* lhs,
			const Type* rhs, timespec* start) const;

		bool DoesLanguageInclusionHoldDownwardsNoSimTime(const Type* lhs,
			const Type* rhs, timespec* start) const;

		bool DoesLanguageInclusionHoldDownwardsWithoutSim(const Type* lhs,
			const Type* rhs, timespec* start) const;
	};


private:  // Private data members

	std::auto_ptr<NDSymbolicBUTreeAutomaton> automaton_;

	StateToInternalStateMap state2internalStateMap_;

	bool areStatesFromOutside_;

	SymbolDictionaryPtrType symbolDict_;

	size_t bddSize_;

	InternalSymbolType nextSymbol_;

private:  // Private methods

	inline const std::auto_ptr<NDSymbolicBUTreeAutomaton>& getAutomaton() const
	{
		return automaton_;
	}

	StateType translateInternalStateToState(
		const InternalDualStateType& internalState) const;

	std::vector<SymbolType> translateInternalSymbolToSymbols(
		const InternalSymbolType& internalSymbol) const;

	std::string statesToString(const InternalStateVector& vec) const;

	std::string finalStatesToString(const InternalStateVector& vec) const;

	static std::string symbolsToString(const std::vector<SymbolType>& vec);


public:   // Public methods

	explicit BUTreeAutomatonCover(size_t bddSize)
		: automaton_(new NDSymbolicBUTreeAutomaton()),
			state2internalStateMap_(),
			areStatesFromOutside_(true),
			symbolDict_(),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }

	BUTreeAutomatonCover(size_t bddSize, TTWrapperPtr wrapper, SymbolDictionaryPtrType symbolDict)
		: automaton_(new NDSymbolicBUTreeAutomaton(wrapper)),
			state2internalStateMap_(),
			areStatesFromOutside_(true),
			symbolDict_(symbolDict),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }

	BUTreeAutomatonCover(size_t bddSize, NDSymbolicBUTreeAutomaton* automaton, SymbolDictionaryPtrType symbolDict)
		: automaton_(automaton),
			state2internalStateMap_(),
			areStatesFromOutside_(false),
			symbolDict_(symbolDict),
			bddSize_(bddSize),
			nextSymbol_(bddSize, 0)
	{ }

	void AddState(const StateType& state);

	inline void AddSymbol(const SymbolType& symbol)
	{
		symbolDict_->Translate(symbol);
	}

	void AddTransition(const LeftHandSideType& lhs, const SymbolType& symbol,
		const RightHandSideType& rhs);

	void SetStateFinal(const StateType& state);

	inline TTWrapperPtr GetTTWrapper()
	{
		return automaton_->GetTTWrapper();
	}

	inline Operation* GetOperation() const
	{
		return new Operation();
	}

	inline size_t GetBDDSize() const
	{
		return bddSize_;
	}

	inline SymbolDictionaryPtrType GetSymbolDictionary() const
	{
		return symbolDict_;
	}

	std::string ToString() const;
};
#endif
