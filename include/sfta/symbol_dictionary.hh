/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file with SymbolDictionary class.
 *
 *****************************************************************************/

#ifndef _SFTA_SYMBOL_DICTIONARY_HH_
#define _SFTA_SYMBOL_DICTIONARY_HH_

// Loki header files
#include <loki/SmartPtr.h>

// SFTA header files
#include <sfta/convert.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename InputSymbol,
		typename OutputSymbol
	>
	class SymbolDictionary;
}


template
<
	typename InputSymbol,
	typename OutputSymbol
>
class SFTA::SymbolDictionary
{
public:   // Public data types

	typedef InputSymbol InputSymbolType;
	typedef OutputSymbol OutputSymbolType;

private:  // Private data types

	typedef std::map<InputSymbolType, OutputSymbolType> I2AMapType;

	typedef SFTA::Private::Convert Convert;

private:  // Private data members

	I2AMapType i2a_;

	OutputSymbolType nextSymbol_;


	/**
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;


public:   // Public methods

	SymbolDictionary()
		: i2a_(),
			nextSymbol_()
	{ }


	OutputSymbolType TranslateI2A(const InputSymbolType& symbol)
	{
		typename I2AMapType::const_iterator it;
		if ((it = i2a_.find(symbol)) == i2a_.end())
		{	// in case a new symbol appeared
			OutputSymbolType newSymbol = nextSymbol_;
			i2a_.insert(std::make_pair(symbol, newSymbol));

			//SFTA_LOGGER_DEBUG("Adding new symbol: " + Convert::ToString(symbol)
			//	+ " -> " + Convert::ToString(newSymbol));

			++nextSymbol_;
			return newSymbol;
		}

		return it->second;
	}

};

// Setting the logging category name for Log4cpp
template
<
	typename I,
	typename O
>
const char* SFTA::SymbolDictionary<I, O>::LOG_CATEGORY_NAME = "symbol_dictionary";

#endif
