/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with OrderedVector class.
 *
 *****************************************************************************/

#ifndef _SFTA_ORDERED_VECTOR_HH_
#define _SFTA_ORDERED_VECTOR_HH_

// Standard library header files
#include <vector>
#include <algorithm>

// SFTA header files
#include <sfta/convert.hh>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename T
	>
	class OrderedVector;
}


template
<
	typename T
>
class SFTA::OrderedVector
{
private:  // Private data types

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<T> VectorType;

public:   // Public data types

	typedef typename VectorType::iterator iterator;
	typedef typename VectorType::const_iterator const_iterator;
	typedef typename VectorType::const_reference const_reference;

private:  // Private data members

	VectorType vec_;


public:   // Public methods

	inline void push_back(const T& x)
	{
		vec_.push_back(x);
		std::sort(vec_.begin(), vec_.end());
	}


	inline void clear()
	{
		vec_.clear();
	}


	template <class InputIterator>
	inline void insert(iterator position, InputIterator first, InputIterator last)
	{
		vec_.insert(position, first, last);
		std::sort(vec_.begin(), vec_.end());
	}

	inline iterator begin()
	{
		return vec_.begin();
	}

	inline const_iterator begin() const
	{
		return vec_.begin();
	}

	inline iterator end()
	{
		return vec_.end();
	}

	inline const_iterator end() const
	{
		return vec_.end();
	}

	inline const_reference operator[](size_t n) const
	{
		return vec_[n];
	}

	/**
	 * @brief  Overloaded << operator
	 *
	 * Overloaded << operator for output stream.
	 *
	 * @see  ToString()
	 *
	 * @param[in]  os    The output stream
	 * @param[in]  asgn  Assignment to the variables
	 *
	 * @returns  Modified output stream
	 */
	friend std::ostream& operator<<(std::ostream& os, const OrderedVector& vec)
	{
		return (os << Convert::ToString(vec.vec_));
	}

	bool operator==(const OrderedVector& rhs) const
	{
		return (vec_ == rhs.vec_);
	}

};

#endif
