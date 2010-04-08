/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Implementation of a fixture class that enables logging shared among test
 *    suites.
 *
 *****************************************************************************/

// SFTA headers
#include <sfta/cudd_facade.hh>
#include <sfta/cudd_shared_mtbdd.hh>

// testing header files
#include "log_fixture.hh"

// initialization of the static variable
bool LogFixture::logInitialized_ = false;


LogFixture::LogFixture()
{
	if (!logInitialized_)
	{	// if the logging has not been initialized yet
		logInitialized_ = true;

		// Create the appender
		log4cpp::Appender* app = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app2 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

		// Set the data layout of the appender
		log4cpp::Layout* layout = new log4cpp::BasicLayout();
		app->setLayout(layout);

		std::string cat_name = SFTA::Private::CUDDFacade::LOG_CATEGORY_NAME;

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "cudd_shared_mtbdd";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app2);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);
	}
}
