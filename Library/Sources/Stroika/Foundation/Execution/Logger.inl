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
            inline  void    Logger::Log (Priority logLevel, const String& format, ...)
            {
                if (logLevel >= sThe_.fMinLogLevel_ and sThe_.fAppender_.get () != nullptr) {
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

