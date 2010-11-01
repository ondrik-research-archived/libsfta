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
}


template
<
	class TreeAutomaton
>
class SFTA::AbstractTABuilder
{
public:   // Public data types

	typedef TreeAutomaton TreeAutomatonType;

public:

	virtual void Build(std::istream& is, TreeAutomatonType* automaton) const = 0;

	virtual ~AbstractTABuilder()
	{ }
};

#endif
