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
#include <sfta/compact_variable_assignment.hh>
#include <sfta/cudd_shared_mtbdd.hh>
#include <sfta/formula_parser.hh>
#include <sfta/map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>

using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;
using SFTA::Private::FormulaParser;


// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDDCC
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/


/**
 * Formulae for standard test cases to be stored in the MTBDD
 */
const char* const STANDARD_TEST_CASES[] =
{
	"~x1 * ~x2 *  x3 *  x4 =  3",
	"~x1 *  x2 * ~x3 * ~x4 =  4",
	" x1 * ~x2 * ~x3 *  x4 =  9",
	" x1 *  x2 *  x3 * ~x4 = 14",
	" x1 *  x2 *  x3 * ~x4 = 14",
	" x1 *  x2 *  x3 *  x4 = 15"
};

/**
 * Number of formulae for standard test cases in the MTBDD
 */
const unsigned STANDARD_TEST_CASES_SIZE =
	sizeof(STANDARD_TEST_CASES) / sizeof(const char* const);


/**
 * Formulae for standard test cases to be found not present in the MTBDD
 */
const char* const STANDARD_FAIL_CASES[] =
{
	"~x1 * ~x2 * ~x3 *  x4 =  1",
	"~x1 * ~x2 *  x3 * ~x4 =  2",
	"~x1 *  x2 * ~x3 *  x4 =  5",
	"~x1 *  x2 *  x3 * ~x4 =  6",
	"~x1 *  x2 *  x3 *  x4 =  7",
	" x1 * ~x2 * ~x3 * ~x4 =  8",
	" x1 * ~x2 *  x3 * ~x4 = 10",
	" x1 * ~x2 *  x3 *  x4 = 11",
	" x1 *  x2 * ~x3 * ~x4 = 12",
	" x1 *  x2 * ~x3 *  x4 = 13"
};


/**
 * Number of formulae for standard test cases that are to be not found in the
 * MTBDD
 */
const unsigned STANDARD_FAIL_CASES_SIZE =
	sizeof(STANDARD_FAIL_CASES) / sizeof(const char* const);


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDCharCharFixture : public LogFixture
{
public:   // public types

	/**
	 * @brief  List of test cases
	 *
	 * List of strings with formulae for test cases.
	 */
	typedef std::vector<std::string> ListOfTestCasesType;


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

protected:// protected methods

	/**
	 * @brief  Loads standard tests
	 *
	 * A routine that loads passed structures with standard tests.
	 *
	 * @param[out]  testCases    Reference to list of test cases that is to be
	 *                           filled
	 * @param[out]  failedCases  Reference to list of failing test cases that is
	 *                           to be filled
	 */
	static void loadStandardTests(ListOfTestCasesType& testCases,
		ListOfTestCasesType& failedCases)
	{
		// formulae that we wish to store in the BDD
		for (unsigned i = 0; i < STANDARD_TEST_CASES_SIZE; ++i)
		{	// load test cases
			testCases.push_back(STANDARD_TEST_CASES[i]);
		}

		// formulae that we want to check that are not in the BDD
		for (unsigned i = 0; i < STANDARD_FAIL_CASES_SIZE; ++i)
		{	// load test cases
			failedCases.push_back(STANDARD_FAIL_CASES[i]);
		}
	}


	static MyVariableAssignment varListToAsgn(
		const FormulaParser::VariableListType& varList)
	{
		// TODO
		return MyVariableAssignment();
	}
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

	char root = bdd->CreateRoot();

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		char leafValue = static_cast<char>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);
		bdd->SetValue(root, asgn, leafValue);
	}

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(testCases[i]);
		char leafValue = static_cast<char>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

//		BOOST_CHECK_MESSAGE(getValue(facade, testRootNodes[i], prsRes.second)
//			== prsRes.first, testCases[i] + " != "
//			+ Convert::ToString(getValue(facade, testRootNodes[i], prsRes.second)));
//
//		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
//			itFailed != failedCases.end(); ++itFailed)
//		{	// for every test case that should fail
//			FormulaParser::ParserResultUnsignedType prsFailedRes =
//				FormulaParser::ParseExpressionUnsigned(*itFailed);
//			BOOST_CHECK_MESSAGE(getValue(facade, testRootNodes[i],
//				prsFailedRes.second) == BDD_BACKGROUND_VALUE, testCases[i] + " == "
//				+ Convert::ToString(getValue(
//					facade, testRootNodes[i], prsFailedRes.second)));
//		}
	}

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
