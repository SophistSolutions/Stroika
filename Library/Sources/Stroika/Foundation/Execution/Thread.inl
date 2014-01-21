/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_   1


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <atomic>
#include    <list>
#include    "Lockable.h"
#include    "ThreadAbortException.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {
// experiment with this -- LGP 2014-01-14
#define qUSE_MUTEX_FOR_STATUS_FIELD_    0


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
                nonvirtual  void    Run_ ();

            private:
                // Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
                // and is just called by Thread::Abort (). It sets (indirectly) the thread-local-storage aborted
                // flag for the target thread. And if called from an aborting thread, it may throw
                nonvirtual  void    NotifyOfAbortFromAnyThread_ ();

            private:
                static  void    ThreadMain_ (shared_ptr<Rep_>* thisThreadRep) noexcept;

            private:
#if         qPlatform_POSIX
                static  void    CalledInRepThreadAbortProc_ (SignalID signal);
#elif       qPlatform_Windows
                static  void    CALLBACK    CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif


            private:
#if     qCompilerAndStdLib_thread_local_with_atomic_keyword_Buggy
                using   AbortFlagType_  =   volatile bool;
#else
                using   AbortFlagType_  =   atomic<bool>;
#endif

            private:
                shared_ptr<IRunnable>   fRunnable_;
                // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
                // a pointer. This is that pointer - so another thread can terminate/abort this thread.
                AbortFlagType_*         fTLSAbortFlag_;
                std::thread             fThread_;
#if     qUSE_MUTEX_FOR_STATUS_FIELD_
                mutable recursive_mutex fStatusCriticalSection_;
#endif
                std::atomic<Status>     fStatus_;
                Event                   fRefCountBumpedEvent_;
                Event                   fOK2StartEvent_;
                Event                   fThreadDone_;
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


            /*
             ********************************************************************************
             ************************************* Thread ***********************************
             ********************************************************************************
             */
            template <typename FUNCTION>
            inline  Thread::Thread (FUNCTION f, typename is_function<FUNCTION>::type*) :
                Thread (std::function<void()>(f))
            {
            }
#if     qPlatform_POSIX
            inline  SignalID        Thread::GetSignalUsedForThreadAbort ()
            {
                return sSignalUsedForThreadAbort_;
            }
#endif
            inline  Thread::IDType  Thread::GetID () const
            {
                if (fRep_.get () == nullptr) {
#if     qPlatform_POSIX
                    return Thread::IDType ();
#endif
                }
                return fRep_->GetID ();
            }
            inline  Thread::NativeHandleType    Thread::GetNativeHandle () noexcept {
                if (fRep_.get () == nullptr)
                {
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
                return this_thread::get_id ();
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
