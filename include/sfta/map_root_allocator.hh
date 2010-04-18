/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with MapRootAllocator policie for CUDDSharedMTBDD
 *
 *****************************************************************************/

#ifndef _SFTA_MAP_ROOT_ALLOCATOR_HH_
#define _SFTA_MAP_ROOT_ALLOCATOR_HH_


// insert the class into proper namespace
namespace SFTA
{
	namespace Private
	{
		template
		<
			typename Root,
			typename Handle
		>
		struct MapRootAllocator;
	}
}


/**
 * @brief   Root allocator that uses map
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is a @c RootAllocator policy for SFTA::CUDDSharedMTBDD that uses a map
 * to provide mapping between virtual and real roots of an MTBDD.
 * 
 * @see  SFTA::CUDDSharedMTBDD
 *
 * @tparam  Root    The type of root.
 * @tparam  Handle  The type of handle.
 */
template
<
	typename Root,
	typename Handle
>
struct SFTA::Private::MapRootAllocator
{
public:   // public data types

	/**
	 * @brief  Type of root handle
	 *
	 * The data type of a root handle.
	 */
	typedef Handle HandleType;


	/**
	 * @brief  Type of root
	 *
	 * The data type of root.
	 */
	typedef Root RootType;


private:  // private data types

	/**
	 * @brief  Container of root handles
	 *
	 * The data type that is addressed by RootType and serves as a container of
	 * handles to root nodes of MTBDD.
	 */
	typedef std::map<RootType, HandleType> HandleContainer;


private:  // private data members

	/**
	 * @brief  Container of handles
	 *
	 * Container of handles to root nodes of MTBDD.
	 */
	HandleContainer arr_;


	/**
	 * @brief  Counter of indices
	 *
	 * Counter of indices, if a new root is to be inserted, this counter
	 * increments and the old value is returned.
	 */
	RootType nextIndex_;


protected:// protected methods


	/**
	 * @brief  Constructor
	 *
	 * The constructor.
	 */
	MapRootAllocator() : arr_(), nextIndex_(0)
	{ }


	/**
	 * @brief  Allocates a new root
	 *
	 * This method allocates a new root for given handle and returns the root.
	 *
	 * @param[in]  handle  The handle the root is to be pointing to
	 *
	 * @returns  The root
	 */
	RootType allocateRoot(const HandleType& handle)
	{
		arr_[nextIndex_] = handle;
		++nextIndex_;
		return nextIndex_ - 1;
	}


	/**
	 * @brief  Gets handle of given root
	 *
	 * This method gets a handle for given root.
	 *
	 * @param[in]  root  The root for which the handle is to be retrieved
	 *
	 * @returns  The handle of the root
	 */
	const HandleType& getHandleOfRoot(const RootType& root) const
	{
		// try to find given root
		typename HandleContainer::const_iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		return it->second;
	}


	/**
	 * @brief  Changes the handle of given root
	 *
	 * This method changes the handle of given root, i.e. it substitutes a root
	 * for a different one.
	 *
	 * @param[in]  root    The root for which the handle is to be changed
	 * @param[in]  handle  The new handle of the root
	 */
	void changeHandleOfRoot(const RootType& root, const HandleType& handle)
	{
		// try to find given root
		typename HandleContainer::iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to change value of root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		it->second = handle;
	}


	/**
	 * @brief  Returns all roots
	 *
	 * The method that returns all roots which are allocated.
	 *
	 * @see  getAllRootHandles()
	 *
	 * @returns  All roots
	 */
	std::vector<RootType> getAllRoots() const
	{
		std::vector<RootType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{	// push back each root
			res.push_back(it->first);
		}

		return res;
	}


	/**
	 * @brief  Returns all handles of roots
	 *
	 * The method that returns all handles of roots which are allocated.
	 *
	 * @see  getAllRoots()
	 *
	 * @returns  All handles of roots
	 */
	std::vector<HandleType> getAllRootHandles() const
	{
		std::vector<HandleType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{	// push back each handle
			res.push_back(it->second);
		}

		return res;
	}


	/**
	 * @brief  Erases a root
	 *
	 * This method erases a root from the container of roots.
	 *
	 * @param[in]  root  The root to be erased
	 */
	void eraseRoot(RootType root)
	{
		// try to find given root
		typename HandleContainer::iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to erase root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		arr_.erase(it);
	}


	/**
	 * @brief  Destructor
	 *
	 * The destructor.
	 */
	~MapRootAllocator()
	{ }

};

#endif
