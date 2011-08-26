/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_
#define	_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_	1

#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Configuration/Common.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	Platform {
				namespace	Windows {
					void	WaitAndPumpMessages (HWND dialog = nullptr, Time::DurationSecondsType forNSecs = 0.1f);
					void	WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, Time::DurationSecondsType forNSecs = 0.1f);
					void	PumpMessagesWhileInputAvailable (HWND dialog = nullptr, Time::DurationSecondsType atMostNSecs = 0.1f);
				}
			}
		}
	}
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#endif	/*_Stroika_Foundation_Execution_Platform_Windows_WaitSupport_h_*/
