/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"../IO/FileAccessException.h"

#include	"Exceptions.h"

#include	"ErrNoException.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;




using	namespace	Characters;
using	namespace	Execution;

using	Debug::TraceContextBumper;






/*
 ********************************************************************************
 ***************************** errno_ErrorException *****************************
 ********************************************************************************
 */
errno_ErrorException::errno_ErrorException (errno_t e)
	: StringException (TString2Wide (LookupMessage (e)))
	, fError (e)
{
}

TString	errno_ErrorException::LookupMessage (errno_t e)
{
	TChar	buf[2048];
	buf[0] = '\0';
	#if		qPlatform_Windows
		if (_tcserror_s (buf, e) != 0) {
			return buf;
		}
		(void)::_stprintf_s (buf, TSTR ("errno_t error code: 0x%x"), e);
	#elif	qPlatform_POSIX
		/*
		 * A bit quirky - gcc and POSIX handle this API fairly differently. Hopefully I have both cases correct???
		 */
		#if (defined (_XOPEN_SOURCE) && _XOPEN_SOURCE >= 600) && !defined (_GNU_SOURCE)
			if (strerror_r (e, buf, NEltsOf (buf)) == 0) {
				return buf;
			}
		#else
			char* res	=	nullptr;
			if ((res = strerror_r (e, buf, NEltsOf (buf))) != nullptr) {
				return res;
			}
		#endif
		(void) ::snprintf (buf, NEltsOf (buf), TSTR ("errno_t error code: 0x%x"), e);
	#else
		AssertNotImplemented ();
	#endif
	return buf;	
}

void	_NoReturn_	errno_ErrorException::DoThrow (errno_t error)
{
	switch (error) {
		case	ENOMEM: {
			Execution::DoThrow (bad_alloc (), "errno_ErrorException::DoThrow (ENOMEM) - throwing bad_alloc");
		}
		case EACCES: {
			Execution::DoThrow (IO::FileAccessException ());       // don't know if they were reading or writing at this level..., and don't know file name...
		}
		// If I decide to pursue mapping, this maybe a good place to start
		//	http://aplawrence.com/Unixart/errors.html
		//		-- LGP 2009-01-02
#if 0
		case	EPERM: {
			// not sure any point in this unification. Maybe if I added my OWN private 'access denied' exception
			// the mapping/unification would make sense.
			//		-- LGP 2009-01-02
			DbgTrace ("errno_ErrorException::DoThrow (EPERM) - throwing ERROR_ACCESS_DENIED");
			throw Win32Exception (ERROR_ACCESS_DENIED);
		}
#endif
	}
	DbgTrace ("errno_ErrorException (0x%x) - throwing errno_ErrorException", error);
	throw errno_ErrorException (error);
}



