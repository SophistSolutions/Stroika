/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_inl_
#define _Stroika_Foundation_Execution_Synchronized_inl_ 1

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#include "../Debug/Assertions.h"
#include "../Debug/Trace.h"

namespace Stroika::Foundation {
    namespace Execution {

        /*
         ********************************************************************************
         **************************** Synchronized<T, TRAITS> ***************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        template <typename... ARGUMENT_TYPES>
        inline Synchronized<T, TRAITS>::Synchronized (ARGUMENT_TYPES&&... args)
            : fProtectedValue_ (std::forward<ARGUMENT_TYPES> (args)...)
        {
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::Synchronized (const Synchronized& src)
            : fProtectedValue_ (src.load ())
        {
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::operator= (const Synchronized& rhs) -> Synchronized&
        {
            if (&rhs != this) {
                auto                    value   = rhs.load (); // load outside the lock to avoid possible deadlock
                [[maybe_unused]] auto&& critSec = lock_guard{fLock_};
                fProtectedValue_                = value;
            }
            return *this;
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::operator= (const T& rhs) -> Synchronized&
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fLock_};
            fProtectedValue_                = rhs;
            return *this;
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline Synchronized<T, TRAITS>::operator T () const
        {
            return load ();
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline T Synchronized<T, TRAITS>::load () const
        {
            ReadLockType_ fromCritSec{fLock_};
            return fProtectedValue_;
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::store (const T& v)
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fLock_};
            fProtectedValue_                = v;
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::store (T&& v)
        {
            [[maybe_unused]] auto&& critSec = lock_guard{fLock_};
            fProtectedValue_                = move (v);
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::cget () const -> ReadableReference
        {
            return ReadableReference (&fProtectedValue_, &fLock_);
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::rwget () -> WritableReference
        {
            return WritableReference (&fProtectedValue_, &fLock_);
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::operator-> () const -> ReadableReference
        {
            return ReadableReference (&fProtectedValue_, &fLock_);
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::lock_shared () const
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::lock_shared", L"&m=%p", &m};
#endif
            fLock_.lock_shared ();
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::unlock_shared () const
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::unlock_shared", L"&m=%p", &m};
#endif
            fLock_.unlock_shared ();
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::lock () const
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::lock", L"&fLock_=%p", &fLock_};
#endif
            fLock_.lock ();
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        inline void Synchronized<T, TRAITS>::unlock () const
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::unlock", L"&fLock_=%p", &fLock_};
#endif
            fLock_.unlock ();
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        auto Synchronized<T, TRAITS>::Experimental_UnlockUpgradeLock (ReadableReference* readReference) -> WritableReference
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::Experimental_UnlockUpgradeLock", L"&fLock_=%p", &fLock_};
#endif
            AssertNotNull (readReference);
            AssertNotNull (readReference->fSharedLock_);
            if (readReference->fSharedLock_->owns_lock ()) {
                readReference->fSharedLock_->unlock ();
            }
            // @todo maybe need todo try_lock here?? Or maybe this is OK - as is - so long as we release lock first
            return WritableReference (&fProtectedValue_, &fLock_);
        }
        template <typename T, typename TRAITS>
        template <typename TEST_TYPE, typename ENABLE_IF_TEST>
        void Synchronized<T, TRAITS>::Experimental_UpgradeLock2 (const function<void(WritableReference&&)>& doWithWriteLock)
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::Experimental_UpgradeLock2", L"&fLock_=%p", &fLock_};
#endif
            // AssertNotNull (readReference);
            // Assert (readReference->fSharedLock_ == &fLock_);
            //Require (fLock_.owns_shared_lock ());
            if constexpr (TRAITS::kSupportsSharedLocks) {
                fLock_.unlock_shared ();
                // @todo maybe need todo try_lock here?? Or maybe this is OK - as is - so long as we release lock first
                [[maybe_unused]] auto&& cleanup = Execution::Finally ([this]() {
                    fLock_.lock_shared ();
                });
                doWithWriteLock (WritableReference (&fProtectedValue_, &fLock_));
            }
            else {
                doWithWriteLock (WritableReference (&fProtectedValue_, &fLock_));
            }
        }

        /*
         ********************************************************************************
         ***************** Synchronized<T, TRAITS>::ReadableReference *******************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const T* t, nullptr_t)
            : fT (t)
        {
            RequireNotNull (t);
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"ReadableReference::CTOR -- locks (nullptr_t)");
#endif
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const T* t, MutexType* m)
            : fT (t)
            , fSharedLock_ (m)
        {
            RequireNotNull (t);
            RequireNotNull (m);
            if constexpr (TRAITS::kSupportsSharedLocks) {
                m->lock_shared ();
            }
            else {
                m->lock ();
            }
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"ReadableReference::CTOR -- locks (fSharedLock_ mutex_=%p)", m);
#endif
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (ReadableReference&& src)
            : fT (src.fT)
            , fSharedLock_{move (src.fSharedLock_)}
        {
            src.fT           = nullptr;
            src.fSharedLock_ = nullptr;
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::ReadableReference::~ReadableReference ()
        {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
            DbgTrace (L"ReadableReference::DTOR -- locks (fSharedLock_ mutex_=%p)", fSharedLock_);
#endif
            if (fSharedLock_ != nullptr) {
                if constexpr (TRAITS::kSupportsSharedLocks) {
                    fSharedLock_->unlock_shared ();
                }
                else {
                    fSharedLock_->unlock ();
                }
            }
        }
        template <typename T, typename TRAITS>
        inline const T* Synchronized<T, TRAITS>::ReadableReference::operator-> () const
        {
            EnsureNotNull (fT);
            return fT;
        }
        template <typename T, typename TRAITS>
        inline const T& Synchronized<T, TRAITS>::ReadableReference::cref () const
        {
            EnsureNotNull (fT);
            return *fT;
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::ReadableReference::operator const T& () const
        {
            EnsureNotNull (fT);
            return *fT;
        }
        template <typename T, typename TRAITS>
        inline T Synchronized<T, TRAITS>::ReadableReference::load () const
        {
            EnsureNotNull (fT);
            return *fT;
        }

        /*
         ********************************************************************************
         *************** Synchronized<T, TRAITS>::WritableReference *********************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::WritableReference::WritableReference (T* t, MutexType* m)
            : ReadableReference (t, nullptr)
            , fUniqueLock_ (*m)
        {
        }
        template <typename T, typename TRAITS>
        inline Synchronized<T, TRAITS>::WritableReference::WritableReference (WritableReference&& src)
            : ReadableReference (move (src))
            , fUniqueLock_ (move (src.fUniqueLock_))
        {
        }
        template <typename T, typename TRAITS>
        inline auto Synchronized<T, TRAITS>::WritableReference::operator= (T rhs) -> const WritableReference&
        {
            RequireNotNull (this->fT);
            // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
            *const_cast<T*> (this->fT) = rhs;
            return *this;
        }
        template <typename T, typename TRAITS>
        inline T* Synchronized<T, TRAITS>::WritableReference::operator-> ()
        {
            // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
            EnsureNotNull (this->fT);
            return const_cast<T*> (this->fT);
        }
        template <typename T, typename TRAITS>
        inline const T* Synchronized<T, TRAITS>::WritableReference::operator-> () const
        {
            return ReadableReference::operator-> ();
        }
        template <typename T, typename TRAITS>
        inline T& Synchronized<T, TRAITS>::WritableReference::rwref ()
        {
            // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
            EnsureNotNull (this->fT);
            return *const_cast<T*> (this->fT);
        }
        template <typename T, typename TRAITS>
        inline void Synchronized<T, TRAITS>::WritableReference::store (const T& v)
        {
            rwref () = v;
        }
        template <typename T, typename TRAITS>
        inline void Synchronized<T, TRAITS>::WritableReference::store (T&& v)
        {
            rwref () = move (v);
        }

        /*
         ********************************************************************************
         *********************************** operator< **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator< (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () < rhs;
        }
        template <typename T, typename TRAITS>
        inline bool operator< (T lhs, const Synchronized<T, TRAITS>& rhs)
        {
            return lhs < rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline bool operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
        {
            return lhs.load () < rhs.load ();
        }

        /*
         ********************************************************************************
         ********************************** operator<= **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator<= (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () <= rhs;
        }
        template <typename T, typename TRAITS>
        inline bool operator<= (T lhs, const Synchronized<T, TRAITS>& rhs)
        {
            return lhs <= rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline bool operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l <= r;
        }

        /*
         ********************************************************************************
         ********************************** operator== **********************************
          ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator== (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () == rhs;
        }
        template <typename T, typename TRAITS>
        inline bool operator== (T lhs, const Synchronized<T, TRAITS>& rhs)
        {
            return lhs == rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline bool operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l == r;
        }

        /*
         ********************************************************************************
         ********************************** operator!= **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator!= (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () != rhs;
        }
        template <typename T, typename TRAITS>
        inline bool operator!= (T lhs, const Synchronized<T, TRAITS>& rhs)
        {
            return lhs != rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline bool operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l != r;
        }

        /*
         ********************************************************************************
         ********************************** operator>= **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator>= (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () >= rhs;
        }

        /*
         ********************************************************************************
         *********************************** operator> **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline bool operator> (const Synchronized<T, TRAITS>& lhs, T rhs)
        {
            return lhs.load () > rhs;
        }

        /*
         ********************************************************************************
         ********************************** operator^ ***********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline auto operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} ^ T{})
        {
            return lhs.load () ^ rhs;
        }
        template <typename T, typename TRAITS>
        inline auto operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{})
        {
            return lhs ^ rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline auto operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{})
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l ^ r;
        }

        /*
         ********************************************************************************
         ********************************** operator* ***********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline auto operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} * T{})
        {
            return lhs.load () * rhs;
        }
        template <typename T, typename TRAITS>
        inline auto operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{})
        {
            return lhs * rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline auto operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{})
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l * r;
        }

        /*
         ********************************************************************************
         ********************************** operator+ ***********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline auto operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} + T{})
        {
            return lhs.load () + rhs;
        }
        template <typename T, typename TRAITS>
        inline auto operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{})
        {
            return lhs + rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline auto operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{})
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l + r;
        }

        /*
         ********************************************************************************
         ********************************** operator-= **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS, typename RHSTYPE>
        inline auto operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator-= (rhs))
        {
            return lhs.rwget ()->operator-= (rhs);
        }

        /*
         ********************************************************************************
         ********************************** operator+= **********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS, typename RHSTYPE>
        inline auto operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator+= (rhs))
        {
            return lhs.rwget ()->operator+= (rhs);
        }

        /*
         ********************************************************************************
         ********************************** operator- ***********************************
         ********************************************************************************
         */
        template <typename T, typename TRAITS>
        inline auto operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} - T{})
        {
            return lhs.load () + rhs;
        }
        template <typename T, typename TRAITS>
        inline auto operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{})
        {
            return lhs - rhs.load ();
        }
        template <typename T, typename TRAITS>
        inline auto operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{})
        {
            // preload to avoid possible deadlock
            auto l = lhs.load ();
            auto r = rhs.load ();
            return l - r;
        }
    }
}
#endif /*_Stroika_Foundation_Execution_Synchronized_inl_*/
