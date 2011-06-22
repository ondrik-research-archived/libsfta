/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation of TDTreeAutomatonCover class.
 *
 *****************************************************************************/

#include <sfta/td_tree_automaton_cover.hh>


// Methods of TDTreeAutomatonCover

std::string SFTA::TDTreeAutomatonCover::statesToString(
	const InternalStateVector& vec) const
{
	std::string result;

	for (typename InternalStateVector::const_iterator itStates = vec.begin();
		itStates != vec.end(); ++itStates)
	{
		result += " " + translateInternalStateToState(*itStates) + ":0";
	}

	return result;
}


std::string SFTA::TDTreeAutomatonCover::ToString() const
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
				if (itRhs->IsElement())
				{
					throw std::runtime_error(__func__ + std::string(": invalid type"));

				}
				const typename InternalDualStateType::VectorType& vecRhs =
					itRhs->GetVector();
				SFTA::Vector<StateType> outputRhs;
				for (typename InternalDualStateType::VectorType::const_iterator
					itVecRhs = vecRhs.begin(); itVecRhs != vecRhs.end(); ++itVecRhs)
				{
					outputRhs.push_back(translateInternalStateToState(*itVecRhs));
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


void SFTA::TDTreeAutomatonCover::SetStateInitial(const StateType& state)
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


void SFTA::TDTreeAutomatonCover::AddTransition( const LeftHandSideType& lhs,
	const SymbolType& symbol, const RightHandSideType& rhs)
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


void SFTA::TDTreeAutomatonCover::AddSymbol(const SymbolType& symbol)
{
	symbolDict_->Translate(symbol);
}


void SFTA::TDTreeAutomatonCover::AddState(const StateType& state)
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


std::string SFTA::TDTreeAutomatonCover::symbolsToString(
	const std::vector<SymbolType>& vec)
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


std::string SFTA::TDTreeAutomatonCover::initialStatesToString(
	const InternalStateVector& vec) const
{
	std::string result;

	for (typename InternalStateVector::const_iterator itStates = vec.begin();
		itStates != vec.end(); ++itStates)
	{
		result += " " + translateInternalStateToState(*itStates);
	}

	return result;
}


std::vector<SFTA::TDTreeAutomatonCover::SymbolType>
	SFTA::TDTreeAutomatonCover::translateInternalSymbolToSymbols(
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


// Methods of TDTreeAutomatonCover::Operation


SFTA::TDTreeAutomatonCover::Type*
	SFTA::TDTreeAutomatonCover::Operation::Union(Type* lhs, Type* rhs) const
{
	// Assertion
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));

	if (lhs->GetBDDSize() != rhs->GetBDDSize())
	{	// in case the BDD sizes do not match
		throw std::runtime_error("BDD sizes do not match!");
	}

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

	return new Type(lhs->GetBDDSize(), result, lhs->GetSymbolDictionary());
}


SFTA::TDTreeAutomatonCover::Type*
	SFTA::TDTreeAutomatonCover::Operation::Intersection(Type* lhs, Type* rhs) const
{
	// Assertion
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));

	if (lhs->GetBDDSize() != rhs->GetBDDSize())
	{	// in case the BDD sizes do not match
		throw std::runtime_error("BDD sizes do not match!");
	}

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

	return new Type(lhs->GetBDDSize(), result, lhs->GetSymbolDictionary());
}
