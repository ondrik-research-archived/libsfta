/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDSharedMTBDD class with roots as unsigned integers and
 *    leaves as ordered vectors of unsigned integers.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/mtbdd_transition_table_wrapper.hh>
#include <sfta/nd_symbolic_td_tree_automaton.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/set.hh>
#include <sfta/vector.hh>


// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDDCC
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"



/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/



/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class NDSymbolicTDTreeAutomatonFixture : public LogFixture
{
private:  // private constants

	const static size_t SymbolLength = 64;

public:   // public types

	typedef unsigned StateType;
	typedef StateType LeftHandSideType;
	typedef SFTA::OrderedVector<SFTA::Vector<StateType> > RightHandSideType;
	typedef SFTA::Private::CompactVariableAssignment<SymbolLength> SymbolType;

	typedef SFTA::CUDDSharedMTBDD
	<
		LeftHandSideType,
		RightHandSideType,
		SymbolType,
		SFTA::Private::DualMapLeafAllocator,
		SFTA::Private::MapRootAllocator
	> SharedMTBDD;


	typedef SFTA::MTBDDTransitionTableWrapper
	<
		StateType,
		SharedMTBDD
	> MTBDDTransitionTableWrapper;

	typedef SFTA::AbstractAutomaton
	<
		StateType,
		SymbolType
	> AbstractAutomaton;

	typedef SFTA::NDSymbolicTDTreeAutomaton
	<
		MTBDDTransitionTableWrapper,
		StateType,
		SymbolType,
		SFTA::OrderedVector
	> NDSymbolicTDTreeAutomaton;

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, NDSymbolicTDTreeAutomatonFixture)

BOOST_AUTO_TEST_CASE(basic_test)
{
	AbstractAutomaton* aut = new NDSymbolicTDTreeAutomaton();
}

BOOST_AUTO_TEST_SUITE_END()
