/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Event_inl_
#define _Stroika_Foundation_Execution_Event_inl_    1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "WaitAbandonedException.h"
#include    "WaitTimedOutException.h"
#if     qUseThreads_WindowsNative
#include    "Platform/Windows/WaitSupport.h"
#include    "Platform/Windows/Exception.h"
#include    "Platform/Windows/HRESULTErrorException.h"
#endif


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            //redeclare to avoid having to include Thread code
            void    CheckForThreadAborting ();


            /*
             ********************************************************************************
             ******************************** Execution::Event ******************************
             ********************************************************************************
             */
            inline  Event::Event ()
#if     qUseThreads_StdCPlusPlus
                : fMutex_ ()
                , fConditionVariable_ ()
                , fTriggered_ (false)
#elif   qUseThreads_WindowsNative
                : fEventHandle (::CreateEvent (nullptr, false, false, nullptr))
#endif
            {
#if         qUseThreads_WindowsNative
                Platform::Windows::ThrowIfFalseGetLastError (fEventHandle != nullptr);
#elif       qUseThreads_StdCPlusPlus
                // initialized above
#else
                AssertNotImplemented ();
#endif
#if     qTrack_Execution_HandleCounts
                Execution::AtomicIncrement (&sCurAllocatedHandleCount);
#endif
            }
            inline  Event::~Event ()
            {
#if     qTrack_Execution_HandleCounts
                AtomicDecrement (&sCurAllocatedHandleCount);
#endif
#if         qUseThreads_WindowsNative
                Verify (::CloseHandle (fEventHandle));
#endif
            }
            inline  void    Event::Reset ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("Event::Reset"));
#if         qUseThreads_WindowsNative
                AssertNotNull (fEventHandle);
                Verify (::ResetEvent (fEventHandle));
#elif       qUseThreads_StdCPlusPlus
                {
                    std::lock_guard<std::mutex> lockGaurd (fMutex_);
                    fTriggered_ = false;
                }
#else
                AssertNotImplemented ();
#endif
            }
            inline  void    Event::Set ()
            {
                //Debug::TraceContextBumper ctx (SDKSTR ("Event::Set"));
#if         qUseThreads_WindowsNative
                AssertNotNull (fEventHandle);
                Verify (::SetEvent (fEventHandle));
#elif       qUseThreads_StdCPlusPlus
                {
                    std::lock_guard<std::mutex> lockGaurd (fMutex_);
                    fTriggered_ = true;
                    fConditionVariable_.notify_all ();
                }
#else
                AssertNotImplemented ();
#endif
            }
#if         qUseThreads_WindowsNative
            inline  Event::operator HANDLE () const
            {
                AssertNotNull (fEventHandle);
                return fEventHandle;
            }
#endif


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Event_inl_*/
