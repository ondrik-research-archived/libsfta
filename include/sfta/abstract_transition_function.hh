/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the AbstractTransitionFunction class.
 *
 *****************************************************************************/

#ifndef _ABSTRACT_TRANSITION_FUNCTION_HH_
#define _ABSTRACT_TRANSITION_FUNCTION_HH_


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename Symbol,
		typename LeftHandSide,
		typename RightHandSide
	>
	class AbstractTransitionFunction;
}


template
<
	typename Symbol,
	typename LeftHandSide,
	typename RightHandSide
>
class SFTA::AbstractTransitionFunction
{
public:   // Public data types

	typedef Symbol SymbolType;
	typedef LeftHandSide LeftHandSideType;
	typedef RightHandSide RightHandSideType;

public:   // Public methods

	virtual void AddTransition(const SymbolType& symbol, const LeftHandSideType& lhs,
		const RightHandSideType& rhs) = 0;


	virtual RightHandSideType GetTransition(const SymbolType& symbol,
		const LeftHandSideType& lhs) const = 0;

	virtual ~AbstractTransitionFunction()
	{ }


};


#endif
