/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with TimbukTABuilder class.
 *
 *****************************************************************************/

#ifndef _SFTA_TIMBUK_TA_BUILDER_HH_
#define _SFTA_TIMBUK_TA_BUILDER_HH_

// Standard library header files
#include <vector>

// SFTA header files
#include <sfta/sfta.hh>
#include <sfta/abstract_ta_builder.hh>
#include <sfta/convert.hh>

// Boost header files
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/split.hpp>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		class TreeAutomaton
	>
	class TimbukTABuilder;
}


template
<
	class TreeAutomaton
>
class SFTA::TimbukTABuilder
	: public AbstractTABuilder
	  <
			TreeAutomaton
		>
{
private:  // Private data types

	typedef AbstractTABuilder<TreeAutomaton> ParentClass;

	typedef SFTA::Private::Convert Convert;

public:   // Public data types

	typedef typename ParentClass::DirectorType DirectorType;
	typedef typename ParentClass::AddStateFunctionType AddStateFunctionType;
	typedef typename ParentClass::AddTransitionFunctionType
		AddTransitionFunctionType;
	typedef typename ParentClass::SetStateFinalFunctionType
		SetStateFinalFunctionType;
	typedef typename ParentClass::SymbolDictionaryType SymbolDictionaryType;
	typedef typename ParentClass::SymbolDictionaryPtr SymbolDictionaryPtr;

	typedef TreeAutomaton TreeAutomatonType;

	typedef typename SymbolDictionaryType::InputSymbolType InputSymbolType;
	typedef typename TreeAutomatonType::SymbolType AutomatonSymbolType;
	typedef typename TreeAutomatonType::RuleLeftHandSideType
		RuleLeftHandSideType;
	typedef typename TreeAutomatonType::SetOfStatesType SetOfStatesType;

private:  // Private data members


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;

public:   // Public methods 

	TimbukTABuilder(SymbolDictionaryPtr dict)
		: ParentClass(dict)
	{ }

	virtual void Build(std::istream& is)
	{
		// Assertions
		assert(ParentClass::director_
			!= static_cast<DirectorType*>(0));
		assert(ParentClass::addStateFunction_
			!= static_cast<AddStateFunctionType>(0));
		assert(ParentClass::addTransitionFunction_
			!= static_cast<AddTransitionFunctionType>(0));
		assert(ParentClass::setStateFinalFunction_
			!= static_cast<SetStateFinalFunctionType>(0));

		bool readingTransitions = false;
		std::string str;
		while (std::getline(is, str))
		{	// until we get to the end of the file
			boost::trim(str);

			std::vector<std::string> spl;
			boost::algorithm::split(spl, str, isspace,
				boost::algorithm::token_compress_on);

			if ((spl.size() == 0) || ((spl.size() == 1) && spl[0] == ""))
			{	// if we read an empty line
				continue;
			}
			else if (readingTransitions)
			{	// in case we are reading transitions
				if (spl.size() != 3)
				{	// if the format is wrong
					throw std::runtime_error("Unknown token in input stream");
				}

				SetOfStatesType rhs;
				rhs.insert(spl[2]);

				std::string leftSide = spl[0];
				size_t pos = leftSide.find('(');
				if (pos == leftSide.npos)
				{	// in case we are dealing with nullary symbol
					AutomatonSymbolType symbol
						= ParentClass::dict_->TranslateI2A(leftSide);

					RuleLeftHandSideType lhs;

					SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					(ParentClass::director_->*ParentClass::addTransitionFunction_)
						(symbol, lhs, rhs);
				}
				else
				{	// in case we are not dealing with nullary symbol
					std::string symbolName = leftSide.substr(0, pos);

					AutomatonSymbolType symbol
						= ParentClass::dict_->TranslateI2A(symbolName);

					size_t endPos = leftSide.find(')');
					std::string stateStr = leftSide.substr(pos + 1, endPos - pos - 1);

					std::vector<std::string> states;

					boost::algorithm::split(states, stateStr, ispunct,
						boost::algorithm::token_compress_on);

					RuleLeftHandSideType lhs;
					for (size_t i = 0; i < states.size(); ++i)
					{	// for each state
						lhs.push_back(states[i]);
					}

					//SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					(ParentClass::director_->*ParentClass::addTransitionFunction_)
						(symbol, lhs, rhs);
				}

				continue;
			}
			else if (spl[0] == "Ops")
			{	// we dispose of definition of arity for operations
				continue;
			}
			else if (spl[0] == "Automaton")
			{	// we are not interested in the name of the automaton
				continue;
			}
			else if (spl[0] == "States")
			{	// we are reading states
				for (size_t i = 1; i < spl.size(); ++i)
				{	// for each state in the list
					std::string stateName = spl[i];
					size_t pos = stateName.find(':');
					stateName.erase(pos);
					SFTA_LOGGER_DEBUG("Adding state: " + stateName);

					(ParentClass::director_->*ParentClass::addStateFunction_)(stateName);
				}

				continue;
			}
			else if ((spl.size() >= 2) &&
				((spl[0] + " " + spl[1]) == "Final States"))
			{	// if we are reading final states
				for (size_t i = 2; i < spl.size(); ++i)
				{	// for each final state in the list
					SFTA_LOGGER_DEBUG("Setting state as final: " + spl[i]);

					(ParentClass::director_->*ParentClass::setStateFinalFunction_)
						(spl[i]);
				}

				continue;
			}
			else if (spl[0] == "Transitions")
			{	// if we are reading transitions
				readingTransitions = true;

				continue;
			}
			else
			{	// unknown token
				throw std::runtime_error("Unknown token in input stream");
			}
		}
	}


	virtual ~TimbukTABuilder()
	{ }
};


// Setting the logging category name for Log4cpp
template
<
	class TA
>
const char* SFTA::TimbukTABuilder<TA>::LOG_CATEGORY_NAME
	= "timbuk_ta_builder";

#endif
