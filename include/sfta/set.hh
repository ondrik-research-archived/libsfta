/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file that contains the Set class.
 *
 *****************************************************************************/

#ifndef _SFTA_SET_HH_
#define _SFTA_SET_HH_

// SFTA header files
#include <sfta/convert.hh>

// Standard library header files
#include <set>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename T
	>
	class Set;
}


template
<
	typename T
>
class SFTA::Set
	: public std::set<T>
{
private:   // Private data types

	typedef std::set<T> ParentClass;

	typedef SFTA::Private::Convert Convert;

public:   // Public methods

	Set()
		: ParentClass()
	{ }


	/**
	 * @brief  Overloaded << operator
	 *
	 * Overloaded << operator for output stream.
	 *
	 * @see  ToString()
	 *
	 * @param[in]  os   The output stream
	 * @param[in]  vec  The vector
	 *
	 * @returns  Modified output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const Set<T>& set)
	{
		return (os << Convert::ToString(static_cast<std::set<T> >(set)));
	}
};

#endif
