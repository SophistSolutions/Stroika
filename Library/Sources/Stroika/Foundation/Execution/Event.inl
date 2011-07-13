/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2011.  All rights reserved
 */
#ifndef	_Stroika_Foundation_Execution_Event_inl_
#define	_Stroika_Foundation_Execution_Event_inl_	1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include	"WaitAbandonedException.h"
#include	"WaitTimedOutException.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



	// class	Event
		inline	Event::Event (bool manualReset, bool initialState):
			fEventHandle (::CreateEvent (NULL, manualReset, initialState, NULL))
			{
				ThrowIfFalseGetLastError (fEventHandle != NULL);
				#if		qTrack_Execution_HandleCounts
					::InterlockedIncrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	Event::~Event ()
			{
				Verify (::CloseHandle (fEventHandle));
				#if		qTrack_Execution_HandleCounts
					::InterlockedDecrement (&sCurAllocatedHandleCount);
				#endif
			}
		inline	void	Event::Pulse() throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::PulseEvent (fEventHandle));
			}
		inline	void	Event::Reset () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::ResetEvent (fEventHandle));
			}
		inline	void	Event::Set () throw ()
			{
				AssertNotNil (fEventHandle);
				Verify (::SetEvent (fEventHandle));
			}
		inline	void	Event::Wait (float timeout) const
			{
				AssertNotNil (fEventHandle);
				DWORD	milliseconds	=	static_cast<DWORD> (timeout * 1000);
				if (timeout > 1000) {
					milliseconds = INFINITE;	// must be careful about rounding errors in int->float->int
				}
				DWORD	result	=	::WaitForSingleObject (fEventHandle, milliseconds);
				switch (result) {
					case	WAIT_TIMEOUT:	DoThrow (WaitTimedOutException ());
					case	WAIT_ABANDONED:	DoThrow (WaitAbandonedException ());
				}
				Verify (result == WAIT_OBJECT_0);
			}
		inline	Event::operator HANDLE () const
			{
				AssertNotNil (fEventHandle);
				return fEventHandle;
			}


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Event_inl_*/
