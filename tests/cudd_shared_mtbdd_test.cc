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
protected:
	~MyLeafAllocator() { }
};

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
private:

	typedef Handle HandleType;

	std::vector<HandleType> arr;
	size_t next_index;

protected:

	typedef typename std::vector<HandleType>::iterator Iterator;

	MyRootAllocator() : arr(INITIAL_VECTOR_SIZE), next_index(0)
	{
	}

	unsigned AllocateRoot(Handle& handle)
	{
		if (next_index >= arr.size())
		{
			arr.resize(2 * arr.size());
		}

		arr[next_index] = handle;
		return next_index++;
	}

	Iterator Begin()
	{
		return arr.begin();
	}

	Iterator End()
	{
		return arr.begin() + next_index;
	}

	~MyRootAllocator() { }

public:

	const static size_t INITIAL_VECTOR_SIZE = 8;
};


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDFixture : public LogFixture
{
private:

	CUDDSharedMTBDDFixture(const CUDDSharedMTBDDFixture& fixture);
	CUDDSharedMTBDDFixture& operator=(const CUDDSharedMTBDDFixture& rhs);

protected:

	typedef AbstractSharedMTBDD<unsigned, unsigned, unsigned> ASMTBDD;

	ASMTBDD* mtbdd;

public:

	CUDDSharedMTBDDFixture()
		: mtbdd(static_cast<ASMTBDD*>(0))
	{
		mtbdd = new SFTA::CUDDSharedMTBDD<unsigned, unsigned, unsigned,
			MyLeafAllocator, MyRootAllocator>();
	}

	~CUDDSharedMTBDDFixture()
	{
		delete mtbdd;
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	unsigned root = mtbdd->CreateRoot();

}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
