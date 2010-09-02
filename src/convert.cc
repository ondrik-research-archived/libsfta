/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Convert class implementation.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/convert.hh>


namespace SFTA
{
	namespace Private
	{
		template <>
		std::string Convert::ToString<unsigned char>(const unsigned char& n)
		{
			// the output stream for the string
			std::ostringstream oss;
			// insert the object into the stream
			oss << static_cast<unsigned>(n);
			// return the string
			return oss.str();
		}
	}
}
