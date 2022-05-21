#pragma once

#include "Log.h"

namespace messages_system
{

DECLARE_LOG_CATEGORY(MsgSysLog, logger::LogVerbosity::All)

template< class... Args >
void DEBUG_LOG(std::string_view fmt, Args&&... args )
{
    logger::DEBUG_LOG<MsgSysLog>(fmt, args ...);
}

template< class... Args >
void INFO_LOG( std::string_view fmt, Args&&... args )
{
    logger::INFO_LOG<MsgSysLog>(fmt, args ...);
}

template< class... Args >
void WARNING_LOG( std::string_view fmt, Args&&... args )
{
    logger::WARNING_LOG<MsgSysLog>(fmt, args ...);
}

template< class... Args >
void ERROR_LOG(std::string_view fmt, Args&&... args )
{
    logger::ERROR_LOG<MsgSysLog>(fmt, args ...);
}

template< class... Args >
void FATAL_LOG( std::string_view fmt, Args&&... args )
{
    logger::FATAL_LOG<MsgSysLog>(fmt, args ...);
}


}

