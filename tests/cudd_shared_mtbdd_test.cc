/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDSharedMTBDD class.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/cudd_shared_mtbdd.hh>
using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDD
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                User classes                                *
 ******************************************************************************/

template
<
	typename Leaf,
	typename Handle
>
struct MyLeafAllocator
{
public:
	typedef Leaf LeafType;
	typedef Handle HandleType;

private:

	typedef std::map<HandleType, LeafType> LeafContainer;

	LeafContainer asocArr_;

	HandleType nextIndex_;

	static const HandleType BOTTOM_INDEX;

protected:

	static HandleType leafReleaser(const HandleType& node, void* data)
	{
		// Assertions
		assert(static_cast<MyLeafAllocator*>(data)
			!= static_cast<MyLeafAllocator*>(0));

		return node;
	}

	MyLeafAllocator()
		: asocArr_(), nextIndex_(1)
	{
		asocArr_[BOTTOM_INDEX] = LeafType();
	}

	void setBottom(const LeafType& leaf)
	{
		asocArr_[BOTTOM_INDEX] = leaf;
	}

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

	std::vector<HandleType> getAllHandles() const
	{
		std::vector<HandleType> result;

		for (typename std::map<HandleType, LeafType>::const_iterator it
			= asocArr_.begin(); it != asocArr_.end(); ++it)
		{
			result.push_back(it->first);
		}

		return result;
	}

	~MyLeafAllocator() { }
};


template
<
	typename L,
	typename H
>
const typename MyLeafAllocator<L, H>::HandleType MyLeafAllocator<L, H>::BOTTOM_INDEX = 0;


template
<
	typename Root,
	typename Handle
>
struct MyRootAllocator;

template
<
	typename Handle
>
struct MyRootAllocator<unsigned, Handle>
{
public:
	typedef Handle HandleType;
	typedef unsigned RootType;

private:

	typedef std::map<unsigned, HandleType> HandleContainer;

public:
	typedef typename HandleContainer::iterator Iterator;

private:

	HandleContainer arr_;
	size_t nextIndex_;

protected:

	MyRootAllocator() : arr_(), nextIndex_(0)
	{
	}

	unsigned allocateRoot(const HandleType& handle)
	{
		arr_[nextIndex_] = handle;
		++nextIndex_;
		return nextIndex_ - 1;
	}

	const HandleType& getHandleOfRoot(unsigned root) const
	{
		// try to find given root
		typename HandleContainer::const_iterator it = arr_.find(root);
		if (it == arr_.end())
		{	// in case it couldn't be found
			assert(false);
			throw std::runtime_error("Trying to access root \""
				+ SFTA::Private::Convert::ToString(root) + "\" that is not managed.");
		}

		return it->second;
	}

	void changeHandleOfRoot(unsigned root, const HandleType& handle)
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

	std::vector<unsigned> getAllRoots() const
	{
		std::vector<RootType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{
			res.push_back(it->first);
		}

		return res;
	}

	std::vector<HandleType> getAllRootHandles() const
	{
		std::vector<HandleType> res(0);

		for (typename HandleContainer::const_iterator it = arr_.begin();
			it != arr_.end(); ++it)
		{
			res.push_back(it->second);
		}

		return res;
	}

	void eraseRoot(unsigned root)
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

	~MyRootAllocator() { }

public:

	const static size_t INITIAL_VECTOR_SIZE = 8;
};


struct MyVariableAssignment
{
private:

	unsigned vars_;

public:
	enum
	{
		ONE,
		ZERO,
		DONT_CARE
	};

	MyVariableAssignment(unsigned value)
		: vars_(value)
	{}


	inline int GetIthVariableValue(unsigned i) const
	{
		if (!((vars_ >> i) & 1))
		{
			return ZERO;
		}
		else
		{
			return ONE;
		}
	}

	size_t Size() const
	{
		//return 8*sizeof(unsigned);
		return 4;
	}

	std::string ToString() const
	{
		return SFTA::Private::Convert::ToString(vars_);
	}

	friend std::ostream& operator<<(std::ostream& os, const MyVariableAssignment asgn)
	{
		return (os << asgn.ToString());
	}
};


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDFixture : public LogFixture
{
private:

	CUDDSharedMTBDDFixture(const CUDDSharedMTBDDFixture& fixture);
	CUDDSharedMTBDDFixture& operator=(const CUDDSharedMTBDDFixture& rhs);

public:

	typedef AbstractSharedMTBDD<unsigned, unsigned, MyVariableAssignment> ASMTBDD;

protected:

	ASMTBDD* mtbdd;

public:

	CUDDSharedMTBDDFixture()
		: mtbdd(static_cast<ASMTBDD*>(0))
	{
		mtbdd = new SFTA::CUDDSharedMTBDD<unsigned, unsigned, MyVariableAssignment,
			MyLeafAllocator, MyRootAllocator>();
	}

	~CUDDSharedMTBDDFixture()
	{
		delete mtbdd;
	}

};


unsigned leaf_addition(const unsigned& lhs, const unsigned& rhs)
{
	return lhs + rhs;
}

/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	unsigned root = mtbdd->CreateRoot();

	unsigned value_added = 7;

	MyVariableAssignment asgn(5);
	mtbdd->SetValue(root, asgn, value_added);

	ASMTBDD::LeafContainer leaves = mtbdd->GetValue(root, asgn);

	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	value_added = 17;
	asgn = 3;
	mtbdd->SetValue(root, asgn, value_added);
	leaves = mtbdd->GetValue(root, asgn);

	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	value_added = 7;
	asgn = 5;
	leaves = mtbdd->GetValue(root, asgn);
	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	unsigned root2 = mtbdd->CreateRoot();
	value_added = 13;
	asgn = 11;
	mtbdd->SetValue(root2, asgn, value_added);

	value_added = 11;
	asgn = 3;
	mtbdd->SetValue(root2, asgn, value_added);

	unsigned root3 = mtbdd->Apply(root, root2, leaf_addition);

	value_added = 28;
	leaves = mtbdd->GetValue(root3, asgn);
	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	mtbdd->DumpToDotFile("pokus.dot");
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
