/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2012.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThreadAbortException.h"


// SHOULD BETTER HIDE THIS (qUseTLSForSAbortingFlag AND RELATED) implemenation details
#ifndef qUseTLSForSAbortingFlag
#if     defined (__GNUC__)
#define qUseTLSForSAbortingFlag     1
#endif
#endif

// RE-Examine altenrate appraoches to this and beter docuemnt!!!!
//
//      http://bugzilla/show_bug.cgi?id=646
#ifndef qUseTLSForSAbortingFlag
#define qUseTLSForSAbortingFlag     0
#endif
#ifndef qUseSleepExForSAbortingFlag
#define qUseSleepExForSAbortingFlag qPlatform_Windows
#endif


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            class   Thread::Rep_ {
            public:
                Rep_ (const SharedPtr<IRunnable>& runnable);
                ~Rep_ ();

            public:
                static  void    DoCreate (SharedPtr<Rep_>* repSharedPtr);

            public:
                nonvirtual  void    Start ();

            protected:
                nonvirtual  void    Run ();


#if     qUseTLSForSAbortingFlag
            protected:
                bool*   fTLSAbortFlag;          // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
                // a pointer. This is that pointer - so another thread can terminate/abort this thread.
#endif

            protected:
                // Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
                // and is just called by Thread::Abort (). It CAN be hooked by subclassses to do soemthing to
                // force a quicker abort.
                //
                // BUT BEWARE WHEN OVERRIDING - WORKS ON ANOTHER THREAD!!!!
                nonvirtual  void    NotifyOfAbort ();

            protected:
                // Called from WITHIN this thread (asserts thats true), and does throw of ThreadAbortException if in eAborting state
                nonvirtual  void    ThrowAbortIfNeeded () const;

            private:
                static  void    ThreadMain_ (SharedPtr<Rep_>* thisThreadRep) noexcept;

#if         qUseThreads_WindowsNative
            private:
                static  unsigned int    __stdcall   ThreadProc_ (void* lpParameter);
#endif

#if     qPlatform_POSIX
            private:
                static  void    AbortProc_ (SignalIDType signal);
#elif       qUseThreads_WindowsNative
            private:
                static  void    CALLBACK    AbortProc_ (ULONG_PTR lpParameter);
#endif

            public:
                nonvirtual  Thread::IDType              GetID () const;
                nonvirtual  Thread::NativeHandleType    GetNativeHandle ();

            public:
                SharedPtr<IRunnable>    fRunnable;

            private:
                friend class    Thread;

            private:
#if     qUseThreads_StdCPlusPlus
                std::thread     fThread_;
#elif   qUseThreads_WindowsNative
                HANDLE          fThread_;
#endif

            private:
                mutable CriticalSection fStatusCriticalSection;
                Status                  fStatus;
                Event                   fRefCountBumpedEvent_;
                Event                   fOK2StartEvent_;
#if     qUseThreads_StdCPlusPlus
                Event                   fThreadDone_;
#endif
                wstring                 fThreadName_;
            };


            // class    Thread::Rep_
            inline  void    Thread::Rep_::Start ()
            {
                fOK2StartEvent_.Set ();
            }
            inline  void    Thread::Rep_::ThrowAbortIfNeeded () const
            {
#if         qUseThreads_WindowsNative
                Require (GetCurrentThreadID () == GetID ());
#endif
                AutoCriticalSection enterCritcalSection (fStatusCriticalSection);
                if (fStatus == eAborting) {
                    DoThrow (ThreadAbortException ());
                }
            }


            // class    Thread
#if         qUseThreads_WindowsNative
            inline  HANDLE  Thread::GetOSThreadHandle () const
            {
                return fRep_->fThread_;
            }
#endif
#if     qPlatform_POSIX
            inline  SignalIDType        Thread::GetSignalUsedForThreadAbort ()
            {
                return sSignalUsedForThreadAbort_;
            }
#endif
            inline  Thread::IDType  Thread::GetID () const
            {
                if (fRep_.IsNull ()) {
                    return Thread::IDType (0);
                }
                return fRep_->GetID ();
            }
            inline  Thread::NativeHandleType    Thread::GetNativeHandle ()
            {
                if (fRep_.IsNull ()) {
                    return Thread::NativeHandleType (nullptr);
                }
                return fRep_->GetNativeHandle ();
            }
            inline  SharedPtr<IRunnable>    Thread::GetRunnable () const
            {
                if (fRep_.IsNull ()) {
                    return SharedPtr<IRunnable> ();
                }
                return fRep_->fRunnable;
            }
            inline  bool    Thread::operator< (const Thread& rhs) const
            {
                return fRep_ < rhs.fRep_;
            }
            inline  wstring Thread::GetThreadName () const
            {
                return fRep_->fThreadName_;
            }
            inline  Thread::Status  Thread::GetStatus () const
            {
                if (fRep_.IsNull ()) {
                    return eNull;
                }
                return GetStatus_ ();
            }



            inline  Thread::IDType  GetCurrentThreadID ()
            {
#if     qUseThreads_WindowsNative
                return ::GetCurrentThreadId ();
#elif   qUseThreads_StdCPlusPlus
                return this_thread::get_id ();
#else
                AssertNotImplemented ();
                return 0;
#endif
            }


            // class    CheckForThreadAborting
            template    <unsigned int kEveryNTimes>
            void    CheckForThreadAborting ()
            {
                static  unsigned int    n   =   0;
                if (++n % kEveryNTimes == kEveryNTimes - 1) {
                    CheckForThreadAborting ();
                }
            }


        }
    }
}
#endif  /*_Stroika_Foundation_Execution_Thread_inl_*/
