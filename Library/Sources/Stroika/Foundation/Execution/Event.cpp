/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#include	"../StroikaPreComp.h"

#include	"Event.h"


using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;




/*
 ********************************************************************************
 ************************************** Event ***********************************
 ********************************************************************************
 */
#if		qTrack_ThreadUtils_HandleCounts
uint32_t	Event::sCurAllocatedHandleCount		=	0;
#endif
void	Event::Wait (Time::DurationSecondsType timeout)
{
	CheckForThreadAborting ();
	#if			qPlatform_Windows
		AssertNotNull (fEventHandle);
		// must be careful about rounding errors in int->DurationSecondsType->int
	Again:
		DWORD	result	=	::WaitForSingleObjectEx (fEventHandle, Platform::Windows::Duration2Milliseconds (timeout), true);
		switch (result) {
			case	WAIT_TIMEOUT:	DoThrow (WaitTimedOutException ());
			case	WAIT_ABANDONED:	DoThrow (WaitAbandonedException ());
			case	WAIT_IO_COMPLETION:	CheckForThreadAborting (); goto Again;	// roughly right to goto again - should decrement timeout- APC other than for abort - we should just keep waiting
		}
		Verify (result == WAIT_OBJECT_0);
	#elif		qUseThreads_StdCPlusPlus
		std::unique_lock<std::mutex> lock (fMutex_);
		Time::DurationSecondsType	until	=	Time::GetTickCount () + timeout;
		Assert (until >= timeout);	// so no funny overflow issues...
		while (not fTriggered_) {
			CheckForThreadAborting ();
			Time::DurationSecondsType	remaining	=	until - Time::GetTickCount ();
			if (remaining < 0) {
				DoThrow (WaitTimedOutException ());
			}
//tmphack til I figure out this lock/waiting stuff - 
remaining = min (remaining, 5);
			if (fConditionVariable_.wait_for (lock, std::chrono::duration<double> (remaining)) == std::cv_status::timeout) {
//				DoThrow (WaitTimedOutException ());
			}
		}
		fTriggered_ = false	;	// autoreset
	#else
		AssertNotImplemented ();
	#endif
}
