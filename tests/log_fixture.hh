/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file of a fixture class that enables logging shared among test
 *    suites.
 *
 *****************************************************************************/

#ifndef _LOG_FIXTURE_HH_
#define _LOG_FIXTURE_HH_


// Log4cpp headers
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>

// SFTA headers
#include <sfta/cudd_facade.hh>


/**
 * @brief   Test fixture that enables logging
 * @author  Ondra Lengal <ondra@lengal.net>
 * @date    2010
 *
 * This test fixture enables Log4cpp logging for proper categories.
 */
class LogFixture
{
private:  // Private data members

	/**
	 * @brief  Static initialization flag
	 *
	 * Static flag that is initialized when logging is enabled (only once for
	 * the whole program.
	 */
	static bool logInitialized_;


public:   // Public methods

	/**
	 * @brief  Constructor
	 *
	 * Fixture constructor that initializes the logging feature (only once for
	 * all fixtures).
	 */
	LogFixture()
	{
		if (!logInitialized_)
		{	// if the logging has not been initialized yet
			logInitialized_ = true;

			// Create the appender
			log4cpp::Appender* app = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

			// Set the data layout of the appender
			log4cpp::Layout* layout = new log4cpp::BasicLayout();
			app->setLayout(layout);

			std::string cat_name = SFTA::Private::CUDDFacade::LOG_CATEGORY_NAME;

			log4cpp::Category::getInstance(cat_name).setAdditivity(false);
			log4cpp::Category::getInstance(cat_name).addAppender(app);
			log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::NOTSET);
		}
	}

};

#endif
