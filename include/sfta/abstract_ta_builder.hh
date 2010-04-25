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

// Standard library header files
#include <istream>

// SFTA header files
#include <sfta/symbol_dictionary.hh>

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

	typedef TreeAutomaton TreeAutomatonType;


	typedef typename TreeAutomatonType::AStateType StateType;
	typedef typename TreeAutomatonType::SymbolType SymbolType;
	typedef typename TreeAutomatonType::RuleLeftHandSideType RuleLeftHandSideType;
	typedef typename TreeAutomatonType::SetOfStatesType SetOfStatesType;

	typedef TABuildingDirector<TreeAutomatonType> DirectorType;

	typedef void (DirectorType::*AddStateFunctionType) (const StateType&);

	typedef void (DirectorType::*AddTransitionFunctionType)
		(const SymbolType&, const RuleLeftHandSideType&, const SetOfStatesType&);

	typedef void (DirectorType::*SetStateFinalFunctionType) (const StateType&);

	typedef SFTA::SymbolDictionary<std::string,
		typename TreeAutomatonType::SymbolType> SymbolDictionaryType;

	typedef Loki::SmartPtr<SymbolDictionaryType> SymbolDictionaryPtr;

protected:// Protected data members

	DirectorType* director_;
	AddStateFunctionType addStateFunction_;
	AddTransitionFunctionType addTransitionFunction_;
	SetStateFinalFunctionType setStateFinalFunction_;

	SymbolDictionaryPtr dict_;

private:  // Private methods

	AbstractTABuilder(const AbstractTABuilder& builder);
	AbstractTABuilder& operator=(const AbstractTABuilder& builder);

public:   // Public methods

	AbstractTABuilder(SymbolDictionaryPtr dict)
		: director_(static_cast<DirectorType*>(0)),
		  addStateFunction_(static_cast<AddStateFunctionType>(0)),
			addTransitionFunction_(static_cast<AddTransitionFunctionType>(0)),
			setStateFinalFunction_(static_cast<SetStateFinalFunctionType>(0)),
			dict_(dict)
	{ }

	virtual void Build(std::istream& is) = 0;

	inline void RegisterDirector(DirectorType* director)
	{
		director_ = director;
	}

	inline void RegisterAddStateFunction(AddStateFunctionType func)
	{
		addStateFunction_ = func;
	}

	inline void RegisterAddTransitionFunction(AddTransitionFunctionType func)
	{
		addTransitionFunction_ = func;
	}

	inline void RegisterSetStateFinalFunction(SetStateFinalFunctionType func)
	{
		setStateFinalFunction_ = func;
	}

	virtual ~AbstractTABuilder()
	{ }
};

#endif
