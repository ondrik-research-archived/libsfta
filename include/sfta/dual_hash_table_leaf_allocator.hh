/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    File with DualHashTableLeafAllocator policy for CUDDSharedMTBDD
 *
 *****************************************************************************/

#ifndef _SFTA_DUAL_HASH_TABLE_LEAF_ALLOCATOR_HH_
#define _SFTA_DUAL_HASH_TABLE_LEAF_ALLOCATOR_HH_

// Standard library header files
#include <tr1/unordered_map>

// Boost library headers
#include <boost/functional/hash.hpp>

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
		struct DualHashTableLeafAllocator;
	}
}


/**
 * @brief   Leaf allocator that uses dual hash tables
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This is a @c LeafAllocator policy for SFTA::CUDDSharedMTBDD that uses two
 * hash tables to provide mapping between virtual and real leaves of an MTBDD
 * and an inverse mapping.
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
struct SFTA::Private::DualHashTableLeafAllocator
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
	 * @brief  Structure for handle <-> leaf pair
	 *
	 * This structure contains the pair of handle and leaf
	 */
	struct LeafDescriptor
	{
	public:   // public data members
		/**
		 * Leaf handle.
		 */
		HandleType handle;

		/**
		 * Leaf.
		 */
		LeafType leaf;

	public:   // public methods

		/**
		 * @brief  Constructor
		 *
		 * The constructor of the structure.
		 */
		LeafDescriptor(HandleType hndl, const LeafType& lf)
			: handle(hndl), leaf(lf)
		{ }
	};


	/**
	 * Container that maps leaf handles to leaf desciptors
	 */
	typedef std::tr1::unordered_map<HandleType, LeafDescriptor*> HandleToDescriptorMap;


	/**
	 * Container that maps leaves to leaf descriptors.
	 */
	typedef std::tr1::unordered_map<LeafType, LeafDescriptor*> LeafToDescriptorMap;


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
	private:

		DualHashTableLeafAllocator* allocator_;

		ReleaserMonadicApplyFunctor(const ReleaserMonadicApplyFunctor& func);
		ReleaserMonadicApplyFunctor& operator=(
			const ReleaserMonadicApplyFunctor& func);


	public:

		ReleaserMonadicApplyFunctor(DualHashTableLeafAllocator* allocator)
			: allocator_(allocator)
		{
			// Assertions
			assert(allocator_ != static_cast<DualHashTableLeafAllocator*>(0));
		}

		virtual HandleType operator()(const HandleType& val)
		{
			return val;
		}
	};


private:  // Private data members


	/**
	 * Mapping of handles to leaf descriptors.
	 */
	HandleToDescriptorMap handles_;


	/**
	 * Mapping of leaves to leaf descriptors.
	 */
	LeafToDescriptorMap leaves_;


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

	DualHashTableLeafAllocator(const DualHashTableLeafAllocator&);
	DualHashTableLeafAllocator& operator=(const DualHashTableLeafAllocator&);


	/**
	 * @brief  Inserts leaf descriptor
	 *
	 * This method inserts a leaf descriptor into both maps.
	 *
	 * @param[in]  leafPtr  Pointer to leaf descriptor (pointer cannot point
	 *                      to stack and by being passed to this method the
	 *                      structure takes responsibility for its target)
	 */
	void insertLeafDescriptor(LeafDescriptor* leafPtr)
	{
		// insert the leaf -> descriptor pair to the map
		bool insertedNewLeaf =
			leaves_.insert(std::make_pair(leafPtr->leaf, leafPtr)).second;

		// check that new value really was inserted
		assert(insertedNewLeaf);

		// insert the handle -> descriptor pair to the map
		bool insertedNewHandle =
			handles_.insert(std::make_pair(leafPtr->handle, leafPtr)).second;

		// check that new value really was inserted
		assert(insertedNewHandle);
	}

protected:// Protected methods

	/**
	 * @brief  Constructor
	 *
	 * The default constructor
	 */
	DualHashTableLeafAllocator()
		: handles_(), leaves_(), nextIndex_(BOTTOM + 1),
		releaser_(new ReleaserMonadicApplyFunctor(this))
	{ }


	/**
	 * @brief  Sets the value of bottom
	 *
	 * Sets the value of bottom of the MTBDD. This method needs to be called
	 * before any other, otherwise the internal structure of mapping may be
	 * inconsistent.
	 *
	 * @see  BOTTOM
	 *
	 * @param[in]  leaf  The value of the bottom
	 */
	void setBottom(const LeafType& leaf)
	{
		LeafDescriptor* leafDesc = new LeafDescriptor(BOTTOM, leaf);
		insertLeafDescriptor(leafDesc);
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
		// first attempt to find the leaf if it already exists
		typename LeafToDescriptorMap::iterator itLeaf;
		if ((itLeaf = leaves_.find(leaf)) == leaves_.end())
		{	// in case the leaf is not in the structure yet

			// create new descriptor
			HandleType handle = nextIndex_;
			++nextIndex_;
			LeafDescriptor* leafDesc = new LeafDescriptor(handle, leaf);

			insertLeafDescriptor(leafDesc);

			return handle;
		}
		else
		{	// in case the leaf is already present
			return itLeaf->second->handle;
		}
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
		// try to find given leafb
		typename HandleToDescriptorMap::iterator itHandles;
		if ((itHandles  = handles_.find(handle)) == handles_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return itHandles->second->leaf;
	}


	/**
	 * @brief  @copybrief getLeafOfHandle()
	 *
	 * @copydetails  getLeafOfHandle()
	 */
	const LeafType& getLeafOfHandle(const HandleType& handle) const
	{
		// try to find given leaf
		typename HandleToDescriptorMap::const_iterator itHandles;
		if ((itHandles  = handles_.find(handle)) == handles_.end())
		{	// in case it couldn't be found
			throw std::runtime_error("Trying to access leaf \""
				+ SFTA::Private::Convert::ToString(handle) + "\" that is not managed.");
		}

		return itHandles->second->leaf;
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

		for (typename HandleToDescriptorMap::const_iterator itHandles =
			handles_.begin(); itHandles != handles_.end(); ++itHandles)
		{	// push back all handles that have associated a leaf in the container
			result.push_back(itHandles->first);
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
	 * @brief  Serialization method
	 *
	 * This method serializes the object into std::string
	 */
	std::string serialize() const
	{
		std::string result;

		result += "<mapleafallocator>\n";

		std::vector<HandleType> handles = getAllHandles();
		for (typename std::vector<HandleType>::const_iterator itHandle =
			handles.begin(); itHandle != handles.end(); ++itHandle)
		{
			result += "<pairing>";
			result += "<left>";
			result += Convert::ToString(*itHandle);
			result += "</left>";
			result += "<right>";
			result += Convert::ToString(getLeafOfHandle(*itHandle));
			result += "</right>";
			result += "</pairing>";
			result += "\n";
		}

		result += "</mapleafallocator>";

		return result;
	};


	/**
	 * @brief  Destructor
	 *
	 * The destructor.
	 */
	~DualHashTableLeafAllocator()
	{
		delete releaser_;

		for (typename HandleToDescriptorMap::iterator itHandles = handles_.begin();
			itHandles != handles_.end(); ++itHandles)
		{	// for each handle
			delete itHandles->second;
		}
	}
};


// The bottom of the MTBDD
template
<
	typename L,
	typename H,
	class AMAF
>
const typename SFTA::Private::DualHashTableLeafAllocator<L, H, AMAF>::HandleType
	SFTA::Private::DualHashTableLeafAllocator<L, H, AMAF>::BOTTOM = 0;

#endif
