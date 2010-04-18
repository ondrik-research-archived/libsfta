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
#include <sfta/cudd_facade.hh>
using SFTA::Private::CUDDFacade;

// Boost headers
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDFacade
#include <boost/test/unit_test.hpp>

// testing headers
#include "log_fixture.hh"


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class CUDDFacadeFixture : public LogFixture
{
protected:

	CUDDFacade facade;

public:

	CUDDFacadeFixture()
		: facade()
	{ }
};


/******************************************************************************
 *                              Start of testing                              *
 ******************************************************************************/

BOOST_FIXTURE_TEST_SUITE(suite, CUDDFacadeFixture)

BOOST_AUTO_TEST_CASE(work_with_variables)
{
	boost::unit_test::unit_test_log.set_threshold_level(boost::unit_test::log_messages);

	CUDDFacade::Node* x0 = facade.AddIthVar(0);
	BOOST_CHECK(x0 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(x0);

	CUDDFacade::Node* not_x0 = facade.AddCmpl(x0);
	BOOST_CHECK(not_x0 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(not_x0);

	CUDDFacade::Node* x2 = facade.AddIthVar(2);
	BOOST_CHECK(x2 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(x2);

	CUDDFacade::Node* not_x2 = facade.AddCmpl(x2);
	BOOST_CHECK(not_x2 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(not_x2);

	CUDDFacade::Node* const_three = facade.AddConst(3);
	BOOST_CHECK(const_three != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(const_three);

	CUDDFacade::Node* const_seven = facade.AddConst(7);
	BOOST_CHECK(const_seven != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(const_seven);

	CUDDFacade::Node* const_hundred = facade.AddConst(100);
	BOOST_CHECK(const_hundred != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(const_hundred);

	CUDDFacade::Node* old_bck = facade.ReadBackground();
	BOOST_CHECK(old_bck != static_cast<CUDDFacade::Node*>(0));
	facade.SetBackground(const_hundred);
	facade.RecursiveDeref(old_bck);

	CUDDFacade::Node* three_x0 = facade.Times(const_three, x0);
	BOOST_CHECK(three_x0 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(three_x0);

	CUDDFacade::Node* three_x0_not_x2 = facade.Times(three_x0, not_x2);
	BOOST_CHECK(three_x0_not_x2 != static_cast<CUDDFacade::Node*>(0));
	facade.Ref(three_x0_not_x2);
	facade.RecursiveDeref(three_x0);

	std::vector<CUDDFacade::Node*> arr;
	arr.push_back(three_x0_not_x2);

	std::vector<std::string> arrStr;
	arrStr.push_back("new root");

	std::string str = facade.StoreToString(arr, arrStr);
	BOOST_TEST_MESSAGE("Output: " + str);

	facade.RecursiveDeref(three_x0_not_x2);
	facade.RecursiveDeref(x0);
	facade.RecursiveDeref(not_x0);
	facade.RecursiveDeref(x2);
	facade.RecursiveDeref(not_x2);
	facade.RecursiveDeref(const_three);
	facade.RecursiveDeref(const_seven);
	facade.RecursiveDeref(const_hundred);
}

BOOST_AUTO_TEST_SUITE_END()
