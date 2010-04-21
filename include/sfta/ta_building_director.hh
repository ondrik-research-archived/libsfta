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
class TABuildingDirector
{
public:   // Public data types

	typedef TreeAutomaton TreeAutomatonType;

private:  // Private data members

	TreeAutomatonType ta_;

public   // Public methods

	TABuildingDirector()
		: ta_()
	{ }

	TreeAutomatonType Construct(std::istream& is)
	{
	}

};


#endif
