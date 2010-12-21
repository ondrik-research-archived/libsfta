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

	typedef std::map<InputSymbolType, OutputSymbolType> I2OMapType;
	typedef std::map<OutputSymbolType, InputSymbolType> O2IMapType;

	typedef SFTA::Private::Convert Convert;

private:  // Private data members

	I2OMapType i2o_;
	O2IMapType o2i_;

	OutputSymbolType nextSymbol_;

public:   // Public methods


	SymbolDictionary()
		: i2o_(),
			o2i_(),
			nextSymbol_(0)
	{ }


	OutputSymbolType Translate(const InputSymbolType& symbol)
	{
		typename I2OMapType::const_iterator itSymbol;
		if ((itSymbol = i2o_.find(symbol)) == i2o_.end())
		{	// in case a new symbol appeared
			OutputSymbolType newSymbol = nextSymbol_;
			++nextSymbol_;

			if (!(i2o_.insert(std::make_pair(symbol, newSymbol))).second)
			{
				throw std::runtime_error(__func__ +
					std::string(": inserting already existing translation ") +
					Convert::ToString(symbol) + " -> " + Convert::ToString(newSymbol));
			}

			if (!(o2i_.insert(std::make_pair(newSymbol, symbol))).second)
			{
				throw std::runtime_error(__func__ +
					std::string(": inserting already existing inverse translation ") +
					Convert::ToString(newSymbol) + " -> " + Convert::ToString(symbol));
			}

			return newSymbol;
		}

		return itSymbol->second;
	}


	std::vector<InputSymbolType> GetVectorOfInputSymbols() const
	{
		std::vector<InputSymbolType> result;

		for (typename I2OMapType::const_iterator itSymbols = i2o_.begin();
			itSymbols != i2o_.end(); ++itSymbols)
		{
			result.push_back(itSymbols->first);
		}

		return result;
	}


	InputSymbolType TranslateInverse(const OutputSymbolType& symbol)
	{
		typename O2IMapType::const_iterator itSymbol;
		if ((itSymbol = o2i_.find(symbol)) == o2i_.end())
		{	// in case a new symbol appeared
			throw std::runtime_error(__func__ +
				std::string(": invalid translation from ") + Convert::ToString(symbol));
		}

		return itSymbol->second;
	}

};

#endif
