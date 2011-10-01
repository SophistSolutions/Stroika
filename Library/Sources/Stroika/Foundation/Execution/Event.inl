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
#include	"AtomicOperations.h"
#include	"WaitAbandonedException.h"
#include	"WaitTimedOutException.h"

namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {



		// class	Event
			inline	Event::Event ()
#if			qPlatform_Windows
				: fEventHandle (::CreateEvent (nullptr, false, false, nullptr))
#endif
				#if		qUseThreads_StdCPlusPlus
					: fMutex_ ()
					, fConditionVariable_ ()
					, fTriggered_ (false)
				#endif
				{
					//bool manualReset, bool initialState
					#if			qPlatform_Windows
						ThrowIfFalseGetLastError (fEventHandle != nullptr);
						#if		qTrack_Execution_HandleCounts
							Execution::AtomicIncrement (&sCurAllocatedHandleCount);
						#endif
					#elif		qUseThreads_StdCPlusPlus
						// initialized above
					#else
						AssertNotImplemented ();
					#endif
				}
			inline	Event::~Event ()
				{
					#if			qPlatform_Windows
						Verify (::CloseHandle (fEventHandle));
						#if		qTrack_Execution_HandleCounts
							AtomicDecrement (&sCurAllocatedHandleCount);
						#endif
					#endif
				}
			inline	void	Event::Reset () throw ()
				{
					#if			qPlatform_Windows
						AssertNotNull (fEventHandle);
						Verify (::ResetEvent (fEventHandle));
					#elif		qUseThreads_StdCPlusPlus
						fMutex_.lock ();
						fTriggered_ = false;
						fMutex_.unlock ();
					#else
						AssertNotImplemented ();
					#endif
				}
			inline	void	Event::Set () throw ()
				{
					#if			qPlatform_Windows
						AssertNotNull (fEventHandle);
						Verify (::SetEvent (fEventHandle));
					#elif		qUseThreads_StdCPlusPlus
						fMutex_.lock ();
						fTriggered_ = true;
						fConditionVariable_.notify_all ();
						fMutex_.unlock ();
					#else
						AssertNotImplemented ();
					#endif
				}
			inline	void	Event::Wait (Time::DurationSecondsType timeout)
				{
					#if			qPlatform_Windows
						AssertNotNull (fEventHandle);
						// must be careful about rounding errors in int->DurationSecondsType->int
						DWORD	milliseconds	=	(timeout > numeric_limits<DWORD>::max ()/2)? INFINITE: static_cast<DWORD> (timeout * 1000);
						DWORD	result	=	::WaitForSingleObject (fEventHandle, milliseconds);
						switch (result) {
							case	WAIT_TIMEOUT:	DoThrow (WaitTimedOutException ());
							case	WAIT_ABANDONED:	DoThrow (WaitAbandonedException ());
						}
						Verify (result == WAIT_OBJECT_0);
					#elif		qUseThreads_StdCPlusPlus
						fMutex_.lock ();
						while (not fTriggered_) {
							fConditionVariable_.wait (fMutex_);
						}
						fMutex_.unlock ();
					#else
						AssertNotImplemented ();
					#endif
				}
			#if			qPlatform_Windows
			inline	Event::operator HANDLE () const
				{
					AssertNotNull (fEventHandle);
					return fEventHandle;
				}
			#endif


		}
	}
}
#endif	/*_Stroika_Foundation_Execution_Event_inl_*/
