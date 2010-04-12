/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for MTBDDTransitionFunction class.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/mtbdd_transition_function.hh>
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE MTBDDTransitionFunction
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class MTBDDTransitionFunctionFixture : public LogFixture
{
private:

	MTBDDTransitionFunctionFixture(const MTBDDTransitionFunctionFixture& fixture);
	MTBDDTransitionFunctionFixture& operator=(const MTBDDTransitionFunctionFixture& rhs);

public:

	typedef SFTA::Private::CompactVariableAssignment<4> MyVariableAssignment;

	typedef SFTA::AbstractTransitionFunction<MyVariableAssignment, std::vector<unsigned>, std::vector<unsigned> > ATF;

protected:

	ATF* transFunc_;

public:

	MTBDDTransitionFunctionFixture()
		: transFunc_(static_cast<ATF*>(0))
	{
		transFunc_ = new SFTA::MTBDDTransitionFunction<MyVariableAssignment, std::vector<unsigned>, std::vector<unsigned>, SFTA::CUDDSharedMTBDD<unsigned, std::vector<unsigned>, MyVariableAssignment, SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator> >();
	}

	~MTBDDTransitionFunctionFixture()
	{
		delete transFunc_;
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, MTBDDTransitionFunctionFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
