/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_OS_Win32_WaitSupport_h_
#define	_Stroika_Foundation_Execution_OS_Win32_WaitSupport_h_	1

#include	"../../../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#else
	#error "WINDOWS REQUIRED FOR THIS MODULE"
#endif

#include	"../../../Configuration/Basics.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {
			namespace	Platform {
				namespace	Windows {
					void	WaitAndPumpMessages (HWND dialog = NULL, float forNSecs = 0.1f);
					void	WaitAndPumpMessages (HWND dialog, const vector<HANDLE>& waitOn, float forNSecs = 0.1f);
					void	PumpMessagesWhileInputAvailable (HWND dialog = NULL, float atMostNSecs = 0.1f);
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

#endif	/*_Stroika_Foundation_Execution_OS_Win32_WaitSupport_h_*/
