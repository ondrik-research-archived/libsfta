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

		class CollectorFunctor : public CUDDFacade::AbstractApplyFunctor
		{
		private:

			ValueType val_;

		public:

			CollectorFunctor()
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

		CollectorFunctor func;
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
				throw std::invalid_argument("ParseExpression: invalid argument");
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
	{ }

};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDFacadeFixture)

BOOST_AUTO_TEST_CASE(work_with_variables)
{
	boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

	ParserResultType prsRes = parseExpression("5 * x1 * x2 * x3 * ~x4");
	CUDDFacade::Node* root = setValue(prsRes.first, prsRes.second);

	BOOST_CHECK(getValue(root, prsRes.second) == prsRes.first);

	facade_.RecursiveDeref(root);

//	CUDDFacade::Node* x0 = facade.AddIthVar(0);
//	facade.Ref(x0);
//
//	CUDDFacade::Node* not_x0 = facade.AddCmpl(x0);
//	BOOST_CHECK(not_x0 != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(not_x0);
//
//	CUDDFacade::Node* x2 = facade.AddIthVar(2);
//	BOOST_CHECK(x2 != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(x2);
//
//	CUDDFacade::Node* not_x2 = facade.AddCmpl(x2);
//	BOOST_CHECK(not_x2 != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(not_x2);
//
//	CUDDFacade::Node* const_three = facade.AddConst(3);
//	BOOST_CHECK(const_three != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(const_three);
//
//	CUDDFacade::Node* const_seven = facade.AddConst(7);
//	BOOST_CHECK(const_seven != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(const_seven);
//
//	CUDDFacade::Node* const_hundred = facade.AddConst(100);
//	BOOST_CHECK(const_hundred != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(const_hundred);
//
//	CUDDFacade::Node* old_bck = facade.ReadBackground();
//	BOOST_CHECK(old_bck != static_cast<CUDDFacade::Node*>(0));
//	facade.SetBackground(const_hundred);
//	facade.RecursiveDeref(old_bck);
//
//	CUDDFacade::Node* three_x0 = facade.Times(const_three, x0);
//	BOOST_CHECK(three_x0 != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(three_x0);
//
//	CUDDFacade::Node* three_x0_not_x2 = facade.Times(three_x0, not_x2);
//	BOOST_CHECK(three_x0_not_x2 != static_cast<CUDDFacade::Node*>(0));
//	facade.Ref(three_x0_not_x2);
//	facade.RecursiveDeref(three_x0);
//
//	std::vector<CUDDFacade::Node*> arr;
//	arr.push_back(three_x0_not_x2);
//
//	std::vector<std::string> arrStr;
//	arrStr.push_back("new root");
//
//	std::string str = facade.StoreToString(arr, arrStr);
//	BOOST_TEST_MESSAGE("Output: " + str);
//
//	facade.RecursiveDeref(three_x0_not_x2);
//	facade.RecursiveDeref(x0);
//	facade.RecursiveDeref(not_x0);
//	facade.RecursiveDeref(x2);
//	facade.RecursiveDeref(not_x2);
//	facade.RecursiveDeref(const_three);
//	facade.RecursiveDeref(const_seven);
//	facade.RecursiveDeref(const_hundred);
}

BOOST_AUTO_TEST_SUITE_END()
