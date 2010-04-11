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

#include <sfta/root_allocator.hh>

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


	typedef SFTA::Private::Convert Convert;

	LeafContainer asocArr_;

	HandleType nextIndex_;

	static const HandleType BOTTOM_INDEX;

	/*
	 * @brief  The name of the Log4cpp category for logging
	 *
	 * The name of the Log4cpp category used for logging messages from this
	 * class.
	 */
	static const char* LOG_CATEGORY_NAME;

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

// Setting the logging category name for Log4cpp
template
<
	typename L,
	typename H
>
const char* MyLeafAllocator<L, H>::LOG_CATEGORY_NAME = "my_leaf_allocator";


template
<
	typename L,
	typename H
>
const typename MyLeafAllocator<L, H>::HandleType MyLeafAllocator<L, H>::BOTTOM_INDEX = 0;


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
		if (!((vars_ >> (Size() - 1 - i)) & 1))
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

	typedef AbstractSharedMTBDD<unsigned, std::vector<unsigned>, MyVariableAssignment> ASMTBDD;

protected:

	ASMTBDD* mtbdd;

public:

	CUDDSharedMTBDDFixture()
		: mtbdd(static_cast<ASMTBDD*>(0))
	{
		mtbdd = new SFTA::CUDDSharedMTBDD<unsigned, std::vector<unsigned>, MyVariableAssignment,
			MyLeafAllocator, SFTA::Private::MapRootAllocator>();
	}

	~CUDDSharedMTBDDFixture()
	{
		delete mtbdd;
	}

};


std::vector<unsigned> leaf_addition(const std::vector<unsigned>& lhs, const std::vector<unsigned>& rhs)
{
	std::vector<unsigned> res(lhs.size() + rhs.size());

	size_t i = 0;
	for (i = 0; i < lhs.size(); ++i)
	{
		res[i] = lhs[i];
	}

	for (size_t j = 0; j < rhs.size(); ++j)
	{
		res[i+j] = rhs[j];
	}

	return res;
}

/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	unsigned root = mtbdd->CreateRoot();

	std::vector<unsigned> value_added(0);
	MyVariableAssignment asgn(0);

//	for (unsigned i = 1; i <= 255; ++i)
//	{
//		value_added.push_back(i);
//		asgn = i;
//		mtbdd->SetValue(root, asgn, value_added);
//	}

	value_added.push_back(3);
	value_added.push_back(5);
	asgn = 7;
	mtbdd->SetValue(root, asgn, value_added);
	ASMTBDD::LeafContainer leaves = mtbdd->GetValue(root, asgn);

	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	value_added[0] = 17;
	value_added[1] = 15;
	asgn = 3;
	mtbdd->SetValue(root, asgn, value_added);
	leaves = mtbdd->GetValue(root, asgn);

	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	value_added[0] = 1;
	value_added[1] = 2;
	asgn = 5;
	mtbdd->SetValue(root, asgn, value_added);
	leaves = mtbdd->GetValue(root, asgn);
	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	unsigned root2 = mtbdd->CreateRoot();
	value_added[0] = 11;
	value_added[1] = 19;
	asgn = 4;
	mtbdd->SetValue(root2, asgn, value_added);

	value_added[0] = 7;
	value_added[1] = 9;
	asgn = 3;
	mtbdd->SetValue(root2, asgn, value_added);

	unsigned root3 = mtbdd->Apply(root, root2, leaf_addition);

	value_added.resize(4);
	value_added[0] = 17;
	value_added[1] = 15;
	value_added[2] = 7;
	value_added[3] = 9;
	leaves = mtbdd->GetValue(root3, asgn);
	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));

	mtbdd->DumpToDotFile("pokus.dot");
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
