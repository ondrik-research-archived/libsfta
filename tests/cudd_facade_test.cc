#include <sfta/cudd_facade.hh>
using SFTA::Private::CUDDFacade;
using SFTA::Private::CUDDFacade;

#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE CUDDFacade
#include <boost/test/unit_test.hpp>


/******************************************************************************
 *                                  Fixtures                                  *
 ******************************************************************************/

class LogFixture
{

private:

	static bool logInitialized_;

public:

	LogFixture()
	{
		if (!logInitialized_)
		{
			logInitialized_ = true;

			// Create the appender
			log4cpp::Appender* app = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

			// Set the data layout of the appender
			log4cpp::Layout* layout = new log4cpp::BasicLayout();
			app->setLayout(layout);

			std::string cat_name = SFTA::Private::CUDDFacade::LOG_CATEGORY_NAME;

			log4cpp::Category::getInstance(cat_name).setAdditivity(false);
			log4cpp::Category::getInstance(cat_name).setAppender(app);
			log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::NOTSET);
		}
	}

	virtual ~LogFixture() { }
};

bool LogFixture::logInitialized_ = false;


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
