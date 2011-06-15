/*
 * Copyright(c) Records For Living, Inc. 2004-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_ThreadUtils_h_
#define	_Stroika_Foundation_Execution_ThreadUtils_h_	1

#include	"../StroikaPreComp.h"

#if		qPlatform_Windows
	#include	<Windows.h>
#endif

#include	"../Configuration/Basics.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"
#include	"Event.h"
#include	"IRunnable.h"
#include	"SimpleThread.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			class	CopyableCriticalSection {
				private:
					Memory::RefCntPtr<CriticalSection>	fCritSec;
				public:
					CopyableCriticalSection ();

				public:
					nonvirtual	void	Lock (); 
					nonvirtual	void	Unlock ();
					nonvirtual	operator CRITICAL_SECTION& ();
			};


			// MAIN reason to use this - is it sets the 'alertable' flag on the sleep, so the QueueUserAPC () stuff works!
			// which allows SimpleThread::Stop () to work properly...
			// -- LGP 2009-04-28
			void	Sleep (Time::DurationSecondsType seconds2Wait);



		}
	}
}
#endif	/*_Stroika_Foundation_Execution_ThreadUtils_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"ThreadUtils.inl"
