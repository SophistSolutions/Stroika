/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	<cassert>

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"Assertions.h"



using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Debug;


#if		qDebug

	namespace	{
		void	DefaultAssertionHandler_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
			{
				#if		qPlatform_Windows
					DebugBreak ();
				#elif	defined (__GNUC__)
					__assert_fail (assertionText, fileName, lineNum, functionName);
				#else
					assert (false);
				#endif
			}
	}

	namespace {
		AssertionHandlerType	sAssertFailureHandler_		=	DefaultAssertionHandler_;
	}


	AssertionHandlerType	Stroika::Foundation::Debug::GetAssertionHandler ()
	{
		return sAssertFailureHandler_;
	}

	void	Stroika::Foundation::Debug::SetAssertionHandler (AssertionHandlerType assertionHandler)
	{
		sAssertFailureHandler_ = (assertionHandler == nullptr)? DefaultAssertionHandler_ : assertionHandler;
	}

	void	Stroika::Foundation::Debug::Private::Debug_Trap_ (const char* assertCategory, const char* assertionText, const char* fileName, int lineNum, const char* functionName)
	{
		(sAssertFailureHandler_) (assertCategory, assertionText, fileName, lineNum, functionName);
	}
#endif


