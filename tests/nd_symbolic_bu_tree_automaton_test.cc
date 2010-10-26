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

	typedef SFTA::AbstractAutomaton
		<
			unsigned,
			SFTA::Private::CompactVariableAssignment<4>
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
			AA::SymbolType
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


	typedef NDSBUTA::StateType State;
	typedef NDSBUTA::SymbolType Symbol;
	typedef NDSBUTA::LeftHandSideType LeftHandSide;
	typedef NDSBUTA::InputRightHandSideType RightHandSide;

protected:

		Symbol SYMBOL_A;
		Symbol SYMBOL_B;

public:   // Public methods

		NDSymbolicBUTreeAutomatonFixture()
			: SYMBOL_A("0000"),
				SYMBOL_B("0001")
		{ }



	/**
	 *
	 *          ___                  ___
	 *      a  /   \       _    a   /   \
	 *    ---->|q_1|----->|_|------>|q_2|
	 *         \___/       A        \___/F
	 *                     |          |
	 *                     |----------|
	 *
	 */
	NDSBUTA* constructAutomaton1(TTW* ttWrapper) const
	{
		NDSBUTA* result = new NDSBUTA(ttWrapper);

		// insert a() -> {q_1}
		LeftHandSide lhs_empty;
		State state_q1 = result->AddState();
		RightHandSide trans_empty_a;
		trans_empty_a.insert(state_q1);
		result->AddTransition(lhs_empty, SYMBOL_A, trans_empty_a);

		// insert a(q_1) -> {q_2}
		LeftHandSide lhs_q1;
		lhs_q1.push_back(state_q1);
		State state_q2 = result->AddState();
		RightHandSide trans_q1_a;
		trans_q1_a.insert(state_q2);
		result->AddTransition(lhs_q1, SYMBOL_A, trans_q1_a);

		// insert a(q_2) -> {q_2}
		LeftHandSide lhs_q2;
		lhs_q2.push_back(state_q2);
		RightHandSide trans_q2_a = trans_q1_a;
		result->AddTransition(lhs_q2, SYMBOL_A, trans_q2_a);

		// set final states
		result->SetStateFinal(state_q2);

		return result;
	}

	/**
	 *
	 *          ___                  ___
	 *      a  /   \       _    a   /   \
	 *    ---->|q_1|----->|_|------>|q_2|
	 *         \___/       |        \___/F
	 *                     |    ___
	 *                    b|   /   \      _
	 *                     |-->|q_3|---->|_|
	 *                         \___/F     |
	 *                           A        |
	 *                           |    b   |
	 *                           |--------|
	 *
	 *
	 */
	NDSBUTA* constructAutomaton2(TTW* ttWrapper) const
	{
		NDSBUTA* result = new NDSBUTA(ttWrapper);

		// insert a() -> {q_1}
		LeftHandSide lhs_empty;
		State state_q1 = result->AddState();
		RightHandSide trans_empty_a;
		trans_empty_a.insert(state_q1);
		result->AddTransition(lhs_empty, SYMBOL_A, trans_empty_a);

		// insert a(q_1) -> {q_2}
		LeftHandSide lhs_q1;
		lhs_q1.push_back(state_q1);
		State state_q2 = result->AddState();
		RightHandSide trans_q1_a;
		trans_q1_a.insert(state_q2);
		result->AddTransition(lhs_q1, SYMBOL_A, trans_q1_a);

		// insert b(q_1) -> {q_3}
		State state_q3 = result->AddState();
		RightHandSide trans_q1_b;
		trans_q1_b.insert(state_q3);
		result->AddTransition(lhs_q1, SYMBOL_B, trans_q1_b);

		// insert b(q_3) -> {q_3}
		LeftHandSide lhs_q3;
		lhs_q3.push_back(state_q3);
		RightHandSide trans_q3_b = trans_q1_b;
		result->AddTransition(lhs_q3, SYMBOL_B, trans_q3_b);

		// set final states
		result->SetStateFinal(state_q2);
		result->SetStateFinal(state_q3);

		return result;
	}
};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, NDSymbolicBUTreeAutomatonFixture)

BOOST_AUTO_TEST_CASE(operation_union)
{
	NDSBUTA* taInvalid = new NDSBUTA();

	NDSBUTA* ta1 = constructAutomaton1(taInvalid->GetTTWrapper());

	NDSBUTA* ta2 = constructAutomaton2(taInvalid->GetTTWrapper());

	BOOST_TEST_MESSAGE("A1:\n" + ta1->ToString());
	BOOST_TEST_MESSAGE("A2:\n" + ta2->ToString());

	AAO* oper = ta1->GetOperation();
	AA* unionTa = oper->Union(ta1, ta2);

	BOOST_TEST_MESSAGE("Union automaton:\n" + unionTa->ToString());

	delete unionTa;
	delete oper;
	delete ta1;
	delete ta2;
}

BOOST_AUTO_TEST_SUITE_END()
