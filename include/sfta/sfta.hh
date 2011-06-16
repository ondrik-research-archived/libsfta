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

//#define DEBUG 0
//#define NDEBUG

#define SFTA_LOG_CATEGORY_NAME "SFTA"

#define SFTA_LOGGER_PREFIX (std::string(__FILE__ ":" + SFTA::Private::Convert::ToString(__LINE__) + ": "))

#define SFTA_LOGGER_LOG_MESSAGE(severity, msg) (log4cpp::Category::getInstance(SFTA_LOG_CATEGORY_NAME).severity((SFTA_LOGGER_PREFIX) + (msg)))

#define SFTA_LOGGER_DEBUG(msg)  (SFTA_LOGGER_LOG_MESSAGE(debug, msg))
#define SFTA_LOGGER_INFO(msg)   (SFTA_LOGGER_LOG_MESSAGE(info, msg))
#define SFTA_LOGGER_NOTICE(msg) (SFTA_LOGGER_LOG_MESSAGE(notice, msg))
#define SFTA_LOGGER_WARN(msg)   (SFTA_LOGGER_LOG_MESSAGE(warn, msg))
#define SFTA_LOGGER_ERROR(msg)  (SFTA_LOGGER_LOG_MESSAGE(error, msg))
#define SFTA_LOGGER_CRIT(msg)   (SFTA_LOGGER_LOG_MESSAGE(crit, msg))
#define SFTA_LOGGER_ALERT(msg)  (SFTA_LOGGER_LOG_MESSAGE(alert, msg))
#define SFTA_LOGGER_FATAL(msg)  (SFTA_LOGGER_LOG_MESSAGE(fatal, msg))

#if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 402
#define GCC_DIAG_STR(s) #s
#define GCC_DIAG_JOINSTR(x,y) GCC_DIAG_STR(x ## y)
# define GCC_DIAG_DO_PRAGMA(x) _Pragma (#x)
# define GCC_DIAG_PRAGMA(x) GCC_DIAG_DO_PRAGMA(GCC diagnostic x)
# if ((__GNUC__ * 100) + __GNUC_MINOR__) >= 406
#  define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(push) \
	         GCC_DIAG_PRAGMA(ignored GCC_DIAG_JOINSTR(-W,x))
#  define GCC_DIAG_ON(x) GCC_DIAG_PRAGMA(pop)
# else
#  define GCC_DIAG_OFF(x) GCC_DIAG_PRAGMA(ignored GCC_DIAG_JOINSTR(-W,x))
#  define GCC_DIAG_ON(x)  GCC_DIAG_PRAGMA(warning GCC_DIAG_JOINSTR(-W,x))
# endif
#else
# define GCC_DIAG_OFF(x)
# define GCC_DIAG_ON(x)
#endif
