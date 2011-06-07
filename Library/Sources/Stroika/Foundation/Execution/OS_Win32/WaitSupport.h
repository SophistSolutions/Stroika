/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	__WaitSupport_h__
#define	__WaitSupport_h__	1

#include	"../../StroikaPreComp.h"

#if		defined(_WIN32)
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../Configuration/Basics.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	OS_Win32 {
				void	WaitAndPumpMessages (HWND dialog = NULL, float forNSecs = 0.1f);
				void	WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, float forNSecs = 0.1f);
				void	PumpMessagesWhileInputAvailable (HWND dialog = NULL, float atMostNSecs = 0.1f);
			}
		}
	}
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */


#endif	/*__WaitSupport_h__*/
