/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with MapLeafAllocator policy for CUDDSharedMTBDD
 *
 *****************************************************************************/

#ifndef _SFTA_MAP_LEAF_ALLOCATOR_HH_
#define _SFTA_MAP_LEAF_ALLOCATOR_HH_


// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			typename Leaf,
			typename Handle,
			class AbstractMonadicApplyFunctor
		>
		struct MapLeafAllocator;
	}
}


/**
 * @brief   Leaf allocator that uses map
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is a @c LeafAllocator policy for SFTA::CUDDSharedMTBDD that uses a map
 * to provide mapping between virtual and real leaves of an MTBDD.
 * 
 * @see  SFTA::CUDDSharedMTBDD
 *
 * @tparam  Leaf                         The type of leaf.
 * @tparam  Handle                       The type of handle.
 * @tparam  AbstractMonadicApplyFunctor  The type of the monadic Apply functor
 *                                       of the underlying MTBDD package
 *                                       facade.
 */
template
<
	typename Leaf,
	typename Handle,
	class AbstractMonadicApplyFunctor
>
struct SFTA::Private::MapLeafAllocator
{
public:   // Public data types

	/**
	 * @brief  Type of leaf
	 *
	 * The data type of leaf.
	 */
	typedef Leaf LeafType;


	/**
	 * @brief  Type of leaf handle
	 *
	 * The data type of leaf handle.
	 */
	typedef Handle HandleType;

private:  // Private data types

	/**
	 * @brief  The container type for leaves
	 *
	 * The data type that serves as a container for leaves.
	 */
	typedef std::map<HandleType, LeafType> LeafContainer;


	/**
	 * @brief  The type of the Convert class
	 *
	 * The type of the Convert class.
	 */
	typedef SFTA::Private::Convert Convert;

private:   // Private data types


	/**
	 * @brief  Leaf releaser
	 *
	 * Monadic Apply functor that properly releases leaves.
	 */
	class ReleaserMonadicApplyFunctor : public AbstractMonadicApplyFunctor
	{
	public:

		virtual HandleType operator()(const HandleType& val)
		{
			// @todo  TODO  Properly release the leaf
			return val;
		}
	};


private:  // Private data members

	/**
	 * @brief  The container for leaves
	 *
	 * The container that stores leaves.
	 */
	LeafContainer asocArr_;


	/**
	 * @brief  Counter of indices
	 *
	 * Counter of indices, if a new leaf is to be inserted, this counter
	 * increments and the old value is returned.
	 */
	HandleType nextIndex_;


	/**
	 * @brief  Leaf releaser
	 *
	 * Monadic Apply functor that is used when a leaf is released.
	 */
	AbstractMonadicApplyFunctor* releaser_;


protected:// Protected data memebers

	/**
	 * @brief  The bottom of the MTBDD
	 *
	 * The value used for the bottom of the MTBDD.
	 */
	static const HandleType BOTTOM;

private:  // Private methods

	MapLeafAllocator(const MapLeafAllocator&);
	MapLeafAllocator& operator=(const MapLeafAllocator&);

protected:// Protected methods

	/**
	 * @brief  Constructor
	 *
	 * The default constructor
	 */
	MapLeafAllocator()
		: asocArr_(), nextIndex_(1), releaser_(new ReleaserMonadicApplyFunctor())
	{
		// initialize the value of bottom
		asocArr_[BOTTOM] = LeafType();
	}


	/**
	 * @brief  Sets the value of bottom
	 *
	 * Sets the value of bottom of the MTBDD
	 *
	 * @see  BOTTOM
	 *
	 * @param[in]  leaf  The value of the bottom
	 */
	void setBottom(const LeafType& leaf)
	{
		// set the value of bottom
		asocArr_[BOTTOM] = leaf;
	}


	/**
	 * @brief  Creates a leaf
	 *
	 * Attempts to first find the leaf in the container and in case it is not
	 * there creates a new one and returns reference to it.
	 *
	 * @param[in]  leaf  The value of the leaf
	 *
	 * @returns  Handle to the leaf
	 */
	HandleType createLeaf(const LeafType& leaf)
	{
		for (typename LeafContainer::const_iterator it = asocArr_.begin();
			it != asocArr_.end(); ++it)
		{	// try to find the leaf in already created leaves
			if (it->second == leaf)
			{	// in case we found it
				return it->first;
			}
		}

		// otherwise create a new leaf
		asocArr_[nextIndex_] = leaf;
		++nextIndex_;
		return nextIndex_ - 1;
	}


	/**
	 * @brief  Returns a leaf associated with given handle
	 *
	 * Returns the leaf that is at given container associated with given handle.
	 *
	 * @param[in]  handle  The handle the leaf for which is to be found
	 *
	 * @returns  The leaf associated with given handle
	 */
	LeafType& getLeafOfHandle(const HandleType& handle)
	{
		// try to find given leaf
		typename LeafContainer::iterator it = asocArr_.find(handle);
		if (it == asocArr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return it->second;
	}


	/**
	 * @brief  @copybrief getLeafOfHandle()
	 *
	 * @copydetails  getLeafOfHandle()
	 */
	const LeafType& getLeafOfHandle(const HandleType& handle) const
	{
		// try to find given leaf
		typename LeafContainer::const_iterator it = asocArr_.find(handle);
		if (it == asocArr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return it->second;
	}


	/**
	 * @brief  Returns all handles
	 *
	 * Returns a std::vector of all handles that have a leaf associated in the
	 * container
	 *
	 * @returns  A std::vector vector of all handles
	 */
	std::vector<HandleType> getAllHandles() const
	{
		std::vector<HandleType> result;

		for (typename std::map<HandleType, LeafType>::const_iterator it
			= asocArr_.begin(); it != asocArr_.end(); ++it)
		{	// push back all handles that have associated a leaf in the container
			result.push_back(it->first);
		}

		return result;
	}


	/**
	 * @brief  Gets the release functor
	 *
	 * Returns the release monadic Apply functor. This functor takes care of
	 * properly releasing given leaf.
	 *
	 * @returns  Proper release monadic Apply functor
	 */
	inline AbstractMonadicApplyFunctor* getLeafReleaser()
	{
		return releaser_;
	}

	/**
	 * @brief  Destructor
	 *
	 * The destructor.
	 */
	~MapLeafAllocator()
	{ }

};


// The bottom of the MTBDD
template
<
	typename L,
	typename H,
	class AMAF
>
const typename SFTA::Private::MapLeafAllocator<L, H, AMAF>::HandleType
	SFTA::Private::MapLeafAllocator<L, H, AMAF>::BOTTOM = 0;

#endif
