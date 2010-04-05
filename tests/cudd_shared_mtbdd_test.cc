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

	std::map<HandleType, LeafType> asoc_arr_;

	HandleType next_index_;

protected:

	MyLeafAllocator()
		: asoc_arr_(), next_index_(0)
	{
	}

	HandleType createLeaf(const LeafType& leaf)
	{
		asoc_arr_[next_index_] = leaf;
		++next_index_;
		return next_index_ - 1;
	}

	LeafType& getLeafOfHandle(const HandleType& handle)
	{
		return asoc_arr_[handle];
	}

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
public:
	typedef Handle HandleType;

private:

	std::vector<HandleType> arr_;
	size_t next_index_;

protected:

	typedef typename std::vector<HandleType>::iterator Iterator;

	MyRootAllocator() : arr_(INITIAL_VECTOR_SIZE), next_index_(0)
	{
	}

	unsigned allocateRoot(const HandleType& handle)
	{
		if (next_index_ >= arr_.size())
		{
			arr_.resize(2 * arr_.size());
		}

		arr_[next_index_] = handle;
		++next_index_;
		return next_index_ - 1;
	}

	Iterator begin()
	{
		return arr_.begin();
	}

	Iterator end()
	{
		return arr_.begin() + next_index_;
	}

	const HandleType& getHandleOfRoot(unsigned root)
	{
		// Assertions
		assert(root < arr_.size());

		return arr_[root];
	}

	void changeHandleOfRoot(unsigned root, const HandleType& handle)
	{
		// Assertions
		assert(root < arr_.size());

		arr_[root] = handle;
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


	int GetIthVariableValue(unsigned i)
	{
		if (!((vars_ >> i) & 1))
		{
			return ONE;
		}
		else
		{
			return ZERO;
		}
	}


	size_t Size() const
	{
		return 8*sizeof(unsigned);
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

protected:

	typedef AbstractSharedMTBDD<unsigned, unsigned, MyVariableAssignment> ASMTBDD;

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

	unsigned value_read = mtbdd->GetValue(root, asgn);

	BOOST_CHECK(value_added == value_read);

	value_added = 15;
	asgn = 3;
	mtbdd->SetValue(root, asgn, value_added);
	value_read = mtbdd->GetValue(root, asgn);

	BOOST_CHECK(value_added == value_read);

}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
