/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for NDSymbolicBUTreeAutomaton.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/mtbdd_transition_table_wrapper.hh>
#include <sfta/nd_symbolic_bu_tree_automaton.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/set.hh>
#include <sfta/vector.hh>

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE NDSymbolicBUTreeAutomaton
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"



/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

/**
 * @brief  NDSymbolicBUTreeAutomaton test fixture
 *
 * Fixture for test of NDSymbolicBUTreeAutomaton.
 */
class NDSymbolicBUTreeAutomatonFixture : public LogFixture
{
public:   // Private data types

	typedef SFTA::Private::CompactVariableAssignment<32> MySymbol;

	typedef SFTA::AbstractAutomaton
		<
			unsigned,
			MySymbol
		> AA;

	typedef SFTA::AbstractBUTreeAutomaton
		<
			AA::StateType,
			AA::SymbolType,
			SFTA::Set<AA::StateType>
		> ABUTA;

	typedef SFTA::AbstractSharedMTBDD
		<
			unsigned,
			SFTA::OrderedVector<unsigned>,
			MySymbol
		> AbstractMTBDDType;

	typedef SFTA::CUDDSharedMTBDD
		<
			AbstractMTBDDType::RootType,
			AbstractMTBDDType::LeafType,
			AbstractMTBDDType::VariableAssignmentType,
			SFTA::Private::DualMapLeafAllocator,
			SFTA::Private::MapRootAllocator
		> MTBDDType;

	typedef SFTA::MTBDDTransitionTableWrapper
		<
			AA::StateType,
			MTBDDType
		> TTW;

	typedef SFTA::SymbolicBUTreeAutomaton
		<
			TTW,
			ABUTA::StateType,
			ABUTA::SymbolType,
			ABUTA::InputRightHandSideType
		> SBUTA;

	typedef SFTA::NDSymbolicBUTreeAutomaton
		<
			TTW,
			SBUTA::StateType,
			SBUTA::SymbolType,
			SFTA::OrderedVector
		> NDSBUTA;

	typedef AA::Operation AAO;


};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, NDSymbolicBUTreeAutomatonFixture)

BOOST_AUTO_TEST_CASE(adding_transitions)
{
	AA* ta = new NDSBUTA();
	AAO* oper = ta->GetOperation();
	AA* unionTa = oper->Union(ta, ta);

	BOOST_TEST_MESSAGE("Input automaton:\n" + ta->ToString());
	BOOST_TEST_MESSAGE("Union automaton:\n" + unionTa->ToString());

	delete unionTa;
	delete oper;
	delete ta;
}

BOOST_AUTO_TEST_SUITE_END()
