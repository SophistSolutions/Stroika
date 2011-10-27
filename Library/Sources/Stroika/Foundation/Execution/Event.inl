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
#if		qPlatform_Windows
#include	"Platform/Windows/WaitSupport.h"
#endif


namespace	Stroika {	
	namespace	Foundation {
		namespace	Execution {


			//redeclare to avoid having to include Thread code
			void	CheckForThreadAborting ();

		// class	Event
			inline	Event::Event ()
				#if		qUseThreads_StdCPlusPlus
					: fMutex_ ()
					, fConditionVariable_ ()
					, fTriggered_ (false)
				#elif	qUseThreads_WindowsNative
					: fEventHandle (::CreateEvent (nullptr, false, false, nullptr))
				#endif
				{
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
						#if 1
							// SB able to compare with Time::kInfinite - not sure why not working - but this SB OK... At least for now
							// --LGP 2011-10-21
							bool	forever	=	(timeout > 24 * 60 * 60 * 365);
						#else
							bool	forever	=	(timeout == Time::kInfinite);
						#endif
						Time::DurationSecondsType	until	=	Time::GetTickCount () + timeout;
						while (not fTriggered_) {
							if (forever) {
								fConditionVariable_.wait (lock);
							}
							else {
								Time::DurationSecondsType	remaining	=	Time::GetTickCount () - until;
								if (remaining < 0) {
									DoThrow (WaitTimedOutException ());
								}
								if (fConditionVariable_.wait_for (lock, std::chrono::duration<double> (remaining)) == std::cv_status::timeout) {
									DoThrow (WaitTimedOutException ());
								}
							}
						}
						fTriggered_ = false	;	// autoreset
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
