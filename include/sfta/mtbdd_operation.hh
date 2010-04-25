/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with the MTBDDOperation class.
 *
 *****************************************************************************/

#ifndef _SFTA_MTBDD_OPERATION_HH_
#define _SFTA_MTBDD_OPERATION_HH_

// SFTA header files
#include <sfta/abstract_operation.hh>

// Standard library header files
#include <algorithm>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class MTBDDOperation;
}


template
<
	class TreeAutomaton
>
class SFTA::MTBDDOperation
	: public SFTA::AbstractOperation<TreeAutomaton>
{
private:  // Private data types

	typedef typename TreeAutomaton::TransFuncPtrType TransFuncPtrType;

	typedef typename TreeAutomaton::TransitionFunctionType TransitionFunctionType;

	typedef typename TransitionFunctionType::StateType StateType;

	typedef typename TransitionFunctionType::LeftHandSideType LeftHandSideType;

	typedef typename TransitionFunctionType::MTBDDType MTBDDType;

	typedef typename MTBDDType::RootType RootType;

	typedef typename MTBDDType::LeafType LeafType;


private:  // Private methods

	static LeafType leafUnion(const LeafType& lhs, const LeafType& rhs)
	{
		return lhs.Union(rhs);
	}


public:   // Public methods

	virtual TreeAutomaton Union(TreeAutomaton& ta1, TreeAutomaton& ta2) const
	{
		// Assertions
		assert(ta1.GetTransitionFunction() == ta2.GetTransitionFunction());

		TransFuncPtrType transFunc = ta1.GetTransitionFunction();

		MTBDDType& mtbdd = transFunc->getMTBDD();

		LeftHandSideType lhs;

		LeftHandSideType newLhs;

		RootType root = mtbdd.Apply(transFunc->getRoot(ta1.GetRegToken(), lhs), transFunc->getRoot(ta2.GetRegToken(), lhs), leafUnion);

		TreeAutomaton result(transFunc);

		transFunc->setRoot(result.GetRegToken(), newLhs, root);

		mtbdd.DumpToDotFile("mtbdd.dot");


		return result;
	}
};

#endif
