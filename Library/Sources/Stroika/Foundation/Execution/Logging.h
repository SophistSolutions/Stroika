/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Logging_h_
#define	_Stroika_Foundation_Execution_Logging_h_	1

#include	"../StroikaPreComp.h"

#include	"../Configuration/Common.h"
#include	"../Characters/String.h"
#include	"../Debug/Assertions.h"
#include	"../Memory/SharedPtr.h"



namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			using	namespace	Configuration;

			using	Characters::String;



#ifndef	qHas_Syslog
#define	qHas_Syslog		qPlatform_POSIX
#endif


			/*
			 *	OVERVIEW:
			 *
			 *		The point of the Logging Module is to provide a simple, portable wrapper on end-user-targetted application logging.
			 *	This form of logging is the kind of logging you leave builtin to your application, and write focused on end-user readability.
			 *	It is NOT (primarily) for debugging (for that - use the Stroika::Foundation::Debug::Trace module).
			 *
			 *		Think of this as a simple/portable wrapper on syslog/log4j/WindowsEventlog.
			 *
			 *		The Logger is a singleton object. It can be set at any number of application logging levels. And it will write information to the
			 *	backend logger its setup with. But default - this is none.
			 *
			 *		To use the logger and actually get logging - pick a logger rep, and call SetLoggingAppender ();
			 *
			 *		This logging API CANNOT be used before main () has started, or after main () has completed [if we choose to allow that we must use ModuleInit,
			 *		but since this requires a call to setup a logger, that seems unlikely]
			 *
			 */
			class	Logger {
				public:
					// See syslog for enumeration of various targets/etc.

				public:
					class	IAppenderRep;
				private:
					Memory::SharedPtr<IAppenderRep>	fRep_;

				public:
				#if		qHas_Syslog
					class	SysLogAppender;
				#endif
					class	FileAppender;
				#if		qPlatform_Windows
					class	WindowsEventLogAppender;
				#endif

				public:
					static	Logger&	Get ();
				private:
					Logger ();

				public:
					nonvirtual	Memory::SharedPtr<IAppenderRep>	GetAppender () const;
					nonvirtual	void							SetAppender (Memory::SharedPtr<IAppenderRep> rep);

				private:
					Memory::SharedPtr<IAppenderRep>	fAppender_;

				public:
					int	fLogLevel_;	// SB PRIVATE
					// Get/Set LogLevel - this affects what WE EAT INLINE versus passon

				public:
					// DoLog
					static	void	Log (int logLevel, const String& format, ...);	// varargs logger
				private:
					static	void	Log_ (int logLevel, const String& format, va_list argList);


				private:
					static	Logger	sThe_;
			};


			class	Logger::IAppenderRep {
				public:
					virtual ~IAppenderRep ();
				public:
					virtual	void	Log (int logLevel, const String& message) = 0;
			};
			#if		qHas_Syslog
			class	Logger::SysLogAppender : public Logger::IAppenderRep {
				public:
					SysLogAppender (const String& applicationName);
					SysLogAppender (const String& applicationName, int facility);
					~SysLogAppender ();
				public:
					virtual	void	Log (int logLevel, const String& message) override;
				private:
					String	fApplicationName_;
			};
			#endif
			class	Logger::FileAppender : public Logger::IAppenderRep {
				public:
					FileAppender (const String& fileName);
				public:
					virtual	void	Log (int logLevel, const String& message) override;
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Logging_h_*/




/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Logging.inl"
