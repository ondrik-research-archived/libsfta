/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test of TreeAutomaton class.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/tree_automaton.hh>
#include <sfta/compact_variable_assignment.hh>
#include <sfta/mtbdd_transition_function.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/simple_state_allocator.hh>
#include <sfta/map_state_translator.hh>
#include <sfta/vector.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/convert.hh>
#include <sfta/mtbdd_operation.hh>
using SFTA::Private::Convert;


// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TreeAutomaton
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class TreeAutomatonFixture : public LogFixture
{
private:

	TreeAutomatonFixture(const TreeAutomatonFixture& fixture);
	TreeAutomatonFixture& operator=(const TreeAutomatonFixture& rhs);

public:

	typedef SFTA::Private::CompactVariableAssignment<4> MySymbol;

	typedef SFTA::CUDDSharedMTBDD<unsigned, SFTA::OrderedVector<unsigned>, MySymbol, SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator> MyMTBDD;

	typedef SFTA::MTBDDTransitionFunction<MySymbol, unsigned, SFTA::Vector, SFTA::OrderedVector, SFTA::OrderedVector, unsigned, MyMTBDD, unsigned, SFTA::Private::SimpleStateAllocator> MTBDDTF;

	typedef SFTA::TreeAutomaton<MTBDDTF::SymbolType, std::string, MTBDDTF, SFTA::Private::MapStateTranslator> TA;

public:

	TreeAutomatonFixture()
	{
	}
};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, TreeAutomatonFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

	MySymbol symbol("0000");

	TA ta1;
	ta1.AddState("read_0000");

	TA::SetOfStatesType rhs;
	rhs.insert("read_0000");

	TA::RuleLeftHandSideType lhs;
	ta1.AddTransition(symbol, lhs, rhs);

	symbol = MySymbol("1111");
	ta1.AddState("read_1111");

	rhs.clear();
	rhs.insert("read_1111");
	ta1.AddTransition(symbol, lhs, rhs);

	symbol = MySymbol("0101");
	lhs.push_back("read_0000");
	lhs.push_back("read_1111");
	ta1.AddState("read_0000_and_1111_and after 0101");
	rhs.clear();
	rhs.insert("read_0000_and_1111_and after 0101");
	ta1.AddTransition(symbol, lhs, rhs);

	BOOST_TEST_MESSAGE("ta1: " + ta1.ToString());


//	trans->RegisterAutomaton();
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	trans->AllocateState(0);
//	MySymbol symbol("1X10");
//
//	SFTA::Vector<unsigned> leftHandSide;
//	leftHandSide.push_back(1);
//	SFTA::OrderedVector<unsigned> rightHandSide;
//	rightHandSide.push_back(1);
//	trans->AddTransition(0, symbol, leftHandSide, rightHandSide);
//
//	leftHandSide.clear();
//	leftHandSide.push_back(2);
//	rightHandSide.clear();
//	rightHandSide.push_back(3);
//	rightHandSide.push_back(4);
//	trans->AddTransition(0, symbol, leftHandSide, rightHandSide);
//
//	TA ta1(trans);
//	TA ta2(trans);
//
//	SFTA::MTBDDOperation<TA> op;
//	TA ta3 = op.Union(ta1, ta2);

//	BOOST_TEST_MESSAGE("After union: " + ta3.ToString());
}

BOOST_AUTO_TEST_CASE(serialization)
{
}

BOOST_AUTO_TEST_SUITE_END()
