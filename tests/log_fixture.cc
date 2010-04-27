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
		log4cpp::Appender* app1 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app2 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app3 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app4 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app5 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app6 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app7 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app8 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);
		log4cpp::Appender* app9 = new log4cpp::OstreamAppender("ClogAppender", &std::clog);

		// Set the data layout of the appender
		log4cpp::Layout* layout1 = new log4cpp::BasicLayout();
		app1->setLayout(layout1);

		log4cpp::Layout* layout2 = new log4cpp::BasicLayout();
		app2->setLayout(layout2);

		log4cpp::Layout* layout3 = new log4cpp::BasicLayout();
		app3->setLayout(layout3);

		log4cpp::Layout* layout4 = new log4cpp::BasicLayout();
		app4->setLayout(layout4);

		log4cpp::Layout* layout5 = new log4cpp::BasicLayout();
		app5->setLayout(layout5);

		log4cpp::Layout* layout6 = new log4cpp::BasicLayout();
		app6->setLayout(layout6);

		log4cpp::Layout* layout7 = new log4cpp::BasicLayout();
		app7->setLayout(layout7);

		log4cpp::Layout* layout8 = new log4cpp::BasicLayout();
		app8->setLayout(layout8);

		log4cpp::Layout* layout9 = new log4cpp::BasicLayout();
		app9->setLayout(layout9);

		std::string cat_name = "cudd_facade";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app1);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "cudd_shared_mtbdd";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app2);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "map_leaf_allocator";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app3);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "compact_variable_assignment";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app4);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "mtbdd_transition_function";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app5);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "map_state_translator";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app6);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "timbuk_ta_builder";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app7);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "symbol_dictionary";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app8);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);

		cat_name = "tree_automaton";

		log4cpp::Category::getInstance(cat_name).setAdditivity(false);
		log4cpp::Category::getInstance(cat_name).addAppender(app9);
		log4cpp::Category::getInstance(cat_name).setPriority(log4cpp::Priority::DEBUG);
	}
}
