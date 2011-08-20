/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Event_h_
#define	_Stroika_Foundation_Execution_Event_h_	1

#include	"../StroikaPreComp.h"

#if			qPlatform_Windows
	#include	<windows.h>
#endif

#include	"../Configuration/Common.h"
#include	"../Memory/RefCntPtr.h"

#include	"CriticalSection.h"
#include	"Exceptions.h"


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {

			#ifndef	qTrack_ThreadUtils_HandleCounts
			#define	qTrack_ThreadUtils_HandleCounts		qDebug
			#endif

			class	Event {
			#if		qTrack_ThreadUtils_HandleCounts
				public:
					static	uint32_t	sCurAllocatedHandleCount;
			#endif
				public:
					Event (bool manualReset, bool initialState);
					~Event ();

				private:
					Event (const Event&);						// NOT IMPL
					const Event operator= (const Event&);		// NOT IMPL

				public:
					// Pulse the event (signals waiting objects, then resets)
					nonvirtual	void	Pulse () throw ();

					// Set the event to the non-signaled state
					nonvirtual	void	Reset () throw ();

					// Set the event to the signaled state
					nonvirtual	void	Set () throw ();

					// Simple wait. Can use operator HANDLE() to do fancier waits
					nonvirtual	void	Wait (Time::DurationSecondsType timeout = Time::kInfinite) const;

				#if			qPlatform_Windows
				public:
					operator HANDLE () const;

				private:
					HANDLE	fEventHandle;
				#endif
			};

		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Event_h_*/






/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"Event.inl"
