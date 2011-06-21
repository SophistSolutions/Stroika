/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Characters/StringUtils.h"
#include	"../../../Configuration/Common.h"
#include	"../../../Containers/Common.h"
#include	"../../../Debug/Trace.h"
#include	"../../../Time/Realtime.h"
#include	"WaitSupport.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;
using	namespace	Stroika::Foundation::Execution::Platform;
using	namespace	Stroika::Foundation::Execution::Platform::Windows;

using	Time::DurationSecondsType;




/*
 ********************************************************************************
 ********************************** WaitSupport *********************************
 ********************************************************************************
 */

/*
 *	Call this if you want to pump messages and want to block/wait for a while if need be (to avoid busy-waiting).
 */
void	Windows::WaitAndPumpMessages (HWND dialog, float forNSecs)
{
	Windows::WaitAndPumpMessages (dialog, vector<HANDLE> (), forNSecs);
}

void	Windows::WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, float forNSecs)
{
	DurationSecondsType	startAt	=	Time::GetTickCount ();
	DurationSecondsType	endAt	=	startAt + forNSecs;

	for (DurationSecondsType timeLeft  = endAt - Time::GetTickCount (); timeLeft > 0; timeLeft  = endAt - Time::GetTickCount ()) {
		(void)::MsgWaitForMultipleObjectsEx (static_cast<DWORD> (waitOn.size ()), Containers::Start (waitOn), static_cast<int> (timeLeft * 1000), QS_ALLEVENTS, MWMO_INPUTAVAILABLE);
		MSG msg;
		while (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
			try {
				if (dialog == NULL or not ::IsDialogMessage (dialog, &msg)) {
					::TranslateMessage (&msg);
					::DispatchMessage (&msg);
				}
			}
			catch (...) {
			}
			timeLeft  = endAt - Time::GetTickCount ();
			if (timeLeft < 0) {
				break;
			}
		}
	}
}

/*
 *	Call this if you want to pump messages, but return immediately if none available (e.g. when you are
 *	doing something else).
 */
void	Windows::PumpMessagesWhileInputAvailable (HWND dialog, float atMostNSecs)
{
	DurationSecondsType	startAt	=	Time::GetTickCount ();
	DurationSecondsType	endAt	=	startAt + atMostNSecs;

	MSG msg;
	while (::PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) {
		try {
			if (dialog == NULL or not ::IsDialogMessage (dialog, &msg)) {
				::TranslateMessage (&msg);
				::DispatchMessage (&msg);
			}
		}
		catch (...) {
		}
		if (endAt < Time::GetTickCount ()) {
			break;		// should be only rarely triggered, except if atMostNSecs <= 0
		}
	}
}
