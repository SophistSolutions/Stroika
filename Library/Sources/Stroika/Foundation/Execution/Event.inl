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
			inline	void	Event::Reset ()
				{
					//Debug::TraceContextBumper ctx (TSTR ("Event::Reset"));
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
			inline	void	Event::Set ()
				{
					//Debug::TraceContextBumper ctx (TSTR ("Event::Set"));
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
