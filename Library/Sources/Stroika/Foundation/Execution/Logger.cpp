/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#if		qHas_Syslog
	#include	<syslog.h>
#endif

#include	"../Characters/Format.h"
#include	"../Debug/Trace.h"
#include	"Process.h"

#include	"Logger.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;



Logger	Logger::sThe_;

Logger::Logger ()
	: fAppender_ ()
	, fMinLogLevel_ (eInfo_P)
{
}

void	Logger::SetAppender (Memory::SharedPtr<IAppenderRep> rep)
{
	fAppender_ = rep;
}

void	Logger::Log_ (Priority logLevel, const String& format, va_list argList)
{
	Memory::SharedPtr<IAppenderRep>	tmp	=	sThe_.fAppender_;	// avoid races and critical sections
	if (not tmp.IsNull ()) {
		tmp->Log (logLevel, Characters::FormatV (format.c_str (), argList));
	}
}








Logger::IAppenderRep::~IAppenderRep ()
{
}






#if		qHas_Syslog
namespace	{
	TString	mkMsg_ (const String& applicationName)
		{
			return Characters::Format (TSTR ("%s[%d]"), applicationName.AsTString ().c_str (), GetCurrentProcessID ());
		}
}
Logger::SysLogAppender::SysLogAppender (const String& applicationName)
	: fApplicationName_ (mkMsg_ (applicationName))
{
	openlog (fApplicationName_.c_str (), 0, LOG_DAEMON);	// not sure what facility to pass?
}

Logger::SysLogAppender::SysLogAppender (const String& applicationName, int facility)
	: fApplicationName_ (mkMsg_ (applicationName))
{
	openlog (fApplicationName_.c_str (), 0, facility);
}

Logger::SysLogAppender::~SysLogAppender ()
{
	closelog ();
}

void	Logger::SysLogAppender::Log (Priority logLevel, const String& message) override
{
	DbgTrace (L"%s", message.c_str ());
	syslog (logLevel, "%s", message.AsTString ().c_str ());
}
#endif






Logger::FileAppender::FileAppender (const String& fileName)
{
	AssertNotImplemented ();
}
void	Logger::FileAppender::Log (Priority logLevel, const String& message) override
{
	AssertNotImplemented ();
}


