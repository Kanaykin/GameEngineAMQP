#include "Log.h"

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"
#include "log4cpp/PropertyConfigurator.hh"

namespace logger
{

    log4cpp::Priority::PriorityLevel convertPriority(LogVerbosity verbosity)
    {
        switch(verbosity)
        {
            case LogVerbosity::NoLogging:
                return log4cpp::Priority::EMERG;
            case LogVerbosity::Fatal:
                return log4cpp::Priority::FATAL;
            case LogVerbosity::Error:
                return log4cpp::Priority::ERROR;
            case LogVerbosity::Warning:
                return log4cpp::Priority::WARN;
            case LogVerbosity::Info:
                return log4cpp::Priority::INFO;
            case LogVerbosity::Debug:
                return log4cpp::Priority::DEBUG;
            case LogVerbosity::All:
                return log4cpp::Priority::DEBUG;
        }
    }

    Logger::Logger(const std::string& category, LogVerbosity verbosity):
    _category(log4cpp::Category::getInstance(category))
    {
		// #HACK FIXME #26
		static log4cpp::Appender::AppenderMapStorageInitializer* ymt = new log4cpp::Appender::AppenderMapStorageInitializer();
		
        log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cout);
        appender1->setLayout(new log4cpp::BasicLayout());
        _category.setPriority(convertPriority(verbosity));
        _category.addAppender(appender1);
    }

    void Logger::debug(const std::string& message)
    {
        _category.debug(message);
    }

    void Logger::info(const std::string& message)
    {
        _category.info(message);
    }

    void Logger::warn(const std::string& message)
    {
        _category.warn(message);
    }

    void Logger::error(const std::string& message)
    {
        _category.error(message);
    }

    void Logger::fatal(const std::string& message)
    {
        _category.fatal(message);
    }
}
