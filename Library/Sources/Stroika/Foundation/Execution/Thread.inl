/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Thread_inl_
#define _Stroika_Foundation_Execution_Thread_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "WaitableEvent.h"
#include <atomic>
#include <list>

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            namespace PRIVATE_ {
                enum class InterruptFlagState_ {
                    eNone,
                    eInterrupted,
                    eAborted,

                    Stroika_Define_Enum_Bounds (eNone, eAborted)
                };
                using InterruptFlagType_ = atomic<InterruptFlagState_>;
            }

            /*
             ********************************************************************************
             ********************************* Thread::Rep_ *********************************
             ********************************************************************************
             */
            class Thread::Rep_ {
            public:
                Rep_ (const Function<void()>& runnable, const Memory::Optional<Configuration>& configuration);
                ~Rep_ ();

            public:
                static void DoCreate (shared_ptr<Rep_>* repSharedPtr);

            public:
                nonvirtual void Start ();

            public:
                nonvirtual Thread::IDType GetID () const;

            public:
                nonvirtual Thread::NativeHandleType GetNativeHandle ();

            public:
                nonvirtual Characters::String ToString () const;

            public:
                void ApplyThreadName2OSThreadObject ();

            private:
                nonvirtual void Run_ ();

            private:
                // Called - typically from ANOTHER thread (but could  be this thread). By default this does nothing,
                // and is just called by Thread::Abort (). It sets (indirectly) the thread-local-storage aborted
                // flag for the target thread. And if called from an aborting thread, it may throw
                nonvirtual void NotifyOfInterruptionFromAnyThread_ (bool aborting);

            private:
                static void ThreadMain_ (shared_ptr<Rep_>* thisThreadRep) noexcept;

            private:
#if qPlatform_POSIX
                static void InteruptionSignalHandler_ (SignalID signal);
#elif qPlatform_Windows
                static void CALLBACK CalledInRepThreadAbortProc_ (ULONG_PTR lpParameter);
#endif

            private:
                using InterruptFlagState_ = PRIVATE_::InterruptFlagState_;
                using InterruptFlagType_  = PRIVATE_::InterruptFlagType_;

            private:
                Function<void()> fRunnable_;
                // We use a global variable (thread local) to store the abort flag. But we must access it from ANOTHER thread typically - using
                // a pointer. This is that pointer - so another thread can terminate/abort this thread.
                InterruptFlagType_* fTLSInterruptFlag_{};   // regular interrupt, abort interrupt, or none
                mutable std::mutex  fAccessSTDThreadMutex_; // rarely needed but to avoid small race as we shutdown thread, while we join in one thread and call GetNativeThread() in another
                std::thread         fThread_;
                atomic<Status>      fStatus_;
                WaitableEvent       fRefCountBumpedEvent_;
                WaitableEvent       fOK2StartEvent_;
                WaitableEvent       fThreadDone_;
                wstring             fThreadName_;
                exception_ptr       fSavedException_;

            private:
                friend class Thread;
            };

            /*
             ********************************************************************************
             *********************************** Thread::Rep_ *******************************
             ********************************************************************************
             */
            inline void Thread::Rep_::Start ()
            {
                fOK2StartEvent_.Set ();
            }

            /*
             ********************************************************************************
             ************************************* Thread ***********************************
             ********************************************************************************
             */
            template <typename FUNCTION>
            inline Thread::Thread (FUNCTION f, const Memory::Optional<Characters::String>& name, const Memory::Optional<Configuration>& configuration, typename enable_if<is_function<FUNCTION>::value>::type*)
                : Thread (Function<void()> (f), name, configuration)
            {
            }
            template <typename FUNCTION>
            inline Thread::Thread (FUNCTION f, AutoStartFlag flag, const Memory::Optional<Characters::String>& name, const Memory::Optional<Configuration>& configuration, typename enable_if<is_function<FUNCTION>::value>::type*)
                : Thread (Function<void()> (f), flag, name, configuration)
            {
            }
            inline Thread::Thread (const shared_ptr<Rep_>& rep)
                : fRep_ (rep)
            {
            }

#if qPlatform_POSIX
            inline SignalID Thread::GetSignalUsedForThreadInterrupt ()
            {
                return sSignalUsedForThreadInterrupt_;
            }
#endif
            inline Thread::IDType Thread::GetID () const
            {
                if (fRep_ == nullptr) {
                    return Thread::IDType ();
                }
                return fRep_->GetID ();
            }
            inline Thread::NativeHandleType Thread::GetNativeHandle () noexcept
            {
                if (fRep_ == nullptr) {
                    return Thread::NativeHandleType (0); // on some systems (e.g. AIX64 7.1) this is not a pointer type and assign nullptr illegal
                }
                return fRep_->GetNativeHandle ();
            }
            inline Function<void()> Thread::GetFunction () const
            {
                if (fRep_ == nullptr) {
                    return nullptr;
                }
                return fRep_->fRunnable_;
            }
            inline bool Thread::operator< (const Thread& rhs) const
            {
                return fRep_ < rhs.fRep_;
            }
            inline Thread::Status Thread::GetStatus () const noexcept
            {
                if (fRep_ == nullptr) {
                    return Status::eNull;
                }
                return GetStatus_ ();
            }
            inline void Thread::Start (const Traversal::Iterable<Thread::Ptr>& threads)
            {
                for (Thread&& t : threads) {
                    t.Start ();
                }
            }
            inline void Thread::WaitForDone (Time::DurationSecondsType timeout) const
            {
                WaitForDoneUntil (timeout + Time::GetTickCount ());
            }
            inline void Thread::WaitForDone (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeout)
            {
                WaitForDoneUntil (threads, timeout + Time::GetTickCount ());
            }
            inline void Thread::AbortAndWaitForDone (Time::DurationSecondsType timeout)
            {
                AbortAndWaitForDoneUntil (timeout + Time::GetTickCount ());
            }
            inline void Thread::AbortAndWaitForDone (const Traversal::Iterable<Thread::Ptr>& threads, Time::DurationSecondsType timeout)
            {
                AbortAndWaitForDoneUntil (threads, timeout + Time::GetTickCount ());
            }

            /*
             ********************************************************************************
             ******************************** Thread::Ptr ***********************************
             ********************************************************************************
             */
            inline Thread::Ptr::Ptr (const Thread& src)
                : Thread (src.fRep_)
            {
            }
            inline Thread::Ptr::Ptr (const Ptr& src)
                : Thread (src.fRep_)
            {
            }
            inline Thread::Ptr::Ptr (Ptr&& src)
                : Thread (move (src.fRep_))
            {
            }
            inline Thread::Ptr& Thread::Ptr::operator= (const Ptr& rhs)
            {
                fRep_ = rhs.fRep_;
                return *this;
            }
            inline Thread::Ptr& Thread::Ptr::operator= (Ptr&& rhs)
            {
                fRep_ = move (rhs.fRep_);
                return *this;
            }
            inline Thread::Ptr& Thread::Ptr::operator= (const Thread& rhs)
            {
                fRep_ = rhs.fRep_;
                return *this;
            }
            inline Thread::Ptr& Thread::Ptr::operator= (Thread&& rhs)
            {
                fRep_ = move (rhs.fRep_);
                return *this;
            }

            /*
             ********************************************************************************
             *************************** GetCurrentThreadID *********************************
             ********************************************************************************
             */
            inline Thread::IDType GetCurrentThreadID () noexcept
            {
                return this_thread::get_id ();
            }

            /*
             ********************************************************************************
             *********************** CheckForThreadInterruption *****************************
             ********************************************************************************
             */
            template <unsigned int kEveryNTimes>
            void                   CheckForThreadInterruption ()
            {
                // note that it is not important that this be protected/thread safe, since the value is just advisory/hint
                static unsigned int n = 0;
                if (++n % kEveryNTimes == kEveryNTimes - 1) {
                    CheckForThreadInterruption ();
                }
            }
        }
    }
}
namespace Stroika {
    namespace Foundation {
        namespace Configuration {
            template <>
            struct DefaultNames<Execution::Thread::Status> : EnumNames<Execution::Thread::Status> {
                static constexpr EnumNames<Execution::Thread::Status> k{
                    EnumNames<Execution::Thread::Status>::BasicArrayInitializer{
                        {
                            {Execution::Thread::Status::eNull, L"Null"},
                            {Execution::Thread::Status::eNotYetRunning, L"Not-Yet-Running"},
                            {Execution::Thread::Status::eRunning, L"Running"},
                            {Execution::Thread::Status::eAborting, L"Aborting"},
                            {Execution::Thread::Status::eCompleted, L"Completed"},
                        }}};
                DefaultNames ()
                    : EnumNames<Execution::Thread::Status> (k)
                {
                }
            };
        }
    }
}
#endif /*_Stroika_Foundation_Execution_Thread_inl_*/
