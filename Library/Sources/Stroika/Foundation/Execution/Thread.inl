/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "ThreadAbortException.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /*
             ********************************************************************************
             ********************************* Thread::Rep_ *********************************
             ********************************************************************************
             */
            class   Thread::Rep_ {
            public:
                Rep_ (const IRunnablePtr& runnable);
                ~Rep_ ();

            public:
                static  void    DoCreate (shared_ptr<Rep_>* repSharedPtr);

            public:
                nonvirtual  void    Start ();

            public:
                nonvirtual  Thread::IDType              GetID () const;

            public:
                nonvirtual  Thread::NativeHandleType    GetNativeHandle ();

            private:
                shared_ptr<IRunnable>    fRunnable_;

            private:
                nonvirtual  void    Run_ ();

            private:
                // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
                // a pointer. This is that pointer - so another thread can terminate/abort this thread.
                bool*   fTLSAbortFlag_;

            private:
                // Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
                // and is just called by Thread::Abort (). It CAN be hooked by subclassses to do soemthing to
                // force a quicker abort.
                //
                // BUT BEWARE WHEN OVERRIDING - WORKS ON ANOTHER THREAD!!!!
                nonvirtual  void    NotifyOfAbortFromAnyThread_ ();

            private:
                // Called from WITHIN this thread (asserts thats true), and does throw of ThreadAbortException if in eAborting state
                nonvirtual  void    ThrowAbortIfNeededFromRepThread_ () const;

            private:
                static  void    ThreadMain_ (shared_ptr<Rep_>* thisThreadRep) noexcept;

#if         qUseThreads_WindowsNative
            private:
                static  unsigned int    __stdcall   ThreadProc_ (void* lpParameter);
#endif

            private:
#if         qPlatform_POSIX
                static  void    CalledInRepThreadAbortProc_ (SignalIDType signal);
#elif       qPlatform_Windows
                static  void    CALLBACK    CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif

            private:
#if     qUseThreads_StdCPlusPlus
                std::thread             fThread_;
#elif   qUseThreads_WindowsNative
                HANDLE                  fThread_;
#endif
                mutable recursive_mutex fStatusCriticalSection_;
                Status                  fStatus_;
                Event                   fRefCountBumpedEvent_;
                Event                   fOK2StartEvent_;
#if     qUseThreads_StdCPlusPlus
                Event                   fThreadDone_;
#endif
                wstring                 fThreadName_;

            private:
                friend class    Thread;
            };


            /*
             ********************************************************************************
             *********************************** Thread::Rep_ *******************************
             ********************************************************************************
             */
            inline  void    Thread::Rep_::Start ()
            {
                fOK2StartEvent_.Set ();
            }
            inline  void    Thread::Rep_::ThrowAbortIfNeededFromRepThread_ () const
            {
                Require (GetCurrentThreadID () == GetID ());
                lock_guard<recursive_mutex> enterCritcalSection (fStatusCriticalSection_);
                if (fStatus_ == Status::eAborting) {
                    DoThrow (ThreadAbortException ());
                }
            }


            /*
             ********************************************************************************
             ************************************* Thread ***********************************
             ********************************************************************************
             */
#if     qPlatform_POSIX
            inline  SignalIDType        Thread::GetSignalUsedForThreadAbort ()
            {
                return sSignalUsedForThreadAbort_;
            }
#endif
            inline  Thread::IDType  Thread::GetID () const
            {
                if (fRep_.get () == nullptr) {
#if     qUseThreads_WindowsNative
                    return Thread::IDType (0);
#elif   qPlatform_POSIX
                    return Thread::IDType ();
#endif
                }
                return fRep_->GetID ();
            }
            inline  Thread::NativeHandleType    Thread::GetNativeHandle () noexcept {
                if (fRep_.get () == nullptr) {
                    return Thread::NativeHandleType (nullptr);
                }
                return fRep_->GetNativeHandle ();
            }
            inline  shared_ptr<IRunnable>    Thread::GetRunnable () const
            {
                if (fRep_.get () == nullptr) {
                    return shared_ptr<IRunnable> ();
                }
                return fRep_->fRunnable_;
            }
            inline  bool    Thread::operator< (const Thread& rhs) const
            {
                return fRep_ < rhs.fRep_;
            }
            inline  Thread::Status  Thread::GetStatus () const noexcept
            {
                if (fRep_.get () == nullptr) {
                    return Status::eNull;
                }
                return GetStatus_ ();
            }
            inline  wstring Thread::GetThreadName () const
            {
                Require (GetStatus () != Status::eNull);
                return fRep_->fThreadName_;
            }


            /*
             ********************************************************************************
             *************************** GetCurrentThreadID *********************************
             ********************************************************************************
             */
            inline  Thread::IDType  GetCurrentThreadID () noexcept {
#if     qUseThreads_WindowsNative
                return ::GetCurrentThreadId ();
#elif   qUseThreads_StdCPlusPlus
                return this_thread::get_id ();
#else
                AssertNotImplemented ();
                return 0;
#endif
            }


            /*
             ********************************************************************************
             ************************* CheckForThreadAborting *******************************
             ********************************************************************************
             */
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
