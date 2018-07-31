/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Logger_inl_
#define _Stroika_Foundation_Execution_Logger_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Characters/Format.h"

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     ********************************* Logger ***************************************
     ********************************************************************************
     */
    inline Logger& Logger::Get ()
    {
        Ensure (sThe_.fRep_ != nullptr); // good but not great since no guarantee about race - that while used doesn't 'go null' in sThe DTOR
        return sThe_;
    }
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
        return logLevel >= fMinLogLevel_ and GetAppender () != nullptr;
    }
#if !qDefaultTracingOn
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

}

namespace Stroika::Foundation::Configuration {
    template <>
    struct DefaultNames<Execution::Logger::Priority> : EnumNames<Execution::Logger::Priority> {
        static constexpr EnumNames<Execution::Logger::Priority> k{
            EnumNames<Execution::Logger::Priority>::BasicArrayInitializer{
                {
                    {Execution::Logger::Priority::eDebug, L"Debug"},
                    {Execution::Logger::Priority::eInfo, L"Info"},
                    {Execution::Logger::Priority::eNotice, L"Notice"},
                    {Execution::Logger::Priority::eWarning, L"Warning"},
                    {Execution::Logger::Priority::eError, L"Error"},
                    {Execution::Logger::Priority::eCriticalError, L"CriticalError"},
                    {Execution::Logger::Priority::eAlertError, L"AlertError"},
                    {Execution::Logger::Priority::eEmergency, L"Emergency"},
                }}};
        DefaultNames ()
            : EnumNames<Execution::Logger::Priority> (k)
        {
        }
    };
}

#endif /*_Stroika_Foundation_Execution_Logger_inl_*/
