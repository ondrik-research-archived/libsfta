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
#include <sfta/dual_map_leaf_allocator.hh>
#include <sfta/map_root_allocator.hh>

using SFTA::AbstractSharedMTBDD;
using SFTA::CUDDSharedMTBDD;
using SFTA::Private::Convert;
using SFTA::Private::FormulaParser;


// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDSharedMTBDDCC
#include <boost/test/unit_test.hpp>
#include <boost/random/mersenne_twister.hpp>

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
	"~x0 * ~x1 *  x2 *  x3 =  3",
	"~x0 *  x1 * ~x2 * ~x3 =  4",
	" x0 * ~x1 * ~x2 *  x3 =  9",
	" x0 *  x1 *  x2 * ~x3 = 14",
	" x0 *  x1 *  x2 * ~x3 = 14",
	" x0 *  x1 *  x2 *  x3 = 15"
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
	"~x0 * ~x1 * ~x2 *  x3 =  1",
	"~x0 * ~x1 *  x2 * ~x3 =  2",
	"~x0 *  x1 * ~x2 *  x3 =  5",
	"~x0 *  x1 *  x2 * ~x3 =  6",
	"~x0 *  x1 *  x2 *  x3 =  7",
	" x0 * ~x1 * ~x2 * ~x3 =  8",
	" x0 * ~x1 *  x2 * ~x3 = 10",
	" x0 * ~x1 *  x2 *  x3 = 11",
	" x0 *  x1 * ~x2 * ~x3 = 12",
	" x0 *  x1 * ~x2 *  x3 = 13"
};


/**
 * Number of formulae for standard test cases that are to be not found in the
 * MTBDD
 */
const unsigned STANDARD_FAIL_CASES_SIZE =
	sizeof(STANDARD_FAIL_CASES) / sizeof(const char* const);

/**
 * Number of variables of the MTBDD
 */
const unsigned NUM_VARIABLES = 64;

/**
 * The seed of the pseudorandom number generator
 */
const unsigned PRNG_SEED = 781436;

/**
 * Number of cases for large test formula test
 */
const unsigned LARGE_TEST_FORMULA_CASES = 200;


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDSharedMTBDDCharCharFixture : public LogFixture
{
private:  // private constants

public:   // public types

	/**
	 * @brief  Root type
	 *
	 * The type of MTBDD root
	 */
	typedef unsigned char RootType;

	/**
	 * @brief  Leaf type
	 *
	 * The type of MTBDD leaf
	 */
	typedef unsigned char LeafType;


	/**
	 * @brief  List of test cases
	 *
	 * List of strings with formulae for test cases.
	 */
	typedef std::vector<std::string> ListOfTestCasesType;

	typedef SFTA::Private::CompactVariableAssignment<NUM_VARIABLES> MyVariableAssignment;

	typedef AbstractSharedMTBDD<RootType, LeafType, MyVariableAssignment> ASMTBDDCC;

	typedef CUDDSharedMTBDD<RootType, LeafType, MyVariableAssignment,
			SFTA::Private::DualMapLeafAllocator, SFTA::Private::MapRootAllocator>
			CuddMTBDDCC;

	typedef std::map<std::string, unsigned> VariableNameDictionary;

private:  // private data members


	/**
	 * Counter of known variables.
	 */
	unsigned varCounter_;

	VariableNameDictionary varDict_;

private:  // private methods

	CUDDSharedMTBDDCharCharFixture(
		const CUDDSharedMTBDDCharCharFixture& fixture);
	CUDDSharedMTBDDCharCharFixture& operator=(
		const CUDDSharedMTBDDCharCharFixture& rhs);

public:   // public methods

	CUDDSharedMTBDDCharCharFixture()
		: varCounter_(0), varDict_()
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
		for (size_t i = 0; i < STANDARD_TEST_CASES_SIZE; ++i)
		{	// load test cases
			testCases.push_back(STANDARD_TEST_CASES[i]);
		}

		// formulae that we want to check that are not in the BDD
		for (unsigned i = 0; i < STANDARD_FAIL_CASES_SIZE; ++i)
		{	// load test cases
			failedCases.push_back(STANDARD_FAIL_CASES[i]);
		}
	}


	size_t translateVarNameToIndex(const std::string& varName)
	{
		VariableNameDictionary::const_iterator itDict;
		if ((itDict = varDict_.find(varName)) == varDict_.end())
		{	// in case the variable was not found in the dictionary
			itDict = varDict_.insert(std::make_pair(varName, varCounter_++)).first;
		}

		return itDict->second;
	}


	MyVariableAssignment varListToAsgn(
		const FormulaParser::VariableListType& varList)
	{
		MyVariableAssignment asgn;

		for (FormulaParser::VariableListType::const_iterator itVar =
			varList.begin(); itVar != varList.end(); ++itVar)
		{	// for each variable in the list, change the corresponding assignment
			size_t index = translateVarNameToIndex(itVar->first);
			asgn.SetIthVariableValue(index, (itVar->second)?
				MyVariableAssignment::ONE : MyVariableAssignment::ZERO);
		}

		return asgn;
	}

	static std::string leafContainerToString(const ASMTBDDCC::LeafContainer& leafCont)
	{
		std::string result;

		for (ASMTBDDCC::LeafContainer::const_iterator itLeaf = leafCont.begin();
			itLeaf != leafCont.end(); ++itLeaf)
		{	// append all leaves
			result += ((itLeaf == leafCont.begin())? " " : ", ") +
				Convert::ToString(static_cast<unsigned>(**itLeaf));
		}

		return result;
	}

	inline static bool compareLeafValues(const LeafType* lhs, const LeafType* rhs)
	{
		return *lhs == *rhs;
	}

	static bool compareTwoLeafContainers(const ASMTBDDCC::LeafContainer& lhs,
		const ASMTBDDCC::LeafContainer& rhs)
	{
		const ASMTBDDCC::LeafContainer* left = &lhs;
		const ASMTBDDCC::LeafContainer* right = &rhs;

		if (left->size() != right->size())
		{	// in case the sizes differ
			return false;
		}

		return std::equal(left->begin(), left->end(), right->begin(), compareLeafValues);
	}

	RootType createMTBDDForTestCases(ASMTBDDCC* bdd,
		const ListOfTestCasesType& testCases)
	{
		RootType root = bdd->CreateRoot();

		for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
			itTests != testCases.end(); ++itTests)
		{
			FormulaParser::ParserResultUnsignedType prsRes =
				FormulaParser::ParseExpressionUnsigned(*itTests);
			LeafType leafValue = static_cast<LeafType>(prsRes.first);
			MyVariableAssignment asgn = varListToAsgn(prsRes.second);
			bdd->SetValue(root, asgn, leafValue);
		}

		return root;
	}
};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/


BOOST_FIXTURE_TEST_SUITE(suite, CUDDSharedMTBDDCharCharFixture)

BOOST_AUTO_TEST_CASE(setters_and_getters_test)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(testCases[i]);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			testCases[i] + " != " + leafContainerToString(bdd->GetValue(root, asgn)));

		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
			itFailed != failedCases.end(); ++itFailed)
		{	// for every test case that should fail
			FormulaParser::ParserResultUnsignedType prsFailedRes =
				FormulaParser::ParseExpressionUnsigned(*itFailed);
			MyVariableAssignment failedAsgn = varListToAsgn(prsFailedRes.second);

			ASMTBDDCC::LeafContainer resFailed;

			BOOST_CHECK_MESSAGE(
				compareTwoLeafContainers(bdd->GetValue(root, failedAsgn), resFailed),
				testCases[i] + " == " + leafContainerToString(bdd->GetValue(root, failedAsgn)));
		}
	}

	delete bdd;
}

BOOST_AUTO_TEST_CASE(large_diagram_test)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	boost::mt19937 prnGen(PRNG_SEED);

	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate test cases
		std::string formula;

		for (unsigned j = 0; j < NUM_VARIABLES; ++j)
		{
			if (prnGen() % 4 != 0)
			{
				formula += (formula.empty()? "" : " * ") +
					Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		LeafType randomNum;
		while ((randomNum = prnGen()) == 0) ;   // generate non-zero random number

		formula += " = " + Convert::ToString(static_cast<unsigned>(randomNum));

		testCases.push_back(formula);
	}

	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate failed test cases
		std::string formula;

		for (unsigned j = 0; j < NUM_VARIABLES; ++j)
		{
			if (prnGen() % 31 != 0)
			{
				formula += (formula.empty()? "" : " * ") +
					Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		formula += " = " + Convert::ToString(static_cast<unsigned>(1));

		failedCases.push_back(formula);
	}

	RootType root = createMTBDDForTestCases(bdd, testCases);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
		itFailed != failedCases.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedType prsFailedRes =
			FormulaParser::ParseExpressionUnsigned(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDCC::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(no_variables_formula)
{
	const char* const TEST_VALUE = " = 42";

	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);
	RootType root = bdd->CreateRoot();

	FormulaParser::ParserResultUnsignedType prsRes =
		FormulaParser::ParseExpressionUnsigned(TEST_VALUE);
	LeafType leafValue = static_cast<LeafType>(prsRes.first);
	MyVariableAssignment asgn = varListToAsgn(prsRes.second);
	bdd->SetValue(root, asgn, leafValue);

	ASMTBDDCC::LeafContainer res;
	res.push_back(&leafValue);

	BOOST_CHECK_MESSAGE(compareTwoLeafContainers(bdd->GetValue(root, asgn), res),
		Convert::ToString(TEST_VALUE) + " != " +
		leafContainerToString(bdd->GetValue(root, asgn)));

	delete bdd;
}


BOOST_AUTO_TEST_CASE(multiple_independent_bdds)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	// load test cases for the first BDD
	ListOfTestCasesType testCases1;
	ListOfTestCasesType failedCases1;
	loadStandardTests(testCases1, failedCases1);

	// load test cases for the second BDD
	ListOfTestCasesType testCases2;
	ListOfTestCasesType failedCases2;
	loadStandardTests(failedCases2, testCases2);

	RootType root1 = createMTBDDForTestCases(bdd, testCases1);
	RootType root2 = createMTBDDForTestCases(bdd, testCases2);

	for (ListOfTestCasesType::const_iterator itTests = testCases1.begin();
		itTests != testCases1.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root1, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root1, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases2.begin();
		itTests != testCases2.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root2, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(root2, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases1.begin();
		itFailed != failedCases1.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedType prsFailedRes =
			FormulaParser::ParseExpressionUnsigned(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDCC::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root1, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root1, asgn)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases2.begin();
		itFailed != failedCases2.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		FormulaParser::ParserResultUnsignedType prsFailedRes =
			FormulaParser::ParseExpressionUnsigned(*itFailed);
		MyVariableAssignment asgn = varListToAsgn(prsFailedRes.second);

		ASMTBDDCC::LeafContainer res;

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(root2, asgn), res),
			*itFailed + " == " + leafContainerToString(bdd->GetValue(root2, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(monadic_apply)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	// apply functor that squares values in leaves
	class SquareMonadicApplyFunctor
		: public ASMTBDDCC::AbstractMonadicApplyFunctorType
	{
	public:

		virtual LeafType operator()(const LeafType& val)
		{
			return val * val;
		}
	};

	SquareMonadicApplyFunctor func;

	RootType squaredRoot = bdd->MonadicApply(root, &func);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		leafValue *= leafValue;
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(squaredRoot, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(squaredRoot, asgn)));
	}


	delete bdd;
}


BOOST_AUTO_TEST_CASE(apply)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	// apply functor that squares values in leaves
	class TimesApplyFunctor
		: public ASMTBDDCC::AbstractApplyFunctorType
	{
	public:

		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			return lhs * rhs;
		}
	};

	TimesApplyFunctor func;

	RootType timesRoot = bdd->Apply(root, root, &func);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		leafValue *= leafValue;
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(timesRoot, asgn), res),
			*itTests + " != " + leafContainerToString(bdd->GetValue(timesRoot, asgn)));
	}

	delete bdd;
}


BOOST_AUTO_TEST_CASE(variable_renaming)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	for (unsigned i = 0; i < NUM_VARIABLES; ++i)
	{	// fill the table of variables
		translateVarNameToIndex("x" + Convert::ToString(i));
	}

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	class MovingUpVariableRenamingFunctor
		: public ASMTBDDCC::AbstractVariableRenamingFunctorType
	{
	public:

		virtual ASMTBDDCC::VariableType operator()(const ASMTBDDCC::VariableType& var)
		{
			if (var < NUM_VARIABLES/2)
			{
				return var + NUM_VARIABLES/2;
			}
			else
			{
				return var;
			}
		}
	};

	MovingUpVariableRenamingFunctor funcMovingUp;
	RootType renamedRoot = bdd->RenameVariables(root, &funcMovingUp);

	class MovingDownVariableRenamingFunctor
		: public ASMTBDDCC::AbstractVariableRenamingFunctorType
	{
	public:

		virtual ASMTBDDCC::VariableType operator()(const ASMTBDDCC::VariableType& var)
		{
			if ((var >= NUM_VARIABLES/2) && (var < NUM_VARIABLES))
			{
				return 2 * (var - NUM_VARIABLES/2) + 1;
			}
			else
			{
				return var;
			}
		}
	};

	MovingDownVariableRenamingFunctor funcMovingDown;
	renamedRoot = bdd->RenameVariables(renamedRoot, &funcMovingDown);


	ListOfTestCasesType renamedTestCases;
	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// for each test case create new with properly renamed variables
		std::string renamedCase = *itTests;

		for (unsigned i = 0; i < NUM_VARIABLES/2; ++i)
		{	// double the name of each variable, 
			boost::algorithm::replace_all(renamedCase, "x" + Convert::ToString(i) + " ",
				"x" + Convert::ToString(i+NUM_VARIABLES/2) + " ");
		}

		for (unsigned i = NUM_VARIABLES/2; i < NUM_VARIABLES; ++i)
		{	// for each variable
			boost::algorithm::replace_all(renamedCase, "x" + Convert::ToString(i) + " ",
				"x" + Convert::ToString(2*(i-static_cast<int>(NUM_VARIABLES)/2)+1) + " ");
		}

		renamedTestCases.push_back(renamedCase);
	}

	for (ListOfTestCasesType::const_iterator itTests = renamedTestCases.begin();
		itTests != renamedTestCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		FormulaParser::ParserResultUnsignedType prsRes =
			FormulaParser::ParseExpressionUnsigned(*itTests);
		LeafType leafValue = static_cast<LeafType>(prsRes.first);
		MyVariableAssignment asgn = varListToAsgn(prsRes.second);

		ASMTBDDCC::LeafContainer res;
		res.push_back(&leafValue);

		BOOST_CHECK_MESSAGE(
			compareTwoLeafContainers(bdd->GetValue(renamedRoot, asgn), res), *itTests
			+ " != " + leafContainerToString(bdd->GetValue(renamedRoot, asgn)));
	}


	delete bdd;
}

BOOST_AUTO_TEST_CASE(variable_trimming)
{
	ASMTBDDCC* bdd = new CuddMTBDDCC();
	bdd->SetBottomValue(0);

	for (unsigned i = 0; i < NUM_VARIABLES; ++i)
	{	// fill the table of variables
		translateVarNameToIndex("x" + Convert::ToString(i));
	}

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	RootType root = createMTBDDForTestCases(bdd, testCases);

	class OddVariablePredicateFunctorType
		: public ASMTBDDCC::AbstractVariablePredicateFunctorType
	{
	public:

		virtual bool operator()(const ASMTBDDCC::VariableType& var)
		{
			return var % 2 == 1;
		}
	};

	OddVariablePredicateFunctorType oddPred;

	class AdditionApplyFunctorType: public ASMTBDDCC::AbstractApplyFunctorType
	{
	public:
		virtual LeafType operator()(const LeafType& lhs, const LeafType& rhs)
		{
			return lhs + rhs;
		}
	};

	AdditionApplyFunctorType addApply;

	RootType newRoot = bdd->TrimVariables(root, &oddPred, &addApply);

	delete bdd;
}

//BOOST_AUTO_TEST_CASE(serialization)
//{
//	ASMTBDDCC* bdd = new CuddMTBDDCC();
//
//	// load test cases
//	ListOfTestCasesType testCases;
//	ListOfTestCasesType failedCases;
//	loadStandardTests(testCases, failedCases);
//
//	RootType root = createMTBDDForTestCases(bdd, testCases);
//
//	std::string str = bdd->Serialize();
//
//	BOOST_TEST_MESSAGE("BDD:\n" + str);
//
//	delete bdd;
//}

BOOST_AUTO_TEST_SUITE_END()
