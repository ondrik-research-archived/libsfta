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
		log4cpp::Appender* app1  = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

		std::string cat_name = "SFTA";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app1);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::INFO);
	}
}
