#pragma once
#include <format>
#include <iostream>
#include <string>
#include <string_view>

#include "log4cpp/Category.hh"
#include "log4cpp/Appender.hh"
#include "log4cpp/FileAppender.hh"
#include "log4cpp/OstreamAppender.hh"
#include "log4cpp/Layout.hh"
#include "log4cpp/BasicLayout.hh"
#include "log4cpp/Priority.hh"

namespace logger
{

enum class LogVerbosity
{
    NoLogging        = 0,
    Fatal,
    Error,
    Warning,
    Display,
    Log,
    Verbose,
    VeryVerbose,
    All              = VeryVerbose
};

#define DECLARE_LOG_CATEGORY(ClassName) \
log4cpp::Category& root = log4cpp::Category::getRoot(); \
log4cpp::Category& ClassName = log4cpp::Category::getInstance(std::string(#ClassName));

template< class... Args >
std::string DEBUG_LOG( log4cpp::Category& category, std::string_view fmt, Args&&... args )
{
    category.warn("sub1 warn");
}

template< class... Args >
std::string INFO_LOG( log4cpp::Category& category, std::string_view fmt, Args&&... args )
{
    category.warn("sub1 warn");
}

template< class... Args >
std::string WARNING_LOG( log4cpp::Category& category, std::string_view fmt, Args&&... args )
{
    category.warn("sub1 warn");
}

template< class... Args >
std::string ERROR_LOG( log4cpp::Category& category, std::string_view fmt, Args&&... args )
{
    category.warn("sub1 warn");
}

template< class... Args >
std::string FATAL_LOG( log4cpp::Category& category, std::string_view fmt, Args&&... args )
{
    category.warn("sub1 warn");
}

}
