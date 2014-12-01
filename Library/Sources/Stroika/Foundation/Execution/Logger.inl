/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Logger_inl_
#define _Stroika_Foundation_Execution_Logger_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ********************************* Logger ***************************************
             ********************************************************************************
             */
#if     !qCompilerAndStdLib_constexpr_Buggy
            constexpr EnumNames<Logger::Priority>   Logger::Stroika_Enum_Names(Priority)
            {
                { Logger::Priority::eDebug, L"Debug" },
                { Logger::Priority::eInfo, L"Info" },
                { Logger::Priority::eNotice, L"Notice" },
                { Logger::Priority::eWarning, L"Warning" },
                { Logger::Priority::eError, L"Error" },
                { Logger::Priority::eCriticalError, L"CriticalError" },
                { Logger::Priority::eAlertError, L"AlertError" },
                { Logger::Priority::eEmergency, L"Emergency" },
            };
#endif
            inline  Logger& Logger::Get ()
            {
                return sThe_;
            }
            inline  Logger::IAppenderRepPtr Logger::GetAppender () const
            {
                return fAppender_;
            }
            inline  Logger::Priority    Logger::GetMinLogLevel () const
            {
                return fMinLogLevel_;
            }
            inline  void        Logger::SetMinLogLevel (Priority minLogLevel)
            {
                fMinLogLevel_ = minLogLevel;
            }
            inline  bool    Logger::WouldLog (Priority logLevel)
            {
                return logLevel >= sThe_.fMinLogLevel_ and sThe_.fAppender_.get () != nullptr;
            }
            inline  bool        Logger::GetBufferingEnabled ()
            {
                return sThe_.fBufferingEnabled_;
            }
            inline  void    Logger::Log (Priority logLevel, String format, ...)
            {
                if (WouldLog (logLevel)) {
                    va_list     argsList;
                    va_start (argsList, format);
                    Log_ (logLevel, format, argsList);
                    va_end (argsList);
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Logger_inl_*/

