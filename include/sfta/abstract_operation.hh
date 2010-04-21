/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with the AbstractOperation abstract class.
 *
 *****************************************************************************/

#ifndef _SFTA_ABSTRACT_OPERATION_HH_
#define _SFTA_ABSTRACT_OPERATION_HH_

#include <sfta/mtbdd_transition_function.hh>

// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class AbstractOperation;
}


template
<
	class TreeAutomaton
>
class SFTA::AbstractOperation
{
private:  // Private data types

	typedef typename TreeAutomaton::TransitionFunctionType TransitionFunctionType;


public:   // Public methods

	virtual TreeAutomaton Union(TreeAutomaton& ta1, TreeAutomaton& ta2) const = 0;

	virtual ~AbstractOperation()
	{ }
};


#endif
