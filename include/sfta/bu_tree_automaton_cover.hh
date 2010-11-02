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
#include <sfta/symbol_dictionary.hh>
#include <sfta/vector.hh>


// Loki header files
#include <loki/SmartPtr.h>


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

	typedef std::map<SymbolType, InternalSymbolType> SymbolToInternalSymbolMap;
	typedef std::map<InternalSymbolType, SymbolType> InternalSymbolToSymbolMap;

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<InternalStateType> InternalStateVector;

	typedef typename NDSymbolicBUTreeAutomaton::TransitionType
		InternalTransitionType;

public:   // Public data types

	typedef typename NDSymbolicBUTreeAutomaton::TTWrapperPtrType TTWrapperPtr;

	typedef SFTA::SymbolDictionary
		<
			SymbolType,
			InternalSymbolType
		> SymbolDictionaryType;

	typedef Loki::SmartPtr<SymbolDictionaryType> SymbolDictionaryPtrType;

	class Operation
	{
	public:   // Public methods

		Type* Union(Type* lhs, Type* rhs) const
		{
			typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
			typename AbstractAutomaton::Operation* oper =
				lhs->getAutomaton()->GetOperation();
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
	};


private:  // Private data members

	std::auto_ptr<NDSymbolicBUTreeAutomaton> automaton_;

	StateToInternalStateMap state2internalStateMap_;

	SymbolDictionaryPtrType symbolDict_;

	InternalSymbolType nextSymbol_;

private:  // Private methods

	inline const std::auto_ptr<NDSymbolicBUTreeAutomaton>& getAutomaton() const
	{
		return automaton_;
	}

	inline StateType translateInternalStateToState(
		const InternalStateType& internalState) const
	{
		return "q" + Convert::ToString(internalState);
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
			result += " " + translateInternalStateToState(*itStates);
		}

		return result;
	}

public:   // Public methods

	BUTreeAutomatonCover()
		: automaton_(new NDSymbolicBUTreeAutomaton()),
			state2internalStateMap_(),
			symbolDict_(),
			nextSymbol_(0)
	{ }

	BUTreeAutomatonCover(TTWrapperPtr wrapper, SymbolDictionaryPtrType symbolDict)
		: automaton_(new NDSymbolicBUTreeAutomaton(wrapper)),
			state2internalStateMap_(),
			symbolDict_(symbolDict),
			nextSymbol_(0)
	{ }

	BUTreeAutomatonCover(NDSymbolicBUTreeAutomaton* automaton, SymbolDictionaryPtrType symbolDict)
		: automaton_(automaton),
			state2internalStateMap_(),
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
		
		result += "States";
		result += statesToString(automaton_->GetVectorOfStates());
		result += "\n";
		result += "\n";
		result += "Final States";
		result += statesToString(automaton_->GetVectorOfFinalStates());
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
					result += Convert::ToString(outputLhs);
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
