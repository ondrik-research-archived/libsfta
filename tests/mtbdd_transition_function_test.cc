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
#include <sfta/vector.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/convert.hh>
using SFTA::Private::Convert;

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

	typedef SFTA::AbstractTransitionFunction<MyVariableAssignment, unsigned, SFTA::Vector, SFTA::OrderedVector, SFTA::OrderedVector> ATF;

	typedef SFTA::MTBDDTransitionFunction<MyVariableAssignment, unsigned, SFTA::Vector, SFTA::OrderedVector, SFTA::OrderedVector, SFTA::CUDDSharedMTBDD<unsigned, SFTA::OrderedVector<unsigned>, MyVariableAssignment, SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator>, unsigned> MTBDDTF;

protected:

	MTBDDTF transFunc_;

	const static size_t STATE_SET_SIZE = 10;
	const static size_t SINK_STATE = 0;

public:

	MTBDDTransitionFunctionFixture()
		: transFunc_(STATE_SET_SIZE, SINK_STATE)
	{
	}

	~MTBDDTransitionFunctionFixture()
	{
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, MTBDDTransitionFunctionFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

	// Test of nullary symbols
	MyVariableAssignment asgn("0010");
	SFTA::Vector<unsigned> leftHandSide;
	leftHandSide.push_back(1);
	SFTA::OrderedVector<unsigned> rightHandSide;
	rightHandSide.push_back(3);
	rightHandSide.push_back(4);
	rightHandSide.push_back(5);
	transFunc_.AddTransition(asgn, leftHandSide, rightHandSide);

	SFTA::OrderedVector<unsigned> outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of unary symbols
	leftHandSide.clear();
	leftHandSide.push_back(3);
	asgn = MyVariableAssignment("000X");
	rightHandSide.clear();
	rightHandSide.push_back(7);
	transFunc_.AddTransition(asgn, leftHandSide, rightHandSide);

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of binary symbols
	leftHandSide.clear();
	leftHandSide.push_back(2);
	leftHandSide.push_back(5);
	asgn = MyVariableAssignment("0001");
	rightHandSide.clear();
	rightHandSide.push_back(3);
	rightHandSide.push_back(1);
	transFunc_.AddTransition(asgn, leftHandSide, rightHandSide);

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of n-nary symbols
	leftHandSide.clear();
	leftHandSide.push_back(1);
	leftHandSide.push_back(2);
	leftHandSide.push_back(3);
	leftHandSide.push_back(4);
	asgn = MyVariableAssignment("0X0X");
	rightHandSide.clear();
	rightHandSide.push_back(6);
	rightHandSide.push_back(2);
	transFunc_.AddTransition(asgn, leftHandSide, rightHandSide);

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of non-present superstate
	leftHandSide.clear();
	leftHandSide.push_back(3);
	leftHandSide.push_back(4);
	asgn = MyVariableAssignment("0001");
	rightHandSide.clear();

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of non-present symbol for a present superstate
	leftHandSide.clear();
	leftHandSide.push_back(2);
	leftHandSide.push_back(5);
	asgn = MyVariableAssignment("0000");
	rightHandSide.clear();

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	// Test of symbol with DON'T CARE values of certain variables that covers
	// some present value
	leftHandSide.clear();
	leftHandSide.push_back(2);
	leftHandSide.push_back(5);
	asgn = MyVariableAssignment("XXXX");
	rightHandSide.clear();
	rightHandSide.push_back(3);
	rightHandSide.push_back(1);

	outputVal = transFunc_.GetTransition(asgn, leftHandSide);
	BOOST_TEST_MESSAGE("Output value: " + Convert::ToString(outputVal));
	BOOST_CHECK(rightHandSide == outputVal);

	ATF::TransitionListType tl = transFunc_.GetListOfTransitions();

	for (ATF::TransitionListType::const_iterator it = tl.begin();
		it != tl.end(); ++it)
	{
		BOOST_TEST_MESSAGE("Transition: " + Convert::ToString(Loki::Field<0>(*it))
			+ " " + Convert::ToString(Loki::Field<1>(*it)) + " "
			+ Convert::ToString(Loki::Field<2>(*it)));
	}
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
