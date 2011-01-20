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
#include <sfta/nd_symbolic_td_tree_automaton.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		size_t SymbolLength = 64
	>
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
template
<
	size_t SymbolLength
>
class SFTA::TDTreeAutomatonCover
{
public:   // Public data types

	typedef TDTreeAutomatonCover
		<
			SymbolLength
		> Type;

	typedef std::string StateType;
	typedef std::string SymbolType;

	typedef StateType LeftHandSideType;
	typedef SFTA::Set<SFTA::Vector<StateType> > RightHandSideType;

private:  // Private data types

	typedef unsigned InternalStateType;
	typedef SFTA::Private::CompactVariableAssignment<SymbolLength>
		InternalSymbolType;
	typedef InternalStateType InternalLeftHandSideType;
	typedef SFTA::OrderedVector<SFTA::Vector<InternalStateType> > InternalRightHandSideType;
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

		Type* Union(Type* lhs, Type* rhs) const
		{
			typedef typename NDSymbolicTDTreeAutomaton::HierarchyRoot AbstractAutomaton;
			std::auto_ptr<typename AbstractAutomaton::Operation> oper(
				lhs->getAutomaton()->GetOperation());
			AbstractAutomaton* abstractResult =
				oper->Union((lhs->getAutomaton()).get(), (rhs->getAutomaton()).get());

			NDSymbolicTDTreeAutomaton* result;
			if ((result = dynamic_cast<NDSymbolicTDTreeAutomaton*>(abstractResult)) ==
				static_cast<NDSymbolicTDTreeAutomaton*>(0))
			{
				throw std::runtime_error(__func__ +
					std::string(": cannot convert to proper type"));
			}

			return new Type(result, lhs->GetSymbolDictionary());
		}

		Type* Intersection(Type* lhs, Type* rhs) const
		{
			typedef typename NDSymbolicTDTreeAutomaton::HierarchyRoot AbstractAutomaton;
			std::auto_ptr<typename AbstractAutomaton::Operation> oper(
				lhs->getAutomaton()->GetOperation());
			AbstractAutomaton* abstractResult =
				oper->Intersection((lhs->getAutomaton()).get(), (rhs->getAutomaton()).get());

			NDSymbolicTDTreeAutomaton* result;
			if ((result = dynamic_cast<NDSymbolicTDTreeAutomaton*>(abstractResult)) ==
				static_cast<NDSymbolicTDTreeAutomaton*>(0))
			{
				throw std::runtime_error(__func__ +
					std::string(": cannot convert to proper type"));
			}

			return new Type(result, lhs->GetSymbolDictionary());
		}
	};

private:  // Private data members

	std::auto_ptr<NDSymbolicTDTreeAutomaton> automaton_;

	StateToInternalStateMap state2internalStateMap_;

	SymbolDictionaryPtrType symbolDict_;

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

	std::string initialStatesToString(const InternalStateVector& vec) const
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

	TDTreeAutomatonCover()
		: automaton_(new NDSymbolicTDTreeAutomaton()),
			state2internalStateMap_(),
			symbolDict_(),
			nextSymbol_(0)
	{ }

	TDTreeAutomatonCover(TTWrapperPtr wrapper, SymbolDictionaryPtrType symbolDict)
		: automaton_(new NDSymbolicTDTreeAutomaton(wrapper)),
			state2internalStateMap_(),
			symbolDict_(symbolDict),
			nextSymbol_(0)
	{ }

	TDTreeAutomatonCover(NDSymbolicTDTreeAutomaton* automaton, SymbolDictionaryPtrType symbolDict)
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
		typename StateToInternalStateMap::const_iterator itStates;
		if ((itStates = state2internalStateMap_.find(lhs)) ==
			state2internalStateMap_.end())
		{	// in case the state is unknown
			throw std::runtime_error(__func__ +
				std::string(": unknown state in a left-hand side = " +
				Convert::ToString(lhs)));
		}
		else
		{	// in case we know the state
			internalLhs = itStates->second;
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
			SFTA::Vector<InternalStateType> newSuperState;

			for (typename SFTA::Vector<StateType>::const_iterator itVec = itRhs->begin();
				itVec != itRhs->end(); ++itVec)
			{
				typename StateToInternalStateMap::const_iterator itStates;
				if ((itStates = state2internalStateMap_.find(*itVec)) ==
					state2internalStateMap_.end())
				{	// in case some state is unknown
					throw std::runtime_error(__func__ +
						std::string(": transition to unknown symbol = " +
						Convert::ToString(*itRhs)));
				}

				newSuperState.push_back(itStates->second);
			}

			origRhs.insert(newSuperState);
		}

		// update the right-hand side
		automaton_->AddTransition(internalLhs, internalSymbol, origRhs);
	}

	void SetStateInitial(const StateType& state)
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
			automaton_->SetStateInitial(itStates->second);
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
		result += "Automaton dedecek";
		result += "\n";
		result += "\n";
		result += "States";
		result += statesToString(automaton_->GetVectorOfStates());
		result += "\n";
		result += "\n";
		result += "Final States";
		result += initialStatesToString(automaton_->GetVectorOfInitialStates());
		result += "\n";
		result += "\n";
		result += "Transitions";
		result += "\n";

		typedef std::vector<InternalTransitionType> TransitionVector;

		TransitionVector trans = automaton_->GetVectorOfTransitions();
		for (typename TransitionVector::const_iterator itTrans = trans.begin();
			itTrans != trans.end(); ++itTrans)
		{
			typedef std::vector<SymbolType> SymbolVector;
			SymbolVector symbols = translateInternalSymbolToSymbols(itTrans->symbol);

			for (typename SymbolVector::const_iterator itSymbols = symbols.begin();
				itSymbols != symbols.end(); ++itSymbols)
			{
				const InternalRightHandSideType& rhs = itTrans->rhs;

				if (rhs.empty())
				{	// in case there is nullary transition
					result += Convert::ToString(*itSymbols);
					result += " -> ";
					result += Convert::ToString(translateInternalStateToState(itTrans->lhs));
					result += "\n";
				}

				for (typename InternalRightHandSideType::const_iterator itRhs = rhs.begin();
					 itRhs != rhs.end(); ++itRhs)
				{
	        const SFTA::Vector<InternalStateType>& rhs = *itRhs;
					SFTA::Vector<StateType> outputRhs;
					for (typename SFTA::Vector<InternalStateType>::const_iterator itRhs = rhs.begin();
						 itRhs != rhs.end(); ++itRhs)
					{
						outputRhs.push_back(translateInternalStateToState(*itRhs));
					}

					result += Convert::ToString(*itSymbols);
					result += (outputRhs.empty()? " " : Convert::ToString(outputRhs));
					result += " -> ";
					result += Convert::ToString(translateInternalStateToState(itTrans->lhs));
					result += "\n";
				}
			}

		}

		return result;
	}
};

#endif
