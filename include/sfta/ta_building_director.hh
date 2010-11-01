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

	typedef TABuildingDirector<TreeAutomaton> Type;

	typedef TreeAutomaton TreeAutomatonType;

	typedef SFTA::AbstractTABuilder<TreeAutomatonType> AbstractTABuilderType;

private:  // Private data members

	TreeAutomatonType defaultTa_;

	AbstractTABuilderType* builder_;

public:   // Public methods


	TABuildingDirector(AbstractTABuilderType* builder)
		: defaultTa_(),
			builder_(builder)
	{

	}


	TreeAutomatonType* Construct(std::istream& is)
	{
		TreeAutomatonType* result = new TreeAutomatonType(defaultTa_.GetTTWrapper());

		builder_->Build(is, result);

		return result;
	}

};


#endif
