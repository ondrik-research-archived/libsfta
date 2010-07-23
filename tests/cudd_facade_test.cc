/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDFacade class.
 *
 *****************************************************************************/
#include <sfta/convert.hh>
#include <sfta/cudd_facade.hh>
using SFTA::Private::CUDDFacade;
using SFTA::Private::Convert;

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDFacade
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/random/mersenne_twister.hpp>

// testing headers
#include "log_fixture.hh"

//#define DEBUG 1


/******************************************************************************
 *                                  Constants                                 *
 ******************************************************************************/

/**
 * Formulae for standard test cases to be stored in the MTBDD
 */
const char* const STANDARD_TEST_CASES[] =
{
	" 3 * ~x1 * ~x2 *  x3 *  x4",
	" 4 * ~x1 *  x2 * ~x3 * ~x4",
	" 9 *  x1 * ~x2 * ~x3 *  x4",
	"14 *  x1 *  x2 *  x3 * ~x4",
	"14 *  x1 *  x2 *  x3 * ~x4",
	"15 *  x1 *  x2 *  x3 *  x4"
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
	" 1 * ~x1 * ~x2 * ~x3 *  x4",
	" 2 * ~x1 * ~x2 *  x3 * ~x4",
	" 5 * ~x1 *  x2 * ~x3 *  x4",
	" 6 * ~x1 *  x2 *  x3 * ~x4",
	" 7 * ~x1 *  x2 *  x3 *  x4",
	" 8 *  x1 * ~x2 * ~x3 * ~x4",
	"10 *  x1 * ~x2 *  x3 * ~x4",
	"11 *  x1 * ~x2 *  x3 *  x4",
	"12 *  x1 *  x2 * ~x3 * ~x4",
	"13 *  x1 *  x2 * ~x3 *  x4"
};


/**
 * Number of formulae for standard test cases that are to be not found in the
 * MTBDD
 */
const unsigned STANDARD_FAIL_CASES_SIZE =
	sizeof(STANDARD_FAIL_CASES) / sizeof(const char* const);


/**
 * The seed of the pseudorandom number generator
 */
const unsigned PRNG_SEED = 781436;

/**
 * Parameters of the test of large formulae
 */
const unsigned LARGE_TEST_FORMULA_LENGTH = 64;
const unsigned LARGE_TEST_FORMULA_CASES = 200;


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

/**
 * @brief  Fixture for the test of CUDDFacade
 *
 */
class CUDDFacadeFixture : public LogFixture
{
public:  // Public types

	typedef unsigned ValueType;
	typedef std::map<std::string, unsigned> StringToUnsignedDictType;
	typedef std::pair<std::string, bool> VariableOccurenceType;
	typedef std::vector<VariableOccurenceType> VariableListType;
	typedef std::pair<ValueType, VariableListType> ParserResultType;
	typedef std::vector<std::string> ListOfTestCasesType;
	typedef std::vector<CUDDFacade::Node*> NodeArrayType;
	typedef std::vector<ValueType> ListOfValuesType;

	class PlusApplyFunctor : public CUDDFacade::AbstractApplyFunctor
	{
	public:

		virtual ValueType operator()(const ValueType& lhs, const ValueType& rhs)
		{
			return (lhs != BDD_BACKGROUND_VALUE)? lhs : rhs;
		}
	};

protected:

	StringToUnsignedDictType varToNumDict_;

	unsigned varCounter_;

	static const ValueType BDD_BACKGROUND_VALUE = 0;
	static const ValueType BDD_TRUE_VALUE = 1;

protected:

	CUDDFacade::Node* extendBddByVariable(CUDDFacade& facade,
		CUDDFacade::Node* bdd, const VariableOccurenceType& var)
	{
		// Assertions
		assert(bdd != static_cast<CUDDFacade::Node*>(0));

		const std::string& varName = var.first;
		const bool& isPositive = var.second;

		StringToUnsignedDictType::const_iterator itDict;
		if ((itDict = varToNumDict_.find(varName)) == varToNumDict_.end())
		{	// in case the variable was not found in the dictionary
			itDict = varToNumDict_.insert(std::make_pair(varName, varCounter_++)).first;
		}

		// create the node for the new variable
		unsigned varNum = itDict->second;
		CUDDFacade::Node* varNode = facade.AddIthVar(varNum);
		facade.Ref(varNode);

		if (!isPositive)
		{	// when the variable is complemented
			CUDDFacade::Node* oldVar = varNode;
			varNode = facade.AddCmpl(varNode);
			facade.Ref(varNode);
			facade.RecursiveDeref(oldVar);
		}

		// carry out the conjunction operation
		CUDDFacade::Node* newBdd = facade.Times(bdd, varNode);
		facade.Ref(newBdd);
		facade.RecursiveDeref(bdd);
		facade.RecursiveDeref(varNode);

		return newBdd;
	}


	CUDDFacade::Node* extendBddByVariableList(CUDDFacade& facade,
		CUDDFacade::Node* bdd, const VariableListType& asgn)
	{
		// Assertions
		assert(bdd != static_cast<CUDDFacade::Node*>(0));

		for (VariableListType::const_iterator it = asgn.begin();
			it != asgn.end(); ++it)
		{	// for each variable in the assignment
			bdd = extendBddByVariable(facade, bdd, *it);
		}

		return bdd;
	}


	CUDDFacade::Node* setValue(CUDDFacade& facade, ValueType value,
		const VariableListType& asgn)
	{
		// create a node with the value
		CUDDFacade::Node* node = facade.AddConst(value);
		facade.Ref(node);

		// create the BDD above the value
		return extendBddByVariableList(facade, node, asgn);
	}


	ValueType getValue(CUDDFacade& facade, CUDDFacade::Node* rootNode,
		const VariableListType& asgn)
	{
		// create a node with the TRUE value
		CUDDFacade::Node* node = facade.AddConst(BDD_TRUE_VALUE);
		facade.Ref(node);

		node = extendBddByVariableList(facade, node, asgn);

		class CollectorApplyFunctor : public CUDDFacade::AbstractApplyFunctor
		{
		private:

			ValueType val_;

		public:

			CollectorApplyFunctor()
				: val_(BDD_BACKGROUND_VALUE)
			{ }

			virtual ValueType operator()(const ValueType& lhs, const ValueType& rhs)
			{
				if (rhs == BDD_TRUE_VALUE)
				{	// in case we are at the right point
					if (lhs != BDD_BACKGROUND_VALUE)
					{	// in case there is something sensible stored
						if ((val_ == BDD_BACKGROUND_VALUE) || (val_ == lhs))
						{	// in case we haven't collected any value yet
							val_ = lhs;
						}
						else
						{	// in case we are overwriting an existing value
							BOOST_TEST_MESSAGE("val_ = " + Convert::ToString(val_)
							  + ", lhs = " + Convert::ToString(lhs));
							throw std::logic_error("Collecting multiple values");
						}
					}
				}

				return 0;
			}

			ValueType GetValue() const
			{
				return val_;
			}
		};

		CollectorApplyFunctor func;
		CUDDFacade::Node* tmpNode = facade.Apply(rootNode, node, &func);
		facade.RecursiveDeref(tmpNode);
		facade.RecursiveDeref(node);

		return func.GetValue();
	}


	/*
	 * format:   5*x1*x2*~x3*x4
	 */
	static ParserResultType parseExpression(std::string input)
	{
		boost::trim(input);
		std::vector<std::string> splitInput;
		boost::algorithm::split(splitInput, input, boost::is_any_of("*"));

		if (splitInput.empty())
		{	// in case the splitting failed
			throw std::runtime_error("boost::algorithm::split() returned an invalid result");
		}

		VariableListType varList;
		for (std::vector<std::string>::const_iterator it = splitInput.begin() + 1;
			it != splitInput.end(); ++it)
		{
			std::string str = boost::trim_copy(*it);

			if (str.length() == 0)
			{
				throw std::invalid_argument("parseExpression: invalid argument");
			}

			VariableOccurenceType var("", true);

			if (str[0] == '~')
			{	// in case the variable is complemented
				var.second = false;
				str = str.substr(1);
			}

			var.first = str;

			varList.push_back(var);
		}

		return ParserResultType(Convert::FromString<ValueType>(splitInput[0]),
			varList);
	}

	static std::string parserResultToString(const ParserResultType& prsRes)
	{
		std::string result = Convert::ToString(prsRes.first);

		for (VariableListType::const_iterator itVars = prsRes.second.begin();
			itVars != prsRes.second.end(); ++itVars)
		{	// print all variables
			result += " * " + Convert::ToString(((itVars->second)? " " : "~"))
				+ itVars->first;
		}

		return result;
	}

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


public:

	CUDDFacadeFixture()
		: varToNumDict_(), varCounter_(0)
	{
		boost::unit_test::unit_test_log.set_threshold_level(
			boost::unit_test::log_messages);
	}

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDFacadeFixture)

BOOST_AUTO_TEST_CASE(single_value_storage_test)
{
	CUDDFacade facade;

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	NodeArrayType testRootNodes;

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		ParserResultType prsRes = parseExpression(*itTests);
		testRootNodes.push_back(setValue(facade, prsRes.first, prsRes.second));
	}

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		ParserResultType prsRes = parseExpression(testCases[i]);
		BOOST_CHECK_MESSAGE(getValue(facade, testRootNodes[i], prsRes.second)
			== prsRes.first, testCases[i] + " != "
			+ Convert::ToString(getValue(facade, testRootNodes[i], prsRes.second)));

		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
			itFailed != failedCases.end(); ++itFailed)
		{	// for every test case that should fail
			ParserResultType prsFailedRes = parseExpression(*itFailed);
			BOOST_CHECK_MESSAGE(getValue(facade, testRootNodes[i], prsFailedRes.second)
				== BDD_BACKGROUND_VALUE, testCases[i] + " == "
				+ Convert::ToString(getValue(
					facade, testRootNodes[i], prsFailedRes.second)));
		}
	}

	for (NodeArrayType::iterator itNodes = testRootNodes.begin();
		itNodes != testRootNodes.end(); ++itNodes)
	{	// dereference the nodes
		facade.RecursiveDeref(*itNodes);
	}
}


BOOST_AUTO_TEST_CASE(composed_values_storage_test)
{
	CUDDFacade facade;

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	CUDDFacade::Node* node = facade.ReadBackground();
	facade.Ref(node);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		CUDDFacade::Node* oldNode = node;

		ParserResultType prsRes = parseExpression(*itTests);
		CUDDFacade::Node* tmpNode = setValue(facade, prsRes.first, prsRes.second);

		PlusApplyFunctor plusApply;
		node = facade.Apply(oldNode, tmpNode, &plusApply);
		facade.RecursiveDeref(oldNode);
		facade.RecursiveDeref(tmpNode);
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
		ParserResultType prsRes = parseExpression(*itTests);
		BOOST_CHECK_MESSAGE(getValue(facade, node, prsRes.second)
			== prsRes.first, *itTests + " != "
			+ Convert::ToString(getValue(facade, node, prsRes.second)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
		itFailed != failedCases.end(); ++itFailed)
	{	// for every test case that should fail
		ParserResultType prsFailedRes = parseExpression(*itFailed);
		BOOST_CHECK_MESSAGE(getValue(facade, node, prsFailedRes.second)
			== BDD_BACKGROUND_VALUE, *itFailed + " == "
			+ Convert::ToString(getValue(facade, node, prsFailedRes.second)));
	}

	facade.RecursiveDeref(node);
}


BOOST_AUTO_TEST_CASE(large_diagram_test)
{
	CUDDFacade facade;
	boost::mt19937 prnGen(PRNG_SEED);

	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate test cases
		std::string formula = Convert::ToString(static_cast<ValueType>(prnGen()));

		for (unsigned j = 0; j < LARGE_TEST_FORMULA_LENGTH; ++j)
		{
			if (prnGen() % 4 != 0)
			{
				formula += " * " + Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		testCases.push_back(formula);
	}

	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;

	for (unsigned i = 0; i < LARGE_TEST_FORMULA_CASES; ++i)
	{	// generate failed test cases
		std::string formula = Convert::ToString(static_cast<ValueType>(1));

		for (unsigned j = 0; j < LARGE_TEST_FORMULA_LENGTH; ++j)
		{
			if (prnGen() % 31 != 0)
			{
				formula += " * " + Convert::ToString((prnGen() % 2 == 0)? " " : "~")
					+ "x" + Convert::ToString(j);
			}
		}

		failedCases.push_back(formula);
	}

	CUDDFacade::Node* node = facade.ReadBackground();
	facade.Ref(node);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		CUDDFacade::Node* oldNode = node;

		ParserResultType prsRes = parseExpression(*itTests);
		CUDDFacade::Node* tmpNode = setValue(facade, prsRes.first, prsRes.second);

#if DEBUG
		BOOST_TEST_MESSAGE("Stored " + parserResultToString(prsRes));
		BOOST_TEST_MESSAGE("Size of BDD: "
			+ Convert::ToString(facade.GetDagSize(node)));
#endif

		PlusApplyFunctor plusApply;
		node = facade.Apply(oldNode, tmpNode, &plusApply);
		facade.RecursiveDeref(oldNode);
		facade.RecursiveDeref(tmpNode);
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		ParserResultType prsRes = parseExpression(*itTests);
		BOOST_CHECK_MESSAGE(getValue(facade, node, prsRes.second)
			== prsRes.first, *itTests + " != "
			+ Convert::ToString(getValue(facade, node, prsRes.second)));

	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
		itFailed != failedCases.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		ParserResultType prsFailedRes = parseExpression(*itFailed);
		BOOST_CHECK_MESSAGE(getValue(facade, node, prsFailedRes.second)
			== BDD_BACKGROUND_VALUE, *itFailed + " == "
			+ Convert::ToString(getValue(facade, node, prsFailedRes.second)));
	}

	facade.RecursiveDeref(node);
}


BOOST_AUTO_TEST_CASE(no_variables_formula)
{
	const char* const TEST_VALUE = "1337";

	CUDDFacade facade;

	ParserResultType prsRes = parseExpression(TEST_VALUE);
	CUDDFacade::Node* node = setValue(facade, prsRes.first, prsRes.second);

	BOOST_CHECK_MESSAGE(getValue(facade, node, prsRes.second)
		== prsRes.first, Convert::ToString(TEST_VALUE) + " != "
		+ Convert::ToString(getValue(facade, node, prsRes.second)));

	facade.RecursiveDeref(node);
}

BOOST_AUTO_TEST_CASE(multiple_independent_bdds)
{
	CUDDFacade facade1;
	CUDDFacade facade2;

	// load test cases for the first BDD
	ListOfTestCasesType testCases1;
	ListOfTestCasesType failedCases1;
	loadStandardTests(testCases1, failedCases1);

	// load test cases for the second BDD
	ListOfTestCasesType testCases2;
	ListOfTestCasesType failedCases2;
	loadStandardTests(failedCases2, testCases2);

	CUDDFacade::Node* node1 = facade1.ReadBackground();
	facade1.Ref(node1);

	CUDDFacade::Node* node2 = facade2.ReadBackground();
	facade2.Ref(node2);

	for (ListOfTestCasesType::const_iterator itTests = testCases1.begin();
		itTests != testCases1.end(); ++itTests)
	{	// store each test case
		CUDDFacade::Node* oldNode = node1;

		ParserResultType prsRes = parseExpression(*itTests);
		CUDDFacade::Node* tmpNode = setValue(facade1, prsRes.first, prsRes.second);

#if DEBUG
		BOOST_TEST_MESSAGE("Stored " + parserResultToString(prsRes));
		BOOST_TEST_MESSAGE("Size of BDD: "
			+ Convert::ToString(facade1.GetDagSize(node1)));
#endif

		PlusApplyFunctor plusApply;
		node1 = facade1.Apply(oldNode, tmpNode, &plusApply);
		facade1.RecursiveDeref(oldNode);
		facade1.RecursiveDeref(tmpNode);
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases2.begin();
		itTests != testCases2.end(); ++itTests)
	{	// store each test case
		CUDDFacade::Node* oldNode = node2;

		ParserResultType prsRes = parseExpression(*itTests);
		CUDDFacade::Node* tmpNode = setValue(facade2, prsRes.first, prsRes.second);

#if DEBUG
		BOOST_TEST_MESSAGE("Stored " + parserResultToString(prsRes));
		BOOST_TEST_MESSAGE("Size of BDD: "
			+ Convert::ToString(facade2.GetDagSize(node2)));
#endif

		PlusApplyFunctor plusApply;
		node2 = facade2.Apply(oldNode, tmpNode, &plusApply);
		facade2.RecursiveDeref(oldNode);
		facade2.RecursiveDeref(tmpNode);
	}

	for (ListOfTestCasesType::const_iterator itTests = testCases1.begin();
		itTests != testCases1.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		ParserResultType prsRes = parseExpression(*itTests);
		BOOST_CHECK_MESSAGE(getValue(facade1, node1, prsRes.second)
			== prsRes.first, *itTests + " != "
			+ Convert::ToString(getValue(facade1, node1, prsRes.second)));

	}

	for (ListOfTestCasesType::const_iterator itTests = testCases2.begin();
		itTests != testCases2.end(); ++itTests)
	{	// test that the test cases have been stored properly
#if DEBUG
		BOOST_TEST_MESSAGE("Finding stored " + *itTests);
#endif
		ParserResultType prsRes = parseExpression(*itTests);
		BOOST_CHECK_MESSAGE(getValue(facade2, node2, prsRes.second)
			== prsRes.first, *itTests + " != "
			+ Convert::ToString(getValue(facade2, node2, prsRes.second)));

	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases1.begin();
		itFailed != failedCases1.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		ParserResultType prsFailedRes = parseExpression(*itFailed);
		BOOST_CHECK_MESSAGE(getValue(facade1, node1, prsFailedRes.second)
			== BDD_BACKGROUND_VALUE, *itFailed + " == "
			+ Convert::ToString(getValue(facade1, node1, prsFailedRes.second)));
	}

	for (ListOfTestCasesType::const_iterator itFailed = failedCases2.begin();
		itFailed != failedCases2.end(); ++itFailed)
	{	// for every test case that should fail
#if DEBUG
		BOOST_TEST_MESSAGE("Finding failed " + *itFailed);
#endif
		ParserResultType prsFailedRes = parseExpression(*itFailed);
		BOOST_CHECK_MESSAGE(getValue(facade2, node2, prsFailedRes.second)
			== BDD_BACKGROUND_VALUE, *itFailed + " == "
			+ Convert::ToString(getValue(facade2, node2, prsFailedRes.second)));
	}

	facade1.RecursiveDeref(node1);
	facade2.RecursiveDeref(node2);
}

BOOST_AUTO_TEST_SUITE_END()
