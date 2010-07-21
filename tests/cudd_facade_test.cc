/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Test suite for CUDDFacade class.
 *
 *****************************************************************************/

// SFTA headers
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


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

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

	CUDDFacade facade_;

	StringToUnsignedDictType varToNumDict_;

	unsigned varCounter_;

	static const ValueType BDD_BACKGROUND_VALUE = 0;
	static const ValueType BDD_TRUE_VALUE = 1;

protected:

	CUDDFacade::Node* extendBddByVariable(CUDDFacade::Node* bdd,
		const VariableOccurenceType& var)
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
		CUDDFacade::Node* varNode = facade_.AddIthVar(varNum);
		facade_.Ref(varNode);

		if (!isPositive)
		{	// when the variable is complemented
			CUDDFacade::Node* oldVar = varNode;
			varNode = facade_.AddCmpl(varNode);
			facade_.Ref(varNode);
			facade_.RecursiveDeref(oldVar);
		}

		// carry out the conjunction operation
		CUDDFacade::Node* newBdd = facade_.Times(bdd, varNode);
		facade_.Ref(newBdd);
		facade_.RecursiveDeref(bdd);
		facade_.RecursiveDeref(varNode);

		return newBdd;
	}


	CUDDFacade::Node* extendBddByVariableList(CUDDFacade::Node* bdd,
		const VariableListType& asgn)
	{
		// Assertions
		assert(bdd != static_cast<CUDDFacade::Node*>(0));

		for (VariableListType::const_iterator it = asgn.begin();
			it != asgn.end(); ++it)
		{	// for each variable in the assignment
			bdd = extendBddByVariable(bdd, *it);
		}

		return bdd;
	}


	CUDDFacade::Node* setValue(ValueType value, const VariableListType& asgn)
	{
		// create a node with the value
		CUDDFacade::Node* node = facade_.AddConst(value);
		facade_.Ref(node);

		// create the BDD above the value
		return extendBddByVariableList(node, asgn);
	}


	ValueType getValue(CUDDFacade::Node* rootNode,
		const VariableListType& asgn)
	{
		// create a node with the TRUE value
		CUDDFacade::Node* node = facade_.AddConst(BDD_TRUE_VALUE);
		facade_.Ref(node);

		node = extendBddByVariableList(node, asgn);

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
						if (val_ == BDD_BACKGROUND_VALUE)
						{	// in case we haven't collected any value yet
							val_ = lhs;
						}
						else
						{	// in case we are overwriting an existing value
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
		CUDDFacade::Node* tmpNode = facade_.Apply(rootNode, node, &func);
		facade_.RecursiveDeref(tmpNode);
		facade_.RecursiveDeref(node);

		return func.GetValue();
	}


	/*
	 * format:   5*x1*x2*~x3*x4
	 */
	ParserResultType parseExpression(std::string input)
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


public:

	CUDDFacadeFixture()
		: facade_(), varToNumDict_(), varCounter_(0)
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
	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;
	testCases.push_back(" 3 * ~x1 * ~x2 *  x3 *  x4");
	testCases.push_back(" 4 * ~x1 *  x2 * ~x3 * ~x4");
	testCases.push_back(" 9 *  x1 * ~x2 * ~x3 *  x4");
	testCases.push_back("14 *  x1 *  x2 *  x3 * ~x4");
	testCases.push_back("14 *  x1 *  x2 *  x3 * ~x4");
	testCases.push_back("15 *  x1 *  x2 *  x3 *  x4");

	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;
	failedCases.push_back(" 1 * ~x1 * ~x2 * ~x3 *  x4");
	failedCases.push_back(" 2 * ~x1 * ~x2 *  x3 * ~x4");
	failedCases.push_back(" 5 * ~x1 *  x2 * ~x3 *  x4");
	failedCases.push_back(" 6 * ~x1 *  x2 *  x3 * ~x4");
	failedCases.push_back(" 7 * ~x1 *  x2 *  x3 *  x4");
	failedCases.push_back(" 8 *  x1 * ~x2 * ~x3 * ~x4");
	failedCases.push_back("10 *  x1 * ~x2 *  x3 * ~x4");
	failedCases.push_back("11 *  x1 * ~x2 *  x3 *  x4");
	failedCases.push_back("12 *  x1 *  x2 * ~x3 * ~x4");
	failedCases.push_back("13 *  x1 *  x2 * ~x3 *  x4");
	failedCases.push_back(" 5 *       ~x2 *       ~x4");

	NodeArrayType testRootNodes;

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		ParserResultType prsRes = parseExpression(*itTests);
		testRootNodes.push_back(setValue(prsRes.first, prsRes.second));
	}

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		ParserResultType prsRes = parseExpression(testCases[i]);
		BOOST_CHECK_MESSAGE(getValue(testRootNodes[i], prsRes.second)
			== prsRes.first, testCases[i] + " != "
			+ Convert::ToString(getValue(testRootNodes[i], prsRes.second)));

		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
			itFailed != failedCases.end(); ++itFailed)
		{	// for every test case that should fail
			ParserResultType prsFailedRes = parseExpression(*itFailed);
			BOOST_CHECK_MESSAGE(getValue(testRootNodes[i], prsFailedRes.second)
				== BDD_BACKGROUND_VALUE, testCases[i] + " == "
				+ Convert::ToString(getValue(testRootNodes[i], prsFailedRes.second)));
		}
	}

	for (NodeArrayType::iterator itNodes = testRootNodes.begin();
		itNodes != testRootNodes.end(); ++itNodes)
	{	// dereference the nodes
		facade_.RecursiveDeref(*itNodes);
	}
}


BOOST_AUTO_TEST_CASE(composed_values_storage_test)
{
	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;
	testCases.push_back(" 3 * ~x1 * ~x2 *  x3 *  x4");
	testCases.push_back(" 4 * ~x1 *  x2 * ~x3 * ~x4");
	testCases.push_back(" 9 *  x1 * ~x2 * ~x3 *  x4");
	testCases.push_back("14 *  x1 *  x2 *  x3 * ~x4");
	testCases.push_back("14 *  x1 *  x2 *  x3 * ~x4");
	testCases.push_back("15 *  x1 *  x2 *  x3 *  x4");

	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;
	failedCases.push_back(" 1 * ~x1 * ~x2 * ~x3 *  x4");
	failedCases.push_back(" 2 * ~x1 * ~x2 *  x3 * ~x4");
	failedCases.push_back(" 5 * ~x1 *  x2 * ~x3 *  x4");
	failedCases.push_back(" 6 * ~x1 *  x2 *  x3 * ~x4");
	failedCases.push_back(" 7 * ~x1 *  x2 *  x3 *  x4");
	failedCases.push_back(" 8 *  x1 * ~x2 * ~x3 * ~x4");
	failedCases.push_back("10 *  x1 * ~x2 *  x3 * ~x4");
	failedCases.push_back("11 *  x1 * ~x2 *  x3 *  x4");
	failedCases.push_back("12 *  x1 *  x2 * ~x3 * ~x4");
	failedCases.push_back("13 *  x1 *  x2 * ~x3 *  x4");
	failedCases.push_back(" 5 *       ~x2 *       ~x4");

	CUDDFacade::Node* node = facade_.ReadBackground();
	facade_.Ref(node);

	for (ListOfTestCasesType::const_iterator itTests = testCases.begin();
		itTests != testCases.end(); ++itTests)
	{	// store each test case
		CUDDFacade::Node* oldNode = node;

		ParserResultType prsRes = parseExpression(*itTests);
		CUDDFacade::Node* tmpNode = setValue(prsRes.first, prsRes.second);

		node = facade_.Apply(oldNode, tmpNode, new PlusApplyFunctor());
		facade_.RecursiveDeref(oldNode);
		facade_.RecursiveDeref(tmpNode);
	}

	for (unsigned i = 0; i < testCases.size(); ++i)
	{	// test that the test cases have been stored properly
		ParserResultType prsRes = parseExpression(testCases[i]);
		BOOST_CHECK_MESSAGE(getValue(node, prsRes.second)
			== prsRes.first, testCases[i] + " != "
			+ Convert::ToString(getValue(node, prsRes.second)));

		for (ListOfTestCasesType::const_iterator itFailed = failedCases.begin();
			itFailed != failedCases.end(); ++itFailed)
		{	// for every test case that should fail
			ParserResultType prsFailedRes = parseExpression(*itFailed);
			BOOST_CHECK_MESSAGE(getValue(node, prsFailedRes.second)
				== BDD_BACKGROUND_VALUE, testCases[i] + " == "
				+ Convert::ToString(getValue(node, prsFailedRes.second)));
		}
	}

	facade_.RecursiveDeref(node);
}


BOOST_AUTO_TEST_CASE(large_diagram_test)
{
	boost::mt19937 prnGen(781436);

	std::string formula = Convert::ToString(static_cast<ValueType>(prnGen()));

	for (unsigned i = 0; i < 64; ++i)
	{
		formula += " * " + Convert::ToString((prnGen() % 2 == 0)? "" : "~")  + "x" + Convert::ToString(i);
	}

	BOOST_TEST_MESSAGE("Generated formula: " + formula);

	// TODO
	// formulae that we wish to store in the BDD
	ListOfTestCasesType testCases;


	// formulae that we want to check that are not in the BDD
	ListOfTestCasesType failedCases;

	NodeArrayType testRootNodes;
}


BOOST_AUTO_TEST_CASE(boundary_cases)
{
	// TODO
}


BOOST_AUTO_TEST_SUITE_END()
