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


/**
 * @brief   Tree automaton building director
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * Building director for tree automata. It is used as an intermediate layer
 * between tree automata builders and code that demands building.
 *
 * @tparam  TreeAutomaton   The tree automaton type.
 */
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

private:  // Private data types

	typedef typename TreeAutomatonType::SymbolDictionaryType
		SymbolDictionaryType;

	typedef typename TreeAutomatonType::SymbolDictionaryPtrType
		SymbolDictionaryPtrType;

private:  // Private data members

	TreeAutomatonType defaultTa_;

	AbstractTABuilderType* builder_;

	SymbolDictionaryPtrType symbolDic_;


private:  // Private methods

	TABuildingDirector(const TABuildingDirector& director);
	TABuildingDirector& operator=(const TABuildingDirector& rhs);

public:   // Public methods


	explicit TABuildingDirector(AbstractTABuilderType* builder)
		: defaultTa_(64 /* TODO: horrible constant */),
			builder_(builder),
			symbolDic_(new SymbolDictionaryType())
	{ }


	TreeAutomatonType* Construct(std::istream& is)
	{
		TreeAutomatonType* result = new TreeAutomatonType(defaultTa_.GetBDDSize(),
			defaultTa_.GetTTWrapper(), symbolDic_);

		builder_->Build(is, result);

		return result;
	}

};


#endif
