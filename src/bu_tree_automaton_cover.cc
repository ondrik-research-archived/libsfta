/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation of BUTreeAutomatonCover class.
 *
 *****************************************************************************/

#include <sfta/bu_tree_automaton_cover.hh>


// Methods of BUTreeAutomatonCover

std::string SFTA::BUTreeAutomatonCover::ToString() const
{
	std::string result;

	result += "Ops";
	result += symbolsToString(symbolDict_->GetVectorOfInputSymbols());
	result += "\n";
	result += "\n";
	result += "Automaton aut";
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


void SFTA::BUTreeAutomatonCover::SetStateFinal(const StateType& state)
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


void SFTA::BUTreeAutomatonCover::AddTransition(const LeftHandSideType& lhs,
	const SymbolType& symbol, const RightHandSideType& rhs)
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


void SFTA::BUTreeAutomatonCover::AddState(const StateType& state)
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


std::string SFTA::BUTreeAutomatonCover::symbolsToString(
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

std::string SFTA::BUTreeAutomatonCover::finalStatesToString(
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

std::string SFTA::BUTreeAutomatonCover::statesToString(
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

std::vector<SFTA::BUTreeAutomatonCover::SymbolType>
	SFTA::BUTreeAutomatonCover::translateInternalSymbolToSymbols(
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

SFTA::BUTreeAutomatonCover::StateType
	SFTA::BUTreeAutomatonCover::translateInternalStateToState(
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


// Methods of BUTreeAutomatonCover::Operation


SFTA::BUTreeAutomatonCover::Type*
	SFTA::BUTreeAutomatonCover::Operation::Union(Type* lhs, Type* rhs) const
{
	// Assertions
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));

	if (lhs->GetBDDSize() != rhs->GetBDDSize())
	{	// check if the BDD sizes match
		throw std::runtime_error("The sizes of BDDs do not match!");
	}

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

	return new Type(lhs->GetBDDSize(), result, lhs->GetSymbolDictionary());
}


SFTA::BUTreeAutomatonCover::Type*
	SFTA::BUTreeAutomatonCover::Operation::Intersection(Type* lhs, Type* rhs) const
{
	// Assertions
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));

	if (lhs->GetBDDSize() != rhs->GetBDDSize())
	{	// check if the BDD sizes match
		throw std::runtime_error("The sizes of BDDs do not match!");
	}

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

	return new Type(lhs->GetBDDSize(), result, lhs->GetSymbolDictionary());
}


SFTA::BUTreeAutomatonCover::SimulationRelationType
	SFTA::BUTreeAutomatonCover::Operation::ComputeSimulationPreorder(
	const Type* aut) const
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


bool SFTA::BUTreeAutomatonCover::Operation::DoesLanguageInclusionHoldUpwards(
	const Type* lhs, const Type* rhs) const
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


bool SFTA::BUTreeAutomatonCover::Operation::DoesLanguageInclusionHoldDownwards(
	const Type* lhs, const Type* rhs) const
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


bool SFTA::BUTreeAutomatonCover::Operation::
	DoesLanguageInclusionHoldDownwardsSimBoth(const Type* lhs, const Type* rhs) const
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
	std::auto_ptr<AbstractAutomaton> united(oper->Union((lhs->getAutomaton().get()),
		(rhs->getAutomaton()).get()));
	std::auto_ptr<InternalSimulationType> sim(oper->ComputeSimulationPreorder(united.get()));

	// convert automata to top-down
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		lhsTD(lhs->getAutomaton()->GetTopDownAutomaton());
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		rhsTD(rhs->getAutomaton()->GetTopDownAutomaton());

	// check language inclusion
	std::auto_ptr<InternalOperationType> tdOper(lhsTD.get()->GetOperation());
	return tdOper->CheckLanguageInclusion(lhsTD.get(), rhsTD.get(), sim.get(), sim.get());
}

bool SFTA::BUTreeAutomatonCover::Operation::
	DoesLanguageInclusionHoldDownwardsSimBothNoSimTime(
	const Type* lhs, const Type* rhs, timespec* start) const
{
	// Assertions
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));
	assert(start != static_cast<timespec*>(0));

	typedef typename NDSymbolicBUTreeAutomaton::HierarchyRoot AbstractAutomaton;
	typedef typename AbstractAutomaton::Operation InternalOperationType;
	typedef typename InternalOperationType::SimulationRelationType
		InternalSimulationType;

	// compute simulations
	std::auto_ptr<InternalOperationType> oper(lhs->getAutomaton()->GetOperation());
	std::auto_ptr<AbstractAutomaton> united(oper->Union((lhs->getAutomaton().get()),
		(rhs->getAutomaton()).get()));
	std::auto_ptr<InternalSimulationType> sim(oper->ComputeSimulationPreorder(united.get()));

	// convert automata to top-down
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		lhsTD(lhs->getAutomaton()->GetTopDownAutomaton());
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		rhsTD(rhs->getAutomaton()->GetTopDownAutomaton());

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, start);

	// check language inclusion
	std::auto_ptr<InternalOperationType> tdOper(lhsTD.get()->GetOperation());
	return tdOper->CheckLanguageInclusion(lhsTD.get(), rhsTD.get(), sim.get(), sim.get());
}


bool SFTA::BUTreeAutomatonCover::Operation::
	DoesLanguageInclusionHoldDownwardsNoSimTime(const Type* lhs,
	const Type* rhs, timespec* start) const
{
	// Assertions
	assert(lhs != static_cast<Type*>(0));
	assert(rhs != static_cast<Type*>(0));
	assert(start != static_cast<timespec*>(0));

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

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, start);

	// check language inclusion
	std::auto_ptr<InternalOperationType> tdOper(lhsTD.get()->GetOperation());
	return tdOper->CheckLanguageInclusion(lhsTD.get(), rhsTD.get(), lhsSim.get(),
		rhsSim.get());
}


bool SFTA::BUTreeAutomatonCover::Operation::
	DoesLanguageInclusionHoldDownwardsWithoutSim(const Type* lhs,
	const Type* rhs, timespec* start) const
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
		oper->GetIdentityRelation((lhs->getAutomaton()).get()));
	std::auto_ptr<InternalSimulationType> rhsSim(
		oper->GetIdentityRelation((rhs->getAutomaton()).get()));

	// convert automata to top-down
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		lhsTD(lhs->getAutomaton()->GetTopDownAutomaton());
	std::auto_ptr<typename NDSymbolicBUTreeAutomaton::NDSymbolicTDTreeAutomatonType>
		rhsTD(rhs->getAutomaton()->GetTopDownAutomaton());

	clock_gettime(CLOCK_THREAD_CPUTIME_ID, start);

	// check language inclusion
	std::auto_ptr<InternalOperationType> tdOper(lhsTD.get()->GetOperation());
	return tdOper->CheckLanguageInclusion(lhsTD.get(), rhsTD.get(), lhsSim.get(),
		rhsSim.get());
}
