/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#include	"../../StroikaPreComp.h"

#if		defined(_WIN32)
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../Characters/StringUtils.h"
#include	"../../Configuration/Basics.h"
#include	"../../Containers/Basics.h"
#include	"../../Debug/Trace.h"
#include	"../../Time/Realtime.h"
#include	"WaitSupport.h"


using	namespace	Stroika;
using	namespace	Stroika::Foundation;
using	namespace	Stroika::Foundation::Execution;

using	Time::TickCountType;




/*
 ********************************************************************************
 ********************************** WaitSupport *********************************
 ********************************************************************************
 */

/*
 *	Call this if you want to pump messages and want to block/wait for a while if need be (to avoid busy-waiting).
 */
void	OS_Win32::WaitAndPumpMessages (HWND dialog, float forNSecs)
{
	OS_Win32::WaitAndPumpMessages (dialog, vector<HANDLE> (), forNSecs);
}

void	OS_Win32::WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, float forNSecs)
{
	TickCountType	startAt	=	Time::GetTickCount ();
	TickCountType	endAt	=	startAt + forNSecs;

	for (TickCountType timeLeft  = endAt - Time::GetTickCount (); timeLeft > 0; timeLeft  = endAt - Time::GetTickCount ()) {
		(void)::MsgWaitForMultipleObjectsEx (waitOn.size (), Containers::Start (waitOn), static_cast<int> (timeLeft * 1000), QS_ALLEVENTS, MWMO_INPUTAVAILABLE);
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
void	OS_Win32::PumpMessagesWhileInputAvailable (HWND dialog, float atMostNSecs)
{
	TickCountType	startAt	=	Time::GetTickCount ();
	TickCountType	endAt	=	startAt + atMostNSecs;

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
