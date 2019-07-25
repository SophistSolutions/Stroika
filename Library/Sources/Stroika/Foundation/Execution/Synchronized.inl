/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
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

namespace Stroika::Foundation::Execution {
    void ThrowTimeOutException (); // forward declare to avoid include/deadly include embrace
}

namespace Stroika::Foundation::Execution {

    /*
     ********************************************************************************
     **************************** Synchronized<T, TRAITS> ***************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    template <typename... ARGUMENT_TYPES>
    inline Synchronized<T, TRAITS>::Synchronized (ARGUMENT_TYPES&&... args)
        : fProtectedValue_ (forward<ARGUMENT_TYPES> (args)...)
    {
#if qStroika_FeatureSupported_Valgrind
        if (TRAITS::kSupportSharedLocks) {
            // This appears to not be supported default valgrind rules/gcc8 (and eariler) libraries/ubuntu1804,
            // though VALGRIND_HG_MUTEX_INIT_POST() is --LGP 2018-06-27
            ANNOTATE_RWLOCK_CREATE (&fMutex_);
        }
        else {
            //VALGRIND_HG_MUTEX_INIT_POST (&fMutex_, TRAITS::kIsRecursiveMutex);
        }
#endif
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::Synchronized (const Synchronized& src)
        : fProtectedValue_ (src.cget ().load ())
    {
#if qStroika_FeatureSupported_Valgrind
        if (TRAITS::kSupportSharedLocks) {
            // This appears to not be supported default valgrind rules/gcc8 (and eariler) libraries/ubuntu1804,
            // though VALGRIND_HG_MUTEX_INIT_POST() is --LGP 2018-06-27
            ANNOTATE_RWLOCK_CREATE (&fMutex_);
        }
        else {
            //VALGRIND_HG_MUTEX_INIT_POST (&fMutex_, TRAITS::kIsRecursiveMutex);
        }
#endif
    }
#if qStroika_FeatureSupported_Valgrind
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::~Synchronized ()
    {
        if (TRAITS::kSupportSharedLocks) {
            // This appears to not be supported default valgrind rules/gcc8 (and eariler) libraries/ubuntu1804,
            // though VALGRIND_HG_MUTEX_INIT_POST() is --LGP 2018-06-27
            ANNOTATE_RWLOCK_DESTROY (&fMutex_);
        }
        else {
            //VALGRIND_HG_MUTEX_DESTROY_PRE (&fMutex_);
        }
    }
#endif
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator= (const Synchronized& rhs) -> Synchronized&
    {
        if (&rhs != this) {
            auto                    value   = rhs.cget ().load (); // load outside the lock to avoid possible deadlock
            [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
            fProtectedValue_                = value;
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator= (const T& rhs) -> Synchronized&
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
        fProtectedValue_                = rhs;
        return *this;
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline Synchronized<T, TRAITS>::operator T () const
    {
        return load ();
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline T Synchronized<T, TRAITS>::load () const
    {
        ReadLockType_ fromCritSec{fMutex_};
        return fProtectedValue_;
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline void Synchronized<T, TRAITS>::store (const T& v)
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
        fProtectedValue_                = v;
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline void Synchronized<T, TRAITS>::store (T&& v)
    {
        [[maybe_unused]] auto&& critSec = lock_guard{fMutex_};
        fProtectedValue_                = move (v);
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::cget () const -> ReadableReference
    {
        return ReadableReference (&fProtectedValue_, &fMutex_);
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::rwget () -> WritableReference
    {
        return WritableReference (&fProtectedValue_, &fMutex_);
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator-> () const -> ReadableReference
    {
        return ReadableReference (&fProtectedValue_, &fMutex_);
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex and TRAITS::kSupportSharedLocks>*>
    inline void Synchronized<T, TRAITS>::lock_shared () const
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::lock_shared", L"&m=%p", &m};
#endif
        fMutex_.lock_shared ();
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex and TRAITS::kSupportSharedLocks>*>
    inline void Synchronized<T, TRAITS>::unlock_shared () const
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::unlock_shared", L"&m=%p", &m};
#endif
        fMutex_.unlock_shared ();
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline void Synchronized<T, TRAITS>::lock () const
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::lock", L"&fMutex_=%p", &fMutex_};
#endif
        fMutex_.lock ();
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsRecursiveMutex>*>
    inline void Synchronized<T, TRAITS>::unlock () const
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::unlock", L"&fMutex_=%p", &fMutex_};
#endif
        fMutex_.unlock ();
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kSupportSharedLocks>*>
    void Synchronized<T, TRAITS>::UpgradeLockNonAtomically ([[maybe_unused]] ReadableReference* lockBeingUpgraded, const function<void (WritableReference&&)>& doWithWriteLock, const chrono::duration<Time::DurationSecondsType>& timeout)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::UpgradeLockNonAtomically", L"&fMutex_=%p, timeout=%s", &fMutex_, Characters::ToString (timeout).c_str ()};
#endif
        RequireNotNull (lockBeingUpgraded);
        Require (lockBeingUpgraded->fSharedLock_.mutex () == &fMutex_);
        Require (lockBeingUpgraded->fSharedLock_.owns_lock ());
        fMutex_.unlock_shared ();
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([this] () {
            fMutex_.lock_shared (); // this API requires (regardless of timeout) that we re-lock (shared)
        });
        if (timeout.count () >= numeric_limits<Time::DurationSecondsType>::max ()) {
            doWithWriteLock (WritableReference (&fProtectedValue_, &fMutex_)); // if wait 'infiniite' use waitless lock call
        }
        else {
            doWithWriteLock (WritableReference (&fProtectedValue_, &fMutex_, timeout));
        }
    }
    template <typename T, typename TRAITS>
    template <typename TEST_TYPE, enable_if_t<TEST_TYPE::kIsUpgradableSharedToExclusive>*>
    void Synchronized<T, TRAITS>::UpgradeLockAtomically ([[maybe_unused]] ReadableReference* lockBeingUpgraded, const function<void (WritableReference&&)>& doWithWriteLock, const chrono::duration<Time::DurationSecondsType>& timeout)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::UpgradeLockAtomically", L"&fMutex_=%p, timeout=%s", &fMutex_, Characters::ToString (timeout).c_str ()};
#endif
        RequireNotNull (lockBeingUpgraded);
        Require (lockBeingUpgraded->fSharedLock_.mutex () == &fMutex_);
        Require (lockBeingUpgraded->fSharedLock_.owns_lock ());

        // @todo CLEANUP - the only upgradeLock we support right now takes BOOST boost::defer_lock ; subclass to map std::defer_lock to boost::defer_lock
        //typename TRAITS::UpgradeLockType upgradeLock{fMutex_, boost::defer_lock};
        boost::unique_lock upgradeLock{fMutex_, boost::defer_lock};
        if (timeout.count () >= numeric_limits<Time::DurationSecondsType>::max ()) {
            upgradeLock.lock (); // if wait 'infiniite' use waitless lock call
        }
        else {
            // @todo CLEANUP - the only upgradeLock we support right now takes BOOST time as arg. Do subclass that converts from std::chrono::duration
            // and then can clean this up...
            if (not upgradeLock.try_lock_for (boost::chrono::duration<Time::DurationSecondsType> (timeout.count ()))) {
                Execution::ThrowTimeOutException ();
            }
        }
        doWithWriteLock (WritableReference (&fProtectedValue_, nullptr));
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
        , fSharedLock_ ((RequireNotNull (m), *m))
    {
        RequireNotNull (t);
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
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (T* t, nullptr_t)
        : ReadableReference (t, nullptr)
    {
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (T* t, MutexType* m)
        : ReadableReference (t, nullptr)
        , fWriteLock_{*m}
    {
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (T* t, MutexType* m, const chrono::duration<Time::DurationSecondsType>& timeout)
        : ReadableReference (t, nullptr)
        , fWriteLock_{*m, timeout}
    {
        if (not fWriteLock_.owns_lock ()) {
            Execution::ThrowTimeOutException ();
        }
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (WritableReference&& src)
        : ReadableReference (move (src))
        , fWriteLock_ (move (src.fWriteLock_))
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
        // preload to avoid possible deadlock
        auto l = lhs.load ();
        auto r = rhs.load ();
        return l < r;
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

#endif /*_Stroika_Foundation_Execution_Synchronized_inl_*/
