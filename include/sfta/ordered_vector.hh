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
		class Key
	>
	class OrderedVector;
}


/**
 * @brief  Implementation of a set using ordered vector
 *
 * This class implements the interface of a set (the same interface as
 * std::set) using ordered vector as the underlying data structure.
 *
 * @tparam  Key  Key type: type of the elements contained in the container.
 *               Each elements in a set is also its key.
 */
template
<
	class Key
>
class SFTA::OrderedVector
{
private:  // Private data types

	typedef SFTA::Private::Convert Convert;

	typedef std::vector<Key> VectorType;

public:   // Public data types

	typedef typename VectorType::iterator iterator;
	typedef typename VectorType::const_iterator const_iterator;
	typedef typename VectorType::const_reference const_reference;

private:  // Private data members

	VectorType vec_;


private:  // Private methods

	bool vectorIsSorted() const
	{
		for (typename VectorType::const_iterator itVec = vec_.begin() + 1;
			itVec < vec_.end(); ++itVec)
		{	// check that the vector is sorted
			if (!(*(itVec - 1) < *itVec))
			{	// in case there is an unordered pair (or there is one element twice)
				return false;
			}
		}

		return true;
	}


public:   // Public methods

	OrderedVector()
		: vec_()
	{
		// Assertions
		assert(vectorIsSorted());
	}


	explicit OrderedVector(const VectorType& vec)
		: vec_(vec)
	{
		// sort
		std::sort(vec_.begin(), vec_.end());

		// remove duplicates
		typename VectorType::iterator it = std::unique(vec_.begin(), vec_.end());
		vec_.resize(it - vec_.begin());

		// Assertions
		assert(vectorIsSorted());
	}

	OrderedVector& operator=(const OrderedVector& rhs)
	{
		// Assertions
		assert(rhs.vectorIsSorted());

		if (&rhs != this)
		{
			vec_ = rhs.vec_;
		}

		// Assertions
		assert(vectorIsSorted());

		return *this;
	}


	void insert(const Key& x)
	{
		// Assertions
		assert(vectorIsSorted());

		// perform binary search (cannot use std::binary_search because it is
		// ineffective due to not returning the iterator to the position of the
		// desirable insertion in case the searched element is not present in the
		// range)
		size_t first = 0;
		size_t last = vec_.size();

		if ((last != 0) && (vec_[last-1] < x))
		{	// for the case which would be prevalent
			vec_.push_back(x);
			return;
		}

		while (first < last)
		{	// while the pointers do not overlap
			size_t middle = first + (last - first) / 2;
			if (vec_[middle] == x)
			{	// in case we found x
				return;
			}
			else if (vec_[middle] < x)
			{	// in case middle is less than x
				first = middle + 1;
			}
			else
			{	// in case middle is greater than x
				last = middle;
			}
		}

		vec_.resize(vec_.size() + 1);
		std::copy_backward(vec_.begin() + first, vec_.end() - 1, vec_.end());

		// insert the new element
		vec_[first] = x;

		// Assertions
		assert(vectorIsSorted());
	}


	void insert(const OrderedVector& vec)
	{
		// Assertions
		assert(vectorIsSorted());
		assert(vec.vectorIsSorted());

		OrderedVector result = this->Union(vec);
		vec_ = result.vec_;

		// Assertions
		assert(vectorIsSorted());
	}


	inline void clear()
	{
		// Assertions
		assert(vectorIsSorted());

		vec_.clear();
	}


	inline size_t size() const
	{
		// Assertions
		assert(vectorIsSorted());

		return vec_.size();
	}


	OrderedVector Union(const OrderedVector& rhs) const
	{
		// Assertions
		assert(vectorIsSorted());
		assert(rhs.vectorIsSorted());

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
				else if (*rhsIt < *lhsIt)
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

		OrderedVector result(newVector);

		// Assertions
		assert(result.vectorIsSorted());

		return result;
	}

	const_iterator find(const Key& key) const
	{
		// Assertions
		assert(vectorIsSorted());

		size_t first = 0;
		size_t last = vec_.size();

		while (first < last)
		{	// while the pointers do not overlap
			size_t middle = first + (last - first) / 2;
			if (vec_[middle] == key)
			{	// in case we found x
				return const_iterator(&vec_[middle]);
			}
			else if (vec_[middle] < key)
			{	// in case middle is less than x
				first = middle + 1;
			}
			else
			{	// in case middle is greater than x
				last = middle;
			}
		}

		return end();
	}


	inline bool empty() const
	{
		// Assertions
		assert(vectorIsSorted());

		return vec_.empty();
	}

	inline const_iterator begin() const
	{
		// Assertions
		assert(vectorIsSorted());

		return vec_.begin();
	}

	inline const_iterator end() const
	{
		// Assertions
		assert(vectorIsSorted());

		return vec_.end();
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
		// Assertions
		assert(vec.vectorIsSorted());

		std::string result = "{";

		for (typename VectorType::const_iterator it = vec.begin();
			it != vec.end(); ++it)
		{
			result += ((it != vec.begin())? ", " : " ") + Convert::ToString(*it);
		}

		return os << (result + "}");
	}

	bool operator==(const OrderedVector& rhs) const
	{
		// Assertions
		assert(vectorIsSorted());
		assert(rhs.vectorIsSorted());

		return (vec_ == rhs.vec_);
	}

	bool operator<(const OrderedVector& rhs) const
	{
		// Assertions
		assert(vectorIsSorted());
		assert(rhs.vectorIsSorted());

		return std::lexicographical_compare(vec_.begin(), vec_.end(),
			rhs.vec_.begin(), rhs.vec_.end());
	}
};

#endif
