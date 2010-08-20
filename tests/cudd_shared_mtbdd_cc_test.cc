/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDSharedMTBDD class with roots as chars and leaves as
 *    chars.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/cudd_shared_mtbdd.hh>
using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;

#include <sfta/map_root_allocator.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/compact_variable_assignment.hh>

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDDCC
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDCharCharFixture : public LogFixture
{
private:

	CUDDSharedMTBDDCharCharFixture(
		const CUDDSharedMTBDDCharCharFixture& fixture);
	CUDDSharedMTBDDCharCharFixture& operator=(
		const CUDDSharedMTBDDCharCharFixture& rhs);

public:

	typedef SFTA::Private::CompactVariableAssignment<4> MyVariableAssignment;

	typedef AbstractSharedMTBDD<char, char, MyVariableAssignment> ASMTBDDCC;

	typedef CUDDSharedMTBDD<char, char, MyVariableAssignment,
			SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator>
			CuddMTBDDCC;

public:

	CUDDSharedMTBDDCharCharFixture()
	{ }

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

//class LeafAdditionFunctor
//	: public CUDDSharedMTBDDFixture::ASMTBDD::AbstractApplyFunctorType
//{
//	public:
//
//	virtual std::vector<unsigned> operator()(const std::vector<unsigned>& lhs, const std::vector<unsigned>& rhs)
//	{
//		std::vector<unsigned> res(lhs.size() + rhs.size());
//
//		size_t i = 0;
//		for (i = 0; i < lhs.size(); ++i)
//		{
//			res[i] = lhs[i];
//		}
//
//		for (size_t j = 0; j < rhs.size(); ++j)
//		{
//			res[i+j] = rhs[j];
//		}
//
//		return res;
//	}
//};


BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDCharCharFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();

	delete bdd;
}

//BOOST_AUTO_TEST_CASE(setters_and_getters_test)
//{
//	unsigned root = mtbdd->CreateRoot();
//
//	std::vector<unsigned> value_added(0);
//	MyVariableAssignment asgn("01X1");
//
////	for (unsigned i = 1; i <= 255; ++i)
////	{
////		value_added.push_back(i);
////		asgn = i;
////		mtbdd->SetValue(root, asgn, value_added);
////	}
//
//	value_added.push_back(3);
//	value_added.push_back(5);
//	asgn = MyVariableAssignment("0111");
//	mtbdd->SetValue(root, asgn, value_added);
//	asgn = MyVariableAssignment("0001");
//	mtbdd->SetValue(root, asgn, value_added);
//	ASMTBDD::LeafContainer leaves = mtbdd->GetValue(root, asgn);
//
//	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));
//
//	value_added[0] = 17;
//	value_added[1] = 15;
//	asgn = MyVariableAssignment("XXX1");
//	mtbdd->SetValue(root, asgn, value_added);
//	leaves = mtbdd->GetValue(root, asgn);
//
//	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));
//
//	value_added[0] = 1;
//	value_added[1] = 2;
//	asgn = MyVariableAssignment("0101");
//	mtbdd->SetValue(root, asgn, value_added);
//	leaves = mtbdd->GetValue(root, asgn);
//	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));
//
//	unsigned root2 = mtbdd->CreateRoot();
//	value_added[0] = 11;
//	value_added[1] = 19;
//	asgn = MyVariableAssignment("1XX0");
//	mtbdd->SetValue(root2, asgn, value_added);
//
//	value_added[0] = 7;
//	value_added[1] = 9;
//	asgn = MyVariableAssignment("000X");
//	mtbdd->SetValue(root2, asgn, value_added);
//
//	unsigned root3 = mtbdd->Apply(root, root2, new LeafAdditionFunctor);
//
//	value_added.resize(4);
//	value_added[0] = 17;
//	value_added[1] = 15;
//	value_added[2] = 7;
//	value_added[3] = 9;
//	leaves = mtbdd->GetValue(root3, asgn);
//	BOOST_CHECK((leaves.size() == 1) && (value_added == *(leaves[0])));
//
//	mtbdd->DumpToDotFile("pokus.dot");
//
//	BOOST_TEST_MESSAGE("Serialized MTBDD: " + mtbdd->Serialize());
//}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
