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
	private:  // Private data types

		enum IteratorState
		{
			ITERATOR_INVALID = 0,
			ITERATOR_NULLARY,
			ITERATOR_UNARY,
			ITERATOR_BINARY,
			ITERATOR_NNARY,
			ITERATOR_END
		};


	private:  // Private data members

		const Type* vecMap_;

		IndexValueType indexValue_;

		IteratorState state_;

		typename HashTableUnary::const_iterator itUnary_;
		typename HashTableBinary::const_iterator itBinary_;
		typename HashTableNnary::const_iterator itNnary_;

	private:  // Private methods

		void reset()
		{
			state_ = ITERATOR_INVALID;
		}

		void updateIndexValue()
		{
			// Assertions
			assert(state_ != ITERATOR_INVALID);
			assert(state_ != ITERATOR_END);

			IndexType index;
			switch (state_)
			{
				case ITERATOR_NULLARY:
					indexValue_ = std::make_pair(index, vecMap_->container0_); break;

				case ITERATOR_UNARY:
					index.push_back(itUnary_->first);
					indexValue_ = std::make_pair(index, itUnary_->second); break;

				case ITERATOR_BINARY:
					index.push_back(itBinary_->first.first);
					index.push_back(itBinary_->first.second);
					indexValue_ = std::make_pair(index, itBinary_->second); break;

				case ITERATOR_NNARY:
					indexValue_ = std::make_pair(itNnary_->first, itNnary_->second); break;

				default: throw std::logic_error(__func__ +
					std::string(": invalid iterator state")); break;
			}
		}

	public:   // Public methods

		explicit Tconst_iterator(const Type* vecMap, bool end = false)
			: vecMap_(vecMap),
				indexValue_(),
				state_((end)? ITERATOR_END : ITERATOR_INVALID),
				itUnary_(),
				itBinary_(),
				itNnary_()
		{
			// Assertions
			assert(vecMap_ != static_cast<const Type*>(0));

			if (state_ != ITERATOR_END)
			{
				reset();
				++(*this);
			}
		}


		Tconst_iterator(const Tconst_iterator& it)
			: vecMap_(it.vecMap_),
				indexValue_(it.indexValue_),
				state_(it.state_),
				itUnary_(it.itUnary_),
				itBinary_(it.itBinary_),
				itNnary_(it.itNnary_)
		{
			// Assertions
			assert(vecMap_ != static_cast<const Type*>(0));
		}


		Tconst_iterator& operator=(const Tconst_iterator& rhs)
		{
			if (&rhs != this)
			{
				vecMap_ = rhs.vecMap_;
				indexValue_ = rhs.indexValue_;
				state_ = rhs.state_;
				itUnary_ = rhs.itUnary_;
				itBinary_ = rhs.itBinary_;
				itNnary_ = rhs.itNnary_;
			}

			return *this;
		}


		Tconst_iterator& operator++()
		{
			// Assertions
			assert(vecMap_ != static_cast<const Type*>(0));

			bool sound = false;

			while (!sound)
			{	// until we reach a sound value
				switch (state_)
				{
					case ITERATOR_INVALID:
						state_ = ITERATOR_NULLARY;
						if (vecMap_->container0_ != vecMap_->defaultValue_)
						{
							sound = true;
							updateIndexValue();
						}

						break;

					case ITERATOR_NULLARY:
						state_ = ITERATOR_UNARY;
						itUnary_ = vecMap_->container1_.begin();
						if (itUnary_ != vecMap_->container1_.end())
						{
							sound = true;
							updateIndexValue();
							++itUnary_;
						}

						break;

					case ITERATOR_UNARY:
						if (itUnary_ == vecMap_->container1_.end())
						{
							state_ = ITERATOR_BINARY;
							itBinary_ = vecMap_->container2_.begin();
							if (itBinary_ != vecMap_->container2_.end())
							{
								sound = true;
								updateIndexValue();
							}
						}
						else
						{
							sound = true;
							updateIndexValue();
							++itUnary_;
						}

						break;

					case ITERATOR_BINARY:
						if (itBinary_ == vecMap_->container2_.end())
						{
							state_ = ITERATOR_NNARY;
							itNnary_ = vecMap_->containerN_.begin();
							if (itNnary_ != vecMap_->containerN_.end())
							{
								sound = true;
								updateIndexValue();
								++itNnary_;
							}
						}
						else
						{
							sound = true;
							updateIndexValue();
							++itBinary_;
						}

						break;

					case ITERATOR_NNARY:
						if (itNnary_ == vecMap_->containerN_.end())
						{
							sound = true;
							state_ = ITERATOR_END;
						}
						else
						{
							sound = true;
							updateIndexValue();
							++itNnary_;
						}

						break;

					default: throw std::logic_error(__func__ +
						std::string(": invalid attempt to increment iterator")); break;
				}
			}

			return *this;
		}


		bool operator==(const Tconst_iterator& rhs) const
		{
			// Assertions
			assert(state_ != ITERATOR_INVALID);
			assert(vecMap_ != static_cast<Type*>(0));

			if (vecMap_ != rhs.vecMap_)
			{
				throw std::logic_error(__func__ +
					std::string(": an attempt to compare iterators from different containers"));
			}

			if (state_ != rhs.state_)
			{
				return false;
			}

			switch (state_)
			{
				case ITERATOR_INVALID: return false; break;
				case ITERATOR_NULLARY: return true; break;
				case ITERATOR_UNARY: return itUnary_ == rhs.itUnary_; break;
				case ITERATOR_BINARY: return itBinary_ == rhs.itBinary_; break;
				case ITERATOR_NNARY: return itNnary_ == rhs.itNnary_; break;
				case ITERATOR_END: return true; break;
				default: throw std::logic_error(__func__ +
					std::string(": invalid state")); break;
			}
		}

		inline bool operator!=(const Tconst_iterator& rhs) const
		{
			return !(*this == rhs);
		}

		inline const IndexValueType& operator*() const
		{
			// Assertions
			assert(state_ != ITERATOR_INVALID);

			return indexValue_;
		}

		inline const IndexValueType* operator->() const
		{
			// Assertions
			assert(state_ != ITERATOR_INVALID);

			return &indexValue_;
		}
	};

public:   // Public data types

	typedef Tconst_iterator const_iterator;

private:  // Private data members

	ValueType defaultValue_;

	ValueType container0_;

	HashTableUnary container1_;

	HashTableBinary container2_;

	HashTableNnary containerN_;

private:  // Private methods


	const ValueType& getValueForArity0(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 0);

		return container0_;
	}

	const ValueType& getValueForArity1(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 1);

		typename HashTableUnary::const_iterator it = container1_.end();
		if ((it = container1_.find(lhs[0])) == container1_.end())
		{	// in case the value is not in the hash table
			return defaultValue_;
		}

		return it->second;
	}

	const ValueType& getValueForArity2(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() == 2);

		typename HashTableBinary::const_iterator it = container2_.end();
		if ((it = container2_.find(std::make_pair(lhs[0], lhs[1]))) == container2_.end())
		{	// in case the value is not in the hash table
			return defaultValue_;
		}

		return it->second;
	}

	const ValueType& getValueForArityN(const IndexType& lhs) const
	{
		// Assertions
		assert(lhs.size() > 2);

		typename HashTableNnary::const_iterator it = containerN_.end();
		if ((it = containerN_.find(lhs)) == containerN_.end())
		{	// in case the key is not in the hash table
			return defaultValue_;
		}

		return it->second;
	}


	void setValueForArity0(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 0);

		container0_ = value;
	}

	void setValueForArity1(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 1);

		container1_.insert(std::make_pair(lhs[0], value));
	}

	void setValueForArity2(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() == 2);

		container2_.insert(std::make_pair(std::make_pair(lhs[0],lhs[1]), value));
	}

	void setValueForArityN(const IndexType& lhs, const ValueType& value)
	{
		// Assertions
		assert(lhs.size() > 2);

		containerN_.insert(std::make_pair(lhs, value));
	}

public:   // Public methods

	VectorMap(const ValueType& defaultValue)
		: defaultValue_(defaultValue),
			container0_(defaultValue_),
			container1_(),
			container2_(),
			containerN_()
	{ }

	const ValueType& GetValue(const IndexType& index) const
	{
		switch (index.size())
		{
			case 0: return getValueForArity0(index); break;
			case 1: return getValueForArity1(index); break;
			case 2: return getValueForArity2(index); break;
			default: return getValueForArityN(index); break;
		}
	}

	void SetValue(const IndexType& index, const ValueType& value)
	{
		switch (index.size())
		{
			case 0: setValueForArity0(index, value); break;
			case 1: setValueForArity1(index, value); break;
			case 2: setValueForArity2(index, value); break;
			default: setValueForArityN(index, value); break;
		}
	}

	void insert(const VectorMap& vecMap)
	{
		// copy all vectors (without the nullary one)
		container1_.insert(vecMap.container1_.begin(), vecMap.container1_.end());
		container2_.insert(vecMap.container2_.begin(), vecMap.container2_.end());
		containerN_.insert(vecMap.containerN_.begin(), vecMap.containerN_.end());
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
