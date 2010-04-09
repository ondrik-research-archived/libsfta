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
#define SFTA_LOGGER_INFO(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).info((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_NOTICE(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).notice((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_WARN(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).warn((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_ERROR(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).error((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_CRIT(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).crit((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_ALERT(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).alert((SFTA_LOGGER_PREFIX) + (msg)))
#define SFTA_LOGGER_FATAL(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).fatal((SFTA_LOGGER_PREFIX) + (msg)))

