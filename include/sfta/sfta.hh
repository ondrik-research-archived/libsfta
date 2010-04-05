/*****************************************************************************
 *  Symbolic Finite Tree Automata Library
 *
 *  Copyright (c) 2010  Ondra Lengal <ondra@lengal.net>
 *
 *  Description:
 *    Header file with global declarations. It contains:
 *      * macros for easy logging
 *
 *****************************************************************************/

// Log4cpp headers
#include <log4cpp/Category.hh>

#define SFTA_LOGGER_PREFIX (std::string(__FILE__ ":" + SFTA::Private::Convert::ToString(__LINE__) + ": "))

#define SFTA_LOGGER_DEBUG(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).debug((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_WARN(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).warn((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_FATAL(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).fatal((SFTA_LOGGER_PREFIX) + (msg)))

