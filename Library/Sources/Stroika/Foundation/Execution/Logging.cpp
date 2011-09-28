/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qHas_Syslog
#include	<syslog.h>
#endif

#include	"../Characters/Format.h"

#include	"Logging.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




Logger::Logger ()
	: fAppender_ ()
	, fLogLevel_ (0)
{
}

void	Logger::SetAppender (Memory::SharedPtr<IAppenderRep> rep)
{
	fAppender_ = rep;
}

void	Logger::Log_ (int logLevel, const String& format, va_list argList)
{
	Memory::SharedPtr<IAppenderRep>	tmp	=	sThe_.fAppender_;	// avoid races and critical sections
	if (not tmp.IsNull ()) {
		tmp->Log (logLevel, Characters::FormatV (format.As<wstring> ().c_str (), argList));
	}
}








Logger::IAppenderRep::~IAppenderRep ()
{
}




#if		qHas_Syslog
Logger::SysLogAppender::SysLogAppender (const String& applicationName)
	: fApplicationName_ (applicationName)
{
	openlog (fApplicationName_.AsTString ().c_str (), 0, LOG_DAEMON);	// not sure what facility to pass?
}

Logger::SysLogAppender::SysLogAppender (const String& applicationName, int facility)
	: fApplicationName_ (applicationName)
{
	openlog (fApplicationName_.AsTString ().c_str (), 0, facility);
}

Logger::SysLogAppender::~SysLogAppender ()
{
	closelog ();
}

void	Log (int logLevel, const String& message) override
{
	syslog (logLevel, "%s", message.AsTString ().c_str ());
}
#endif


Logger::FileAppender::FileAppender (const String& fileName)
{
	AssertNotImplemented ();
}
void	Logger::FileAppender::Log (int logLevel, const String& message) override
{
	AssertNotImplemented ();
}


