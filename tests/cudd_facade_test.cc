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
 * Formulae for standard test cases represented by a table
 */
const char* const STANDARD_TEST_CASES_TABLE =
	"|0|0|0|3|4|0|0|0|0|9|0|0|0|0|14|15|";


/**
 * Formulae for reindexed standard test case represented by a table. Boolean
 * varibles at index 1 are moved to index 4
 */
const char* const REINDEXED_STANDARD_TEST_CASES_TABLE =
	"|0|4|0|0|0|0|3|0|0|4|0|0|0|0|3|0|0|0|9|0|0|14|0|15|0|0|9|0|0|14|0|15|";


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
	typedef std::vector<ValueType> ValueTableType;
	typedef std::vector<signed char> AssignmentType;

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
	static const signed char ASGN_TRUE = 1;
	static const signed char ASGN_FALSE = 0;
	static const signed char ASGN_UNKNOWN = -1;

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
		facade.Ref(tmpNode);
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


	static void setValueTableValue(const AssignmentType& asgn, ValueType value,
		ValueTableType& table, unsigned index, unsigned pos)
	{
		// Assertions
		assert(index <= asgn.size());
	
		if (index == asgn.size())
		{	// in case we are at the end of the assignment array
			table[pos] = value;
		}
		else
		{	// in case we are insed the assignment array
			if (asgn[index] == ASGN_TRUE)
			{
				setValueTableValue(asgn, value, table, index+1,
					pos + (1 << (asgn.size() - index - 1)));
			}
			else if (asgn[index] == ASGN_FALSE)
			{
				setValueTableValue(asgn, value, table, index+1, pos);
			}
			else
			{	// in case the variable is not bound
				setValueTableValue(asgn, value, table, index+1,
					pos + (1 << (asgn.size() - index - 1)));
				setValueTableValue(asgn, value, table, index+1, pos);
			}
		}
	}
	
	
	static void fillValueTableBlockForNode(const CUDDFacade& facade,
		CUDDFacade::Node* node, AssignmentType& asgn, ValueTableType& table)
	{
		// Assertions
		assert(node != static_cast<CUDDFacade::Node*>(0));
	
		if (facade.IsNodeConstant(node))
		{	// in case we hit the bottom
			setValueTableValue(asgn, facade.GetNodeValue(node), table, 0, 0);
		}
		else
		{	// in case we are in the middle of the BDD
			asgn[facade.GetNodeIndex(node)] = ASGN_TRUE;
			fillValueTableBlockForNode(facade, facade.GetThenChild(node), asgn, table);
			asgn[facade.GetNodeIndex(node)] = ASGN_FALSE;
			fillValueTableBlockForNode(facade, facade.GetElseChild(node), asgn, table);
			asgn[facade.GetNodeIndex(node)] = ASGN_UNKNOWN;
		}
	}
	
	
	static ValueTableType GetValueTable(const CUDDFacade& facade,
		CUDDFacade::Node* node)
	{
		// Assertions
		assert(node != static_cast<CUDDFacade::Node*>(0));
	
		ValueTableType result(1 << facade.GetVarCount());
		AssignmentType asgn(facade.GetVarCount(), ASGN_UNKNOWN);
	
		fillValueTableBlockForNode(facade, node, asgn, result);
	
		return result;
	}


	static std::string ValueTableToString(const ValueTableType& table)
	{
		std::string result = "|";

		for (ValueTableType::const_iterator itTable = table.begin();
				itTable != table.end(); ++itTable)
		{
			result += Convert::ToString(*itTable) + "|";
		}

		return result;
	}

	CUDDFacade::Node* CreateMTBDDForTestCases(CUDDFacade& facade,
		const ListOfTestCasesType& testCases)
	{
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
				+ Convert::ToString(facade1.GetDagSize(node1)));
#endif

			PlusApplyFunctor plusApply;
			node = facade.Apply(oldNode, tmpNode, &plusApply);
			facade.Ref(node);
			facade.RecursiveDeref(oldNode);
			facade.RecursiveDeref(tmpNode);
		}

		return node;
	}


public:

	CUDDFacadeFixture()
		: varToNumDict_(), varCounter_(0)
	{
		boost::unit_test::unit_test_log.set_threshold_level(
			boost::unit_test::log_messages);
	}

};


const signed char CUDDFacadeFixture::ASGN_TRUE;
const signed char CUDDFacadeFixture::ASGN_FALSE;
const signed char CUDDFacadeFixture::ASGN_UNKNOWN;


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

	CUDDFacade::Node* node = CreateMTBDDForTestCases(facade, testCases);

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

	CUDDFacade::Node* node = CreateMTBDDForTestCases(facade, testCases);

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

	CUDDFacade::Node* node1 = CreateMTBDDForTestCases(facade1, testCases1);
	CUDDFacade::Node* node2 = CreateMTBDDForTestCases(facade2, testCases2);

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


BOOST_AUTO_TEST_CASE(then_else_test)
{
	CUDDFacade facade;

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	CUDDFacade::Node* node = CreateMTBDDForTestCases(facade, testCases);

	ValueTableType table = GetValueTable(facade, node);

	BOOST_CHECK_MESSAGE(ValueTableToString(table) == STANDARD_TEST_CASES_TABLE,
		"Stored table " + ValueTableToString(table)
		+ " is not equal to expected table "
		+ Convert::ToString(STANDARD_TEST_CASES_TABLE));


	facade.RecursiveDeref(node);
}


BOOST_AUTO_TEST_CASE(variable_index_change)
{
	CUDDFacade facade;

	// load test cases
	ListOfTestCasesType testCases;
	ListOfTestCasesType failedCases;
	loadStandardTests(testCases, failedCases);

	CUDDFacade::Node* node = CreateMTBDDForTestCases(facade, testCases);

	// changes the table
	CUDDFacade::Node* tmpNode = facade.ChangeVariableIndex(node, 1, 4);
	facade.Ref(tmpNode);
	facade.RecursiveDeref(node);
	node = tmpNode;

	BOOST_CHECK_MESSAGE(ValueTableToString(GetValueTable(facade, node))
		== REINDEXED_STANDARD_TEST_CASES_TABLE,
		"Stored table " + ValueTableToString(GetValueTable(facade, node))
		+ " is not equal to expected table "
		+ Convert::ToString(REINDEXED_STANDARD_TEST_CASES_TABLE));


	// should not change the table
	tmpNode = facade.ChangeVariableIndex(node, 3, 3);
	facade.Ref(tmpNode);
	facade.RecursiveDeref(node);
	node = tmpNode;

	BOOST_CHECK_MESSAGE(ValueTableToString(GetValueTable(facade, node))
		== REINDEXED_STANDARD_TEST_CASES_TABLE,
		"Stored table " + ValueTableToString(GetValueTable(facade, node))
		+ " is not equal to expected table "
		+ Convert::ToString(REINDEXED_STANDARD_TEST_CASES_TABLE));


	// should also not change the table
	tmpNode = facade.ChangeVariableIndex(node, 7, 9);
	facade.RecursiveDeref(node);
	node = tmpNode;

	BOOST_CHECK_MESSAGE(ValueTableToString(GetValueTable(facade, node))
		== REINDEXED_STANDARD_TEST_CASES_TABLE,
		"Stored table " + ValueTableToString(GetValueTable(facade, node))
		+ " is not equal to expected table "
		+ Convert::ToString(REINDEXED_STANDARD_TEST_CASES_TABLE));

	facade.RecursiveDeref(node);
}


BOOST_AUTO_TEST_SUITE_END()
