/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with AbstractTABuilder class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_TA_BUILDER_HH_
#define _SFTA_ABSTRACT_TA_BUILDER_HH_


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class AbstractTABuilder;

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
class SFTA::AbstractTABuilder
{
public:   // Public data types

	typedef typename TreeAutomaton::AStateType StateType;
	typedef typename TreeAutomaton::ASymbolType SymbolType;
	typedef typename TreeAutomaton::RuleLeftHandSideType RuleLeftHandSideType;
	typedef typename TreeAutomaton::SetOfStates SetOfStates;

	typedef void (TABuildingDirector::*AddStateFunctionType)
		(const StateType&);

	typedef void (TABuildingDirector::*AddTransitionFunctionType)
		(const SymbolType&, const RuleLeftHandSideType&, const SetOfStates&);

	typedef void (TABuildingDirector::*SetStateFinalFunctionType)
		(const StateType&);


public:   // Public methods

	void Build(istream& is) const = 0;

	void RegisterAddStateFunction(AddStateFunctionType func) = 0;

	void RegisterAddTransitionFunction(AddTransitionFunctionType func) = 0;

	void RegisterSetStateFinalFunction(SetStateFinalFunctionType func) = 0;
};

#endif
