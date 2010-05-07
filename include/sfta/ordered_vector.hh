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


	OrderedVector()
		: vec_()
	{
	}

	explicit OrderedVector(const VectorType& vec)
		: vec_(vec)
	{
	}


	inline void push_back(const T& x)
	{
		if (std::find(vec_.begin(), vec_.end(), x) == vec_.end())
		{	// in case the element is not in the vector so far
			vec_.push_back(x);
			std::sort(vec_.begin(), vec_.end());
		}
	}


	inline void clear()
	{
		vec_.clear();
	}

	inline size_t size() const
	{
		return vec_.size();
	}

	OrderedVector Union(const OrderedVector& rhs) const
	{
		VectorType newVector;

		typename VectorType::const_iterator lhsIt = vec_.begin();
		typename VectorType::const_iterator rhsIt = rhs.vec_.begin();

		while ((lhsIt != vec_.end()) || (rhsIt != rhs.vec_.end()))
		{	// until we get to the end of both vectors
			if (lhsIt == vec_.end())
			{	// if we are finished with the left-hand side vector
				newVector.push_back(*rhsIt);
				++rhsIt;
			}
			else if (rhsIt == rhs.vec_.end())
			{	// if we are finished with the right-hand side vector
				newVector.push_back(*lhsIt);
				++lhsIt;
			}
			else
			{
				if (*lhsIt < *rhsIt)
				{
					newVector.push_back(*lhsIt);
					++lhsIt;
				}
				else if (*lhsIt > *rhsIt)
				{
					newVector.push_back(*rhsIt);
					++rhsIt;
				}
				else
				{	// in case they are equal
					newVector.push_back(*rhsIt);
					++rhsIt;
					++lhsIt;
				}
			}
		}

		return OrderedVector(newVector);
	}


	template <class InputIterator>
	inline void insert(iterator position, InputIterator first, InputIterator last)
	{
		vec_.insert(position, first, last);
		std::sort(vec_.begin(), vec_.end());
	}


	inline void Erase(size_t position)
	{
		// Assertions
		assert(position < vec_.size());

		vec_.erase(vec_.begin() + position);
	}

	inline iterator begin()
	{
		return vec_.begin();
	}

	inline bool empty() const
	{
		return vec_.empty();
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
