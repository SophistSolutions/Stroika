/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Characters/Format.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ********************************* Logger ***************************************
     ********************************************************************************
     */
    inline Logger           Logger::sThe;
    inline Logger::Priority Logger::GetMinLogLevel () const
    {
        return fMinLogLevel_;
    }
    inline void Logger::SetMinLogLevel (Priority minLogLevel)
    {
        fMinLogLevel_ = minLogLevel;
    }
    inline bool Logger::WouldLog (Priority logLevel) const
    {
        return logLevel >= fMinLogLevel_ and not GetAppenders ().empty ();
    }
#if !qStroika_Foundation_Debug_DefaultTracingOn
    inline void Logger::Log (Priority logLevel, const wchar_t* format, ...)
    {
        if (WouldLog (logLevel)) {
            va_list argsList;
            va_start (argsList, format);
            String msg = Characters::FormatV (format, argsList);
            va_end (argsList);
            Log_ (logLevel, msg);
        }
    }
#endif
    template <typename CHAR_T, typename... ARGS>
    inline void Logger::Log (Priority logLevel, const Characters::FormatString<CHAR_T>& fmt, ARGS&&... args)
    {
        using namespace Characters::Literals;
        if (WouldLog (logLevel)) {
            String msg = fmt (args...);
            DbgTrace ("Logger::Log ({}, \"{}\")"_f, logLevel, msg);
            Log_ (logLevel, msg);
        }
        else {
#if qStroika_Foundation_Debug_DefaultTracingOn
            DbgTrace ("...suppressed by WouldLog: {}"_f, fmt (args...));
#endif
        }
    }
    template <Characters::IFormatLiteral FORMAT_LITERAL, typename... ARGS>
    inline void Logger::Log (Priority logLevel, const FORMAT_LITERAL& fmt, ARGS&&... args)
    {
        FORMAT_LITERAL::template CheckArgs<ARGS...> ();
        Log (logLevel, static_cast<const Characters::FormatString<wchar_t>&> (fmt), forward<ARGS> (args)...);
    }

}

namespace Stroika::Foundation::Execution {
    Common::StroikaNaming EnumNamingRule (Logger::Priority);
}
