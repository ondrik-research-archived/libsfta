/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with TABuildingDirector class.
 *
 *****************************************************************************/

#ifndef _SFTA_TA_BUILDING_DIRECTOR_HH_
#define _SFTA_TA_BUILDING_DIRECTOR_HH_

// SFTA header files
#include <sfta/abstract_ta_builder.hh>

// Loki header files
#include <loki/SmartPtr.h>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class TABuildingDirector;
}


template
<
	class TreeAutomaton
>
class SFTA::TABuildingDirector
{
public:   // Public data types

	typedef TreeAutomaton TreeAutomatonType;

	typedef SFTA::AbstractTABuilder<TreeAutomaton> AbstractTABuilderType;

	typedef Loki::SmartPtr<AbstractTABuilderType, Loki::DestructiveCopy>
		AbsTABPtr;

private:  // Private data types

	typedef typename TreeAutomatonType::AStateType StateType;
	typedef typename TreeAutomatonType::SymbolType SymbolType;
	typedef typename TreeAutomatonType::RuleLeftHandSideType RuleLeftHandSideType;
	typedef typename TreeAutomatonType::SetOfStatesType SetOfStatesType;
	typedef typename TreeAutomatonType::TransFuncPtrType TransFuncPtrType;

	typedef TABuildingDirector<TreeAutomatonType> DirectorType;

private:  // Private data members

	TreeAutomatonType ta_;

	AbsTABPtr builder_;

	bool hasBeenTaken_;

private:  // Private methods

	void addState(const StateType& state)
	{
		ta_.AddState(state);
	}

	void addTransition(const SymbolType& symbol, const RuleLeftHandSideType& lhs,
		const SetOfStatesType& rhs)
	{
		ta_.AddTransition(symbol, lhs, rhs);
	}

	void setStateFinal(const StateType& state)
	{
		ta_.SetStateFinal(state);
	}

public:   // Public methods

	explicit TABuildingDirector(AbsTABPtr builder)
		: ta_(), builder_(builder), hasBeenTaken_(false)
	{
		builder_->RegisterDirector(this);
		builder_->RegisterAddStateFunction(&DirectorType::addState);
		builder_->RegisterAddTransitionFunction(&DirectorType::addTransition);
		builder_->RegisterSetStateFinalFunction(&DirectorType::setStateFinal);
	}

	TABuildingDirector(AbsTABPtr builder, TransFuncPtrType transFunc)
		: ta_(transFunc), builder_(builder), hasBeenTaken_(false)
	{
		builder_->RegisterDirector(this);
		builder_->RegisterAddStateFunction(&DirectorType::addState);
		builder_->RegisterAddTransitionFunction(&DirectorType::addTransition);
		builder_->RegisterSetStateFinalFunction(&DirectorType::setStateFinal);
	}

	TreeAutomatonType Construct(std::istream& is)
	{
		if (hasBeenTaken_)
		{	// in case the tree automaton has already been taken
			throw std::runtime_error("An attempt to use building director twice.");
		}

		builder_->Build(is);

		hasBeenTaken_ = true;

		return ta_;
	}

};


#endif
