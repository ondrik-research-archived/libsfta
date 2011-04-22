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
	template
	<
		size_t SymbolLength = 8//64
	>
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
		template <typename T>
		struct hash<SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> > >
			: public std::unary_function<SFTA::OrderedVector<SFTA::Private::ElemOrVector<T> >, size_t>
		{
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
template
<
	size_t SymbolLength
>
class SFTA::BUTreeAutomatonCover
{
public:   // Public data types

	typedef BUTreeAutomatonCover
		<
			SymbolLength
		> Type;

	typedef std::string StateType;
	typedef std::string SymbolType;

	typedef SFTA::Vector<StateType> LeftHandSideType;
	typedef SFTA::Set<StateType> RightHandSideType;

private:  // Private data types

	typedef unsigned InternalStateType;
	typedef SFTA::Private::CompactVariableAssignment<SymbolLength>
		InternalSymbolType;
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

		Type* Union(Type* lhs, Type* rhs) const
		{
			// Assertions
			assert(lhs != static_cast<Type*>(0));
			assert(rhs != static_cast<Type*>(0));

			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			std::auto_ptr<typename AbstractAutomaton::Operation> oper(
				lhs->getAutomaton()->GetOperation());
			AbstractAutomaton* abstractResult =
				oper->Union((lhs->getAutomaton()).get(), (rhs->getAutomaton()).get());

			NDSymbolicBUTreeAutomaton* result;
			if ((result = dynamic_cast<NDSymbolicBUTreeAutomaton*>(abstractResult)) ==
				static_cast<NDSymbolicBUTreeAutomaton*>(0))
			{
				throw std::runtime_error(__func__ +
					std::string(": cannot convert to proper type"));
			}

			return new Type(result, lhs->GetSymbolDictionary());
		}

		Type* Intersection(Type* lhs, Type* rhs) const
		{
			// Assertions
			assert(lhs != static_cast<Type*>(0));
			assert(rhs != static_cast<Type*>(0));

			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			std::auto_ptr<typename AbstractAutomaton::Operation> oper(
				lhs->getAutomaton()->GetOperation());
			AbstractAutomaton* abstractResult =
				oper->Intersection((lhs->getAutomaton()).get(), (rhs->getAutomaton()).get());

			NDSymbolicBUTreeAutomaton* result;
			if ((result = dynamic_cast<NDSymbolicBUTreeAutomaton*>(abstractResult)) ==
				static_cast<NDSymbolicBUTreeAutomaton*>(0))
			{
				throw std::runtime_error(__func__ +
					std::string(": cannot convert to proper type"));
			}

			return new Type(result, lhs->GetSymbolDictionary());
		}

		SimulationRelationType ComputeSimulationPreorder(const Type* aut) const
		{
			// Assertions
			assert(aut != static_cast<Type*>(0));

			SimulationRelationType result;

			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			typedef typename AbstractAutomaton::Operation InternalOperationType;
			typedef typename InternalOperationType::SimulationRelationType
				InternalSimulationType;

			std::auto_ptr<InternalOperationType> oper(aut->getAutomaton()->GetOperation());
			std::auto_ptr<InternalSimulationType> simulation(
				oper->ComputeSimulationPreorder((aut->getAutomaton()).get()));


			std::vector<InternalStateType> internalStates =
				aut->getAutomaton()->GetVectorOfStates();

			for (size_t iState = 0; iState < internalStates.size(); ++iState)
			{
				for (size_t jState = 0; jState < internalStates.size(); ++jState)
				{
					const InternalStateType& lesserState = internalStates[iState];
					const InternalStateType& biggerState = internalStates[jState];

					if (simulation->is_in(std::make_pair(lesserState, biggerState)))
					{	// in case the states are in the relation
						result.insert(std::make_pair(aut->translateInternalStateToState(
							lesserState), aut->translateInternalStateToState(biggerState)));
					}
				}
			}

			return result;
		}

		bool DoesLanguageInclusionHoldUpwards(const Type* lhs, const Type* rhs) const
		{
			// Assertions
			assert(lhs != static_cast<Type*>(0));
			assert(rhs != static_cast<Type*>(0));

			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			typedef typename AbstractAutomaton::Operation InternalOperationType;
			typedef typename InternalOperationType::SimulationRelationType
				InternalSimulationType;

			// check language inclusion
			std::auto_ptr<InternalOperationType> buOper(lhs->getAutomaton()->GetOperation());
			return buOper->CheckLanguageInclusion(lhs->getAutomaton().get(), rhs->getAutomaton().get(),
				static_cast<InternalSimulationType*>(0), static_cast<InternalSimulationType*>(0));
		}


		bool DoesLanguageInclusionHoldDownwards(const Type* lhs, const Type* rhs) const
		{
			// Assertions
			assert(lhs != static_cast<Type*>(0));
			assert(rhs != static_cast<Type*>(0));

			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			typedef typename AbstractAutomaton::Operation InternalOperationType;
			typedef typename InternalOperationType::SimulationRelationType
				InternalSimulationType;

			// compute simulations
			std::auto_ptr<InternalOperationType> oper(lhs->getAutomaton()->GetOperation());
			std::auto_ptr<InternalSimulationType> lhsSim(
				oper->ComputeSimulationPreorder((lhs->getAutomaton()).get()));
			std::auto_ptr<InternalSimulationType> rhsSim(
				oper->ComputeSimulationPreorder((rhs->getAutomaton()).get()));

			// convert automata to top-down
			std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
				lhsTD(lhs->getAutomaton()->GetTopDownAutomaton());
			std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
				rhsTD(rhs->getAutomaton()->GetTopDownAutomaton());

			// check language inclusion
			std::auto_ptr<InternalOperationType> tdOper(lhsTD.get()->GetOperation());
			return tdOper->CheckLanguageInclusion(lhsTD.get(), rhsTD.get(), lhsSim.get(),
				rhsSim.get());
		}
	};


private:  // Private data members

	std::auto_ptr<NDSymbolicBUTreeAutomaton> automaton_;

	StateToInternalStateMap state2internalStateMap_;

	bool areStatesFromOutside_;

	SymbolDictionaryPtrType symbolDict_;

	InternalSymbolType nextSymbol_;

private:  // Private methods

	inline const std::auto_ptr<NDSymbolicBUTreeAutomaton>& getAutomaton() const
	{
		return automaton_;
	}

	inline StateType translateInternalStateToState(
		const InternalDualStateType& internalState) const
	{
		if (!internalState.IsElement())
		{
			throw std::runtime_error(__func__ +
				std::string(": invalid state type"));
		}

		const InternalStateType& state = internalState.GetElement();

		if (areStatesFromOutside_)
		{
			for (typename StateToInternalStateMap::const_iterator itStates =
				state2internalStateMap_.begin(); itStates != state2internalStateMap_.end();
				++itStates)
			{
				if (itStates->second == state)
				{
					return itStates->first;
				}
			}

			throw std::runtime_error(__func__ + std::string(": could not find state ") +
				Convert::ToString(internalState));
		}
		else
		{
			return "q" + Convert::ToString(internalState);
		}
	}

	std::vector<SymbolType> translateInternalSymbolToSymbols(
		const InternalSymbolType& internalSymbol) const
	{
		std::vector<SymbolType> result;

		typedef std::vector<InternalSymbolType> InternalSymbolVector;

		InternalSymbolVector symbols = internalSymbol.GetVectorOfConcreteSymbols();
		for (typename InternalSymbolVector::const_iterator itSym = symbols.begin();
			itSym != symbols.end(); ++itSym)
		{
			result.push_back(symbolDict_->TranslateInverse(*itSym));
		}

		return result;
	}

	std::string statesToString(const InternalStateVector& vec) const
	{
		std::string result;

		for (typename InternalStateVector::const_iterator itStates = vec.begin();
			itStates != vec.end(); ++itStates)
		{
			result += " " + translateInternalStateToState(*itStates) + ":0";
		}

		return result;
	}

	std::string finalStatesToString(const InternalStateVector& vec) const
	{
		std::string result;

		for (typename InternalStateVector::const_iterator itStates = vec.begin();
			itStates != vec.end(); ++itStates)
		{
			result += " " + translateInternalStateToState(*itStates);
		}

		return result;
	}

	static std::string symbolsToString(const std::vector<SymbolType>& vec)
	{
		std::string result;

		typedef std::vector<SymbolType> VectorOfSymbols;

		for (typename VectorOfSymbols::const_iterator itSymbols = vec.begin();
			itSymbols != vec.end(); ++itSymbols)
		{
			result += " " + Convert::ToString(*itSymbols) + ":0";
		}

		return result;
	}


public:   // Public methods

	BUTreeAutomatonCover()
		: automaton_(new NDSymbolicBUTreeAutomaton()),
			state2internalStateMap_(),
			areStatesFromOutside_(true),
			symbolDict_(),
			nextSymbol_(0)
	{ }

	BUTreeAutomatonCover(TTWrapperPtr wrapper, SymbolDictionaryPtrType symbolDict)
		: automaton_(new NDSymbolicBUTreeAutomaton(wrapper)),
			state2internalStateMap_(),
			areStatesFromOutside_(true),
			symbolDict_(symbolDict),
			nextSymbol_(0)
	{ }

	BUTreeAutomatonCover(NDSymbolicBUTreeAutomaton* automaton, SymbolDictionaryPtrType symbolDict)
		: automaton_(automaton),
			state2internalStateMap_(),
			areStatesFromOutside_(false),
			symbolDict_(symbolDict),
			nextSymbol_(0)
	{ }

	void AddState(const StateType& state)
	{
		InternalStateType internalState = automaton_->AddState();

		if (!state2internalStateMap_.insert(
			std::make_pair(state, internalState)).second)
		{	// in case there has already been something in the place
			throw std::runtime_error(__func__ +
				std::string(": inserting already existing state " +
				Convert::ToString(state)));
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

		// translate the symbol
		InternalSymbolType internalSymbol = symbolDict_->Translate(symbol);

		// retrieve the original right-hand side
		InternalRightHandSideType origRhs =
			automaton_->GetTransition(internalLhs, internalSymbol);

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
		automaton_->AddTransition(internalLhs, internalSymbol, origRhs);
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
			automaton_->SetStateFinal(itStates->second);
		}
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

	std::string ToString() const
	{
		std::string result;

		result += "Ops";
		result += symbolsToString(symbolDict_->GetVectorOfInputSymbols());
		result += "\n";
		result += "\n";
		result += "Automaton babicka";
		result += "\n";
		result += "\n";
		result += "States";
		result += statesToString(automaton_->GetVectorOfStates());
		result += "\n";
		result += "\n";
		result += "Final States";
		result += finalStatesToString(automaton_->GetVectorOfFinalStates());
		result += "\n";
		result += "\n";
		result += "Transitions";
		result += "\n";

		typedef std::vector<InternalTransitionType> TransitionVector;

		TransitionVector trans = automaton_->GetVectorOfTransitions();
		for (typename TransitionVector::const_iterator itTrans = trans.begin();
			itTrans != trans.end(); ++itTrans)
		{
			const InternalLeftHandSideType& lhs = itTrans->lhs;

			LeftHandSideType outputLhs;
			for (typename InternalLeftHandSideType::const_iterator itLhs = lhs.begin();
				 itLhs != lhs.end(); ++itLhs)
			{
				outputLhs.push_back(translateInternalStateToState(*itLhs));
			}

			typedef std::vector<SymbolType> SymbolVector;
			SymbolVector symbols = translateInternalSymbolToSymbols(itTrans->symbol);

			for (typename SymbolVector::const_iterator itSymbols = symbols.begin();
				itSymbols != symbols.end(); ++itSymbols)
			{
				const InternalRightHandSideType& rhs = itTrans->rhs;
				for (typename InternalRightHandSideType::const_iterator itRhs = rhs.begin();
					 itRhs != rhs.end(); ++itRhs)
				{
					result += Convert::ToString(*itSymbols);
					result += (outputLhs.empty()? " " : Convert::ToString(outputLhs));
					result += " -> ";
					result += Convert::ToString(translateInternalStateToState(*itRhs));
					result += "\n";
				}
			}
		}

		return result;
	}
};
#endif
