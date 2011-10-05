/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Logger_inl_
#define	_Stroika_Foundation_Execution_Logger_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			inline	Logger&	Logger::Get ()
				{
					return sThe_;
				}
			inline	Memory::SharedPtr<Logger::IAppenderRep>	Logger::GetAppender () const
				{
					return fAppender_;
				}
			inline	void	Logger::Log (Priority logLevel, const String& format, ...)
				{
					// CANNOT DO NUMBNERIC FILTERING TIL I WORK OUT IF I WANT THE SAME GOFFY NUMBERING AS SYSLOG?
					if (/*logLevel >= sThe_.fMinLogLevel_ and*/ not sThe_.fAppender_.IsNull ()) {
						va_list		argsList;
						va_start (argsList, format);
						Log_ (logLevel, format, argsList);
						va_end (argsList);
					}
				}


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Logger_inl_*/

