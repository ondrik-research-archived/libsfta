/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file for VectorMap class.
 *
 *****************************************************************************/

#ifndef _VECTOR_MAP_HH_
#define _VECTOR_MAP_HH_


// Standard library headers
#include <tr1/unordered_map>


// Boost library headers
#include <boost/functional/hash.hpp>


// insert the class into proper namespace
namespace SFTA
{
	template
	<
		typename KeyElement,
		typename Value
	>
	class VectorMap;
}


template
<
	typename KeyElement,
	typename Value
>
class SFTA::VectorMap
{
public:   // Public data types

	typedef KeyElement KeyElementType;
	typedef Value ValueType;

	typedef VectorMap
		<
			KeyElementType,
			ValueType
		> Type;

	typedef typename SFTA::Vector<KeyElementType> IndexType;

	typedef std::pair<IndexType, ValueType> IndexValueType;

private:  // Private data types

	struct HasherUnary
	{
		size_t operator()(const KeyElementType& key) const
		{
			return boost::hash_value(key);
		}
	};

	typedef std::tr1::unordered_map<KeyElementType, ValueType, HasherUnary>
		HashTableUnary;


	typedef std::pair<KeyElementType, KeyElementType> KeyElementPairType;

	struct HasherBinary
	{
		size_t operator()(const KeyElementPairType& key) const
		{
			size_t seed  = 0;
			boost::hash_combine(seed, key.first);
			boost::hash_combine(seed, key.second);
			return seed;
		}
	};

	typedef std::tr1::unordered_map<KeyElementPairType, ValueType, HasherBinary>
		HashTableBinary;

	struct HasherNnary
	{
		size_t operator()(const IndexType& key) const
		{
			return boost::hash_range(key.begin(), key.end());
		}
	};

	typedef std::tr1::unordered_map<IndexType, ValueType, HasherNnary>
		HashTableNnary;

	struct Tconst_iterator
	{
	private:  // Private data members

		const Type* vecMap_;

		bool end_;

		IndexValueType indexValue_;

	private:  // Private methods

		void reset()
		{
			assert(false);
		}

	public:   // Public methods

		explicit Tconst_iterator(const Type* vecMap, bool end = false)
			: vecMap_(vecMap),
				end_(end),
				indexValue_()
		{
			// Assertions
			assert(vecMap_ != static_cast<const Type*>(0));

			reset();
			++(*this);
		}

		Tconst_iterator& operator++()
		{
			//TODO: increment
			assert(false);
			return *this;
		}

		bool operator==(const Tconst_iterator& rhs) const
		{
			// Assertions
			assert(vecMap_ != static_cast<Type*>(0));

			if (vecMap_ != rhs.vecMap_)
			{
				throw std::logic_error(__func__ +
					std::string(": attempt to compare iterators from different containers"));
			}

			assert(false);
		}

		inline bool operator!=(const Tconst_iterator& rhs) const
		{
			return !(*this == rhs);
		}

		const IndexValueType& operator*() const
		{
			return indexValue_;

		}

		const IndexValueType* operator->() const
		{
			return &indexValue_;
		}
	};

public:   // Public data types

	typedef Tconst_iterator const_iterator;

private:  // Private data members

	ValueType container0_;

	HashTableUnary container1_;

	HashTableBinary container2_;

	HashTableNnary containerN_;

	ValueType defaultValue_;

private:  // Private methods


	const ValueType& getRootForArity0(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 0);

		return container0_;
	}

	const ValueType& getRootForArity1(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 1);

		typename HashTableUnary::const_iterator it = container1_.end();
		if ((it = container1_.find(lhs[0])) == container1_.end())
		{	// in case the value is not in the hash table
			assert(false);
			return defaultValue_;
		}

		return it->second;
	}

	const ValueType& getRootForArity2(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 2);

		typename HashTableBinary::const_iterator it = container2_.end();
		if ((it = container2_.find(std::make_pair(lhs[0], lhs[1]))) == container2_.end())
		{	// in case the value is not in the hash table
			assert(false);
			return defaultValue_;
		}

		return it->second;
	}

	const ValueType& getRootForArityN(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() > 2);

		typename HashTableNnary::const_iterator it = containerN_.end();
		if ((it = containerN_.find(lhs)) == containerN_.end())
		{	// in case the key is not in the hash table
			assert(false);
			return defaultValue_;
		}

		return it->second;
	}


	void setRootForArity0(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 0);

		container0_ = value;
	}

	void setRootForArity1(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 1);

		container1_.insert(std::make_pair(lhs[0], value));
	}

	void setRootForArity2(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 2);

		container2_.insert(std::make_pair(std::make_pair(lhs[0],lhs[1]), value));
	}

	void setRootForArityN(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() > 2);

		containerN_.insert(std::make_pair(lhs, value));
	}

public:   // Public methods

	VectorMap()
		: container0_(),
			container1_(),
			container2_(),
			containerN_(),
			defaultValue_()
	{ }

	const ValueType& GetValue(const IndexType& index) const
	{
		switch (index.size())
		{
			case 0: return getRootForArity0(index); break;
			case 1: return getRootForArity1(index); break;
			case 2: return getRootForArity2(index); break;
			default: return getRootForArityN(index); break;
		}
	}

	void SetValue(const IndexType& index, const ValueType& value)
	{
		switch (index.size())
		{
			case 0: setRootForArity0(index, value); break;
			case 1: setRootForArity1(index, value); break;
			case 2: setRootForArity2(index, value); break;
			default: setRootForArityN(index, value); break;
		}
	}

	void insert(const VectorMap& vecMap)
	{
		// copy all vectors (without the nullary one)
		container1_.insert(vecMap.container1_.begin(), vecMap.container1_.end());
		container2_.insert(vecMap.container2_.begin(), vecMap.container2_.end());
		containerN_.insert(vecMap.containerN_.begin(), vecMap.containerN_.end());
	}

	inline void SetDefaultValue(const ValueType& val)
	{
		defaultValue_ = val;
	}

	const_iterator begin() const
	{
		return const_iterator(this);
	}

	const_iterator end() const
	{
		return const_iterator(this, true);
	}
};

#endif
