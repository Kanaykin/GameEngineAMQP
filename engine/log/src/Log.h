#pragma once
#include <format>
#include <iostream>
#include <string>
#include <string_view>
#include <format>
#include "boost/format.hpp"


#include "LogSingleton.h"

namespace log4cpp
{
    class Category;
}

namespace logger
{

enum class LogVerbosity
{
    NoLogging        = 0,
    Fatal,
    Error,
    Warning,
    Info,
    Debug,
    All
};

class Logger
{
public:

    Logger(const std::string& category, LogVerbosity verbosity);
    
    void debug(const std::string& message);
    void info(const std::string& message);
    void warn(const std::string& message);
    void error(const std::string& message);
    void fatal(const std::string& message);

protected:
    log4cpp::Category& _category;
};

#define DECLARE_LOG_CATEGORY(ClassName, Verbosity) \
class Class##ClassName : public logger::Logger {\
public: \
Class##ClassName():logger::Logger(#ClassName, Verbosity){} \
};\
typedef logger::LogSingleton<Class##ClassName> ClassName;


template< class... Args >
std::string format(std::string_view fmt, Args&&... args )
{
    boost::format f(std::string{fmt});
    int unroll[] {0, (f % std::forward<Args>(args), 0)...};
    static_cast<void>(unroll);
    return boost::str(f);
}

template< class LoggerCategory, class... Args >
void DEBUG_LOG(std::string_view fmt, Args&&... args )
{
    LoggerCategory::getInstance().debug(format(fmt, args...));
}

template< class LoggerCategory, class... Args >
void INFO_LOG( std::string_view fmt, Args&&... args )
{
    LoggerCategory::getInstance().info(format(fmt, args...));
    
}

template< class LoggerCategory, class... Args >
void WARNING_LOG( std::string_view fmt, Args&&... args )
{
    LoggerCategory::getInstance().warn(format(fmt, args...));
}

template< class LoggerCategory, class... Args >
void ERROR_LOG(std::string_view fmt, Args&&... args )
{
    LoggerCategory::getInstance().error(format(fmt, args...));
}

template< class LoggerCategory, class... Args >
void FATAL_LOG( std::string_view fmt, Args&&... args )
{
    LoggerCategory::getInstance().fatal(format(fmt, args...));
}

}
