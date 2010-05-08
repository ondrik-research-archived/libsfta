/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for TimbukTABuilder class.
 *
 *****************************************************************************/

// SFTA header files
#include <sfta/timbuk_ta_builder.hh>
#include <sfta/ta_building_director.hh>
#include <sfta/compact_variable_assignment.hh>
#include <sfta/mtbdd_transition_function.hh>
#include <sfta/vector.hh>
#include <sfta/ordered_vector.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>
#include <sfta/simple_state_allocator.hh>
#include <sfta/tree_automaton.hh>
#include <sfta/map_state_translator.hh>
#include <sfta/mtbdd_operation.hh>

// Standard library header files
#include <fstream>

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TimbukTABuilder
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class TimbukTABuilderFixture : public LogFixture
{
private:

	TimbukTABuilderFixture(const TimbukTABuilderFixture& fixture);
	TimbukTABuilderFixture& operator=(const TimbukTABuilderFixture& rhs);

public:

	typedef SFTA::Private::CompactVariableAssignment<8> MyVariableAssignment;

	typedef SFTA::AbstractTransitionFunction<MyVariableAssignment, unsigned, SFTA::Vector, SFTA::OrderedVector, SFTA::OrderedVector, unsigned> ATF;

	typedef SFTA::MTBDDTransitionFunction<MyVariableAssignment, unsigned, SFTA::Vector, SFTA::OrderedVector, SFTA::OrderedVector, unsigned, SFTA::CUDDSharedMTBDD<unsigned, SFTA::OrderedVector<unsigned>, MyVariableAssignment, SFTA::Private::MapLeafAllocator, SFTA::Private::MapRootAllocator>, unsigned, SFTA::Private::SimpleStateAllocator> MTBDDTF;

	typedef SFTA::TreeAutomaton<MTBDDTF::SymbolType, std::string, MTBDDTF, SFTA::Private::MapStateTranslator> TAType;

	typedef SFTA::TimbukTABuilder<TAType> Builder;
	typedef SFTA::TABuildingDirector<TAType> Director;

	typedef Director::AbsTABPtr BuilderPtr;

	typedef SFTA::SymbolDictionary<std::string, MyVariableAssignment> SymDict;

	typedef Loki::SmartPtr<SymDict> SymDictPtr;

protected:


public:

	TimbukTABuilderFixture()
	{
	}

	~TimbukTABuilderFixture()
	{
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, TimbukTABuilderFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

	SymDictPtr dict(new SymDict);
	SFTA::MTBDDOperation<TAType> op;

//	BuilderPtr builder1(new Builder(dict));
//	Director director1(builder1);
//	std::ifstream file1("automata/finite");
//	TAType ta1 = director1.Construct(file1);
//
//	BuilderPtr builder2(new Builder(dict));
//	Director director2(builder2, ta1.GetTransitionFunction());
//	std::ifstream file2("automata/infinite");
//	TAType ta2 = director2.Construct(file2);
//	BOOST_TEST_MESSAGE("Result 1:\n" + ta1.ToString());
//	BOOST_TEST_MESSAGE("Result 2:\n" + ta2.ToString());
//
//	TAType ta3 = op.Union(ta1, ta2);
//	BOOST_TEST_MESSAGE("After union:\n" + ta3.ToString());
//
//	TAType ta4 = op.Intersection(ta1, ta2);
//	BOOST_TEST_MESSAGE("After intersection:\n" + ta4.ToString());

	BuilderPtr builder3(new Builder(dict));
//	Director director3(builder3, ta1.GetTransitionFunction());
	Director director3(builder3);
	std::ifstream file3("automata/for_simulation");
	TAType ta5 = director3.Construct(file3);
	BOOST_TEST_MESSAGE("Before reduction:\n" + ta5.ToString());
	TAType ta6 = op.SimulationReduction(ta5);
	BOOST_TEST_MESSAGE("After reduction:\n" + ta6.ToString());
}

BOOST_AUTO_TEST_SUITE_END()
