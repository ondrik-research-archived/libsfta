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

#include <sfta/map_root_allocator.hh>
#include <sfta/map_leaf_allocator.hh>

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDD
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                User classes                                *
 ******************************************************************************/


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
			SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator>();
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
	asgn = 1;
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
