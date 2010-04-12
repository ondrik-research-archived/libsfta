/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the MTBDDTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _MTBDD_TRANSITION_FUNCTION_HH_
#define _MTBDD_TRANSITION_FUNCTION_HH_

// SFTA headers
#include <sfta/abstract_transition_function.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename LeftHandSide,
		typename RightHandSide,
		class MTBDD
	>
	class MTBDDTransitionFunction;
}


template
<
	typename Symbol,
	typename LeftHandSide,
	typename RightHandSide,
	class MTBDD
>
class SFTA::MTBDDTransitionFunction
	: public AbstractTransitionFunction
		<
			Symbol,
			LeftHandSide,
			RightHandSide
		>
{
public:   // Public data types

	typedef Symbol SymbolType;
	typedef LeftHandSide LeftHandSideType;
	typedef RightHandSide RightHandSideType;
	typedef MTBDD MTBDDType;

public:   // Public methods


	virtual void AddTransition(const SymbolType& symbol, const LeftHandSideType& lhs,
		const RightHandSideType& rhs)
	{
	}


	virtual RightHandSideType GetTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs) const
	{
		// TODO:
		return RightHandSideType();
	}

	virtual ~MTBDDTransitionFunction()
	{ }

};

#endif
