// Log4cpp headers
#include <log4cpp/Category.hh>

#define KNIHOVNA_LOGGER_PREFIX (std::string(__FILE__ ":" + Knihovna::Private::Convert::ToString(__LINE__) + ": "))

#define KNIHOVNA_LOGGER_WARN(msg) (log4cpp::Category::getInstance(LOG_CATEGORY_NAME).warn((KNIHOVNA_LOGGER_PREFIX) + (msg)))

