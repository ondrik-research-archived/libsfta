/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    The file that contains the InflatableVector class.
 *
 *****************************************************************************/

#ifndef _SFTA_INFLATABLE_VECTOR_HH_
#define _SFTA_INFLATABLE_VECTOR_HH_

// SFTA header files
#include <sfta/convert.hh>
#include <sfta/vector.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename T
	>
	class InflatableVector;
}


/**
 * @brief   STL std::vector container which is transparently inflatable
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is a specialization of STL std::vector container that is transparently
 * inflatable. This means that when there is an access to a position which is
 * out of bounds, the bounds are broadened to this position with default
 * elements.
 *
 * @tparam  T   Contained data type.
 */
template
<
	typename T
>
class SFTA::InflatableVector
	: public SFTA::Vector<T>
{
private:   // Private data types

	typedef SFTA::Vector<T> ParentClass;

	typedef SFTA::Private::Convert Convert;

public:   // Public data types

	typedef typename ParentClass::reference reference;
	typedef typename ParentClass::const_reference const_reference;

private:  // Private methods

	inline void inflateTo(size_t n)
	{
		while (n >= ParentClass::size())
		{
			push_back(T());
		}
	}

public:   // Public methods

	InflatableVector()
		: ParentClass()
	{ }

	explicit InflatableVector(size_t n, const T& value = T())
		: ParentClass(n, value)
	{ }

	reference operator[](size_t n)
	{
		return at(n);
	}

	const_reference operator[](size_t n) const
	{
		return at(n);
	}

	reference at(size_t n)
	{
		inflateTo(n);

		return ParentClass::at(n);
	}

	const_reference at(size_t n) const
	{
		inflateTo(n);

		return ParentClass::at(n);
	}
};

#endif
