/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with TimbukBUTABuilder class.
 *
 *****************************************************************************/

#ifndef _SFTA_TIMBUK_BU_TA_BUILDER_HH_
#define _SFTA_TIMBUK_BU_TA_BUILDER_HH_

// Standard library header files
#include <vector>

// SFTA header files
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
		class BUTreeAutomaton
	>
	class TimbukBUTABuilder;
}


/**
 * @brief   Bottom-up tree automata builder from Timbuk format
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This class is used to build bottom-up tree automata from files that have
 * Timbuk format.
 *
 * @tparam  BUTreeAutomaton   Bottom-up tree automaton type.
 */
template
<
	class BUTreeAutomaton
>
class SFTA::TimbukBUTABuilder
	: public AbstractTABuilder
		<
			BUTreeAutomaton
		>
{
public:   // Public data types

	typedef BUTreeAutomaton BUTreeAutomatonType;

private:  // Private data types

	typedef typename BUTreeAutomatonType::LeftHandSideType LeftHandSideType;
	typedef typename BUTreeAutomatonType::RightHandSideType RightHandSideType;

	typedef SFTA::Private::Convert Convert;

public:   // Public methods 

	virtual void Build(std::istream& is, BUTreeAutomatonType* automaton) const
	{
		bool readingTransitions = false;
		std::string str;
		while (std::getline(is, str))
		{	// until we get to the end of the file
			boost::trim(str);

			boost::algorithm::replace_all(str, "  ", " ");
			boost::algorithm::replace_all(str, ", ", ",");

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
					throw std::runtime_error(__func__ +
						std::string("Unknown token in input stream: ") + Convert::ToString(str));
				}

				RightHandSideType rhs;
				rhs.insert(spl[2]);

				std::string leftSide = spl[0];
				size_t pos = leftSide.find('(');
				if (pos == leftSide.npos)
				{	// in case we are dealing with nullary symbol
					SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					automaton->AddTransition(LeftHandSideType(), spl[0], rhs);
				}
				else
				{	// in case we are not dealing with nullary symbol
					std::string symbol = leftSide.substr(0, pos);

					size_t endPos = leftSide.find(')');
					std::string stateStr = leftSide.substr(pos + 1, endPos - pos - 1);

					std::vector<std::string> states;

					boost::algorithm::split(states, stateStr, ispunct,
						boost::algorithm::token_compress_on);

					LeftHandSideType lhs;
					for (size_t i = 0; i < states.size(); ++i)
					{	// for each state
						lhs.push_back(states[i]);
					}

					SFTA_LOGGER_DEBUG("Adding transition: " + spl[0] + " -> " + spl[2]);

					automaton->AddTransition(lhs, symbol, rhs);
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

					automaton->AddState(stateName);
				}

				continue;
			}
			else if ((spl.size() >= 2) &&
				((spl[0] + " " + spl[1]) == "Final States"))
			{	// if we are reading final states
				for (size_t i = 2; i < spl.size(); ++i)
				{	// for each final state in the list
					SFTA_LOGGER_DEBUG("Setting state as final: " + spl[i]);

					automaton->SetStateFinal(spl[i]);
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
};

#endif
