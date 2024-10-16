/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

// Comment this in to turn on aggressive noisy DbgTrace in this module (note the extra long name since its in a header)
//#define Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_ 1

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Trace.h"

namespace Stroika::Foundation::Execution {
    void ThrowTimeOutException (); // forward declare to avoid include/deadly include embrace

    namespace Private_ {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
        void DbgTraceHelper_ (const wchar_t* w1, const optional<std::wstring>& w2);
#endif
    }
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
        : fProtectedValue_ (forward<ARGUMENT_TYPES> (args)...) // use () not {} so works with T with explicit CTOR (empirically has trouble - not sure why)
    {
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::Synchronized (const Synchronized& src)
        : fProtectedValue_{src.cget ().load ()}
    {
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator= (const Synchronized& rhs) -> Synchronized&
    {
        if (&rhs != this) [[likely]] {
            auto                        value = rhs.cget ().load (); // load outside the lock to avoid possible deadlock
            [[maybe_unused]] lock_guard critSec{fMutex_};
            [[maybe_unused]] auto&&     cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
            NoteLockStateChanged_ (L"Locked");
            ++fWriteLockCount_;
            fProtectedValue_ = value;
        }
        return *this;
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator= (T&& rhs) -> Synchronized&
    {
        [[maybe_unused]] lock_guard critSec{fMutex_};
        [[maybe_unused]] auto&&     cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        fProtectedValue_ = move (rhs);
        ++fWriteLockCount_;
        return *this;
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator= (const T& rhs) -> Synchronized&
    {
        [[maybe_unused]] lock_guard critSec{fMutex_};
        [[maybe_unused]] auto&&     cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        fProtectedValue_ = rhs;
        ++fWriteLockCount_;
        return *this;
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::operator T () const
        requires (TRAITS::kIsRecursiveReadMutex)
    {
        return load ();
    }
    template <typename T, typename TRAITS>
    inline T Synchronized<T, TRAITS>::load () const
        requires (TRAITS::kIsRecursiveReadMutex)
    {
        ReadLockType_ fromCritSec{fMutex_};
        return fProtectedValue_;
    }
    template <typename T, typename TRAITS>
    inline T Synchronized<T, TRAITS>::load (const chrono::duration<double>& tryFor) const
        requires (TRAITS::kIsRecursiveReadMutex and TRAITS::kSupportsTimedLocks)
    {
        ReadLockType_ critSec{fMutex_, tryFor};
        if (not critSec) [[unlikely]] {
            ThrowTimeOutException ();
        }
        return fProtectedValue_;
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::store (const T& v)
        requires (TRAITS::kIsRecursiveLockMutex)
    {
        [[maybe_unused]] lock_guard critSec{fMutex_};
        [[maybe_unused]] auto&&     cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        ++fWriteLockCount_;
        fProtectedValue_ = v;
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::store (T&& v)
        requires (TRAITS::kIsRecursiveLockMutex)
    {
        [[maybe_unused]] lock_guard critSec{fMutex_};
        [[maybe_unused]] auto&&     cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        ++fWriteLockCount_;
        fProtectedValue_ = std::move (v);
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::store (const T& v, const chrono::duration<double>& tryFor)
        requires (TRAITS::kIsRecursiveLockMutex and TRAITS::kSupportsTimedLocks)
    {
        [[maybe_unused]] unique_lock critSec{fMutex_, tryFor};
        if (not critSec) [[unlikely]] {
            ThrowTimeOutException ();
        }
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        ++fWriteLockCount_;
        fProtectedValue_ = v;
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::store (T&& v, const chrono::duration<double>& tryFor)
        requires (TRAITS::kIsRecursiveLockMutex and TRAITS::kSupportsTimedLocks)
    {
        [[maybe_unused]] unique_lock critSec{fMutex_, tryFor};
        if (not critSec) [[unlikely]] {
            ThrowTimeOutException ();
        }
        [[maybe_unused]] auto&& cleanup = Execution::Finally ([this] () noexcept { NoteLockStateChanged_ (L"Unlocked"); });
        NoteLockStateChanged_ (L"Locked");
        ++fWriteLockCount_;
        fProtectedValue_ = std::move (v);
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::cget () const -> ReadableReference
    {
        return ReadableReference{this};
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::cget (const chrono::duration<double>& tryFor) const -> ReadableReference
        requires (TRAITS::kSupportsTimedLocks)
    {
        ReadLockType_ critSec{fMutex_, tryFor};
        if (not critSec) [[unlikely]] {
            ThrowTimeOutException ();
        }
        return ReadableReference{this, move (critSec)};
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::rwget () -> WritableReference
    {
        return WritableReference{this};
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::rwget (const chrono::duration<double>& tryFor) -> WritableReference
        requires (TRAITS::kSupportsTimedLocks)
    {
        [[maybe_unused]] unique_lock critSec{fMutex_, tryFor};
        if (not critSec) [[unlikely]] {
            ThrowTimeOutException ();
        }
        return WritableReference{this, move (critSec)};
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator->() const -> ReadableReference
    {
        return ReadableReference{this};
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::lock_shared () const
        requires (TRAITS::kIsRecursiveReadMutex and TRAITS::kSupportSharedLocks)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::lock_shared", L"&m=%p", &m};
#endif
        fMutex_.lock_shared ();
        NoteLockStateChanged_ (L"Locked Shared");
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::unlock_shared () const
        requires (TRAITS::kIsRecursiveReadMutex and TRAITS::kSupportSharedLocks)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized_Traits<MUTEX>::unlock_shared", L"&m=%p", &m};
#endif
        fMutex_.unlock_shared ();
        NoteLockStateChanged_ (L"Unlocked Shared");
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::lock () const
        requires (TRAITS::kIsRecursiveReadMutex)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::lock", L"&fMutex_=%p", &fMutex_};
#endif
        fMutex_.lock ();
        NoteLockStateChanged_ (L"Locked");
        ++fWriteLockCount_;
    }
    template <typename T, typename TRAITS>
    inline bool Synchronized<T, TRAITS>::try_lock () const
        requires (TRAITS::kIsRecursiveReadMutex)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::try_lock", L"&fMutex_=%p", &fMutex_};
#endif
        bool result = fMutex_.try_lock ();
        if (result) [[likely]] {
            NoteLockStateChanged_ (L"Locked");
            ++fWriteLockCount_;
        }
        return result;
    }
    template <typename T, typename TRAITS>
    inline bool Synchronized<T, TRAITS>::try_lock_for (const chrono::duration<double>& tryFor) const
        requires (TRAITS::kIsRecursiveReadMutex and TRAITS::kSupportsTimedLocks)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::try_lock_for", L"&fMutex_=%p", &fMutex_};
#endif
        bool result = fMutex_.try_lock_for (tryFor);
        if (result) [[likely]] {
            NoteLockStateChanged_ (L"Locked");
            ++fWriteLockCount_;
        }
        return result;
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::unlock () const
        requires (TRAITS::kIsRecursiveReadMutex)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::unlock", L"&fMutex_=%p", &fMutex_};
#endif
        NoteLockStateChanged_ (L"Unlocked");
        fMutex_.unlock ();
    }
    template <typename T, typename TRAITS>
    inline bool Synchronized<T, TRAITS>::operator== (const Synchronized& rhs) const
        requires (TRAITS::kIsRecursiveReadMutex and equality_comparable<T>)
    {
        return load () == rhs.load ();
    }
    template <typename T, typename TRAITS>
    inline bool Synchronized<T, TRAITS>::operator== (const T& rhs) const
        requires (TRAITS::kIsRecursiveReadMutex and equality_comparable<T>)
    {
        return load () == rhs;
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator<=> (const Synchronized& rhs) const
        requires (TRAITS::kIsRecursiveReadMutex and three_way_comparable<T>)
    {
        return load () <=> rhs.load ();
    }
    template <typename T, typename TRAITS>
    inline auto Synchronized<T, TRAITS>::operator<=> (const T& rhs) const
        requires (TRAITS::kIsRecursiveReadMutex and three_way_comparable<T>)
    {
        return load () <=> rhs;
    }
    template <typename T, typename TRAITS>
    inline bool Synchronized<T, TRAITS>::UpgradeLockNonAtomicallyQuietly ([[maybe_unused]] ReadableReference*         lockBeingUpgraded,
                                                                          const function<void (WritableReference&&)>& doWithWriteLock,
                                                                          Time::DurationSeconds                       timeout)
        requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks)
    {
        return UpgradeLockNonAtomicallyQuietly (
            lockBeingUpgraded,
            [&] (WritableReference&& wRef, [[maybe_unused]] bool interveningWriteLock) {
                if (interveningWriteLock) [[unlikely]] {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
                    DbgTrace ("in UpgradeLockNonAtomicallyQuietly - turning interveningWriteLock into fake timeout");
#endif
                    return false;
                }
                else {
                    doWithWriteLock (std::move (wRef));
                    return true;
                }
            },
            timeout);
    }
    template <typename T, typename TRAITS>
    bool Synchronized<T, TRAITS>::UpgradeLockNonAtomicallyQuietly ([[maybe_unused]] ReadableReference* lockBeingUpgraded,
                                                                   const function<bool (WritableReference&&, bool interveningWriteLock)>& doWithWriteLock,
                                                                   Time::DurationSeconds timeout)
        requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks)
    {
        using Time::DurationSeconds;
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::UpgradeLockNonAtomically", L"&fMutex_=%p, timeout=%s", &fMutex_,
                                      Characters::ToString (timeout).c_str ()};
#endif
        RequireNotNull (lockBeingUpgraded);
        Require (lockBeingUpgraded->fSharedLock_.mutex () == &fMutex_);
        Require (lockBeingUpgraded->fSharedLock_.owns_lock ());
        auto writeLockCountBeforeReleasingReadLock = fWriteLockCount_;
        fMutex_.unlock_shared ();
        [[maybe_unused]] auto&&        cleanup = Execution::Finally ([this] () noexcept {
            fMutex_.lock_shared (); // this API requires (regardless of timeout) that we re-lock (shared)
            NoteLockStateChanged_ (L"in Synchronized<T, TRAITS>::UpgradeLockNonAtomicallyQuietly finally relocked shared");
        });
        typename TRAITS::WriteLockType upgradeLock{fMutex_, std::defer_lock}; // NOTE ONLY held til the end of this and doWithWriteLock
        if (timeout >= Time::DurationSeconds::max ()) {
            upgradeLock.lock (); // if wait 'infinite' use no-time-arg lock call
        }
        else {
            if (not upgradeLock.try_lock_for (timeout)) {
                return false;
            }
        }
        NoteLockStateChanged_ (L"in Synchronized<T, TRAITS>::UpgradeLockNonAtomicallyQuietly acquired Lock"); // no need to message on unlock cuz lock transfered to WritableReference that messages on unlock
        WritableReference wr                   = WritableReference{this, std::move (upgradeLock)};
        bool              interveningWriteLock = fWriteLockCount_ > 1 + writeLockCountBeforeReleasingReadLock;
        // pass 'interveningWriteLock' flag to callback so IT can decide how to handle - ignore or throw
        return doWithWriteLock (std::move (wr), interveningWriteLock);
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::UpgradeLockNonAtomically ([[maybe_unused]] ReadableReference*         lockBeingUpgraded,
                                                                   const function<void (WritableReference&&)>& doWithWriteLock,
                                                                   Time::DurationSeconds                       timeout)
        requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::UpgradeLockNonAtomically", L"&fMutex_=%p, timeout=%s", &fMutex_,
                                      Characters::ToString (timeout).c_str ()};
#endif
        if (not UpgradeLockNonAtomicallyQuietly (lockBeingUpgraded, doWithWriteLock, timeout)) [[unlikely]] {
            Execution::ThrowTimeOutException (); // @todo a bit of a defect, could be returned false not due to timeout, but do to doWithWriteLock returning false...
        }
    }
    template <typename T, typename TRAITS>
    void Synchronized<T, TRAITS>::UpgradeLockNonAtomically ([[maybe_unused]] ReadableReference* lockBeingUpgraded,
                                                            const function<bool (WritableReference&&, bool interveningWriteLock)>& doWithWriteLock,
                                                            Time::DurationSeconds timeout)
        requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks)
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        Debug::TraceContextBumper ctx{L"Synchronized<T, TRAITS>::UpgradeLockNonAtomically", L"&fMutex_=%p, timeout=%s", &fMutex_,
                                      Characters::ToString (timeout).c_str ()};
#endif
        if (not UpgradeLockNonAtomicallyQuietly (lockBeingUpgraded, doWithWriteLock, timeout)) [[unlikely]] {
            Execution::ThrowTimeOutException (); // @todo a bit of a defect, could be returned false not due to timeout, but do to doWithWriteLock returning false...
        }
    }
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::NoteLockStateChanged_ ([[maybe_unused]] const wchar_t* m) const noexcept
    {
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            if (this->fDbgTraceLocksName) {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
                Private_::DbgTraceHelper_ (m, this->fDbgTraceLocksName);
#endif
            }
        }
    }

    /*
     ********************************************************************************
     ***************** Synchronized<T, TRAITS>::ReadableReference *******************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const Synchronized* s, _ExternallyLocked)
        : fT{(RequireExpression (s != nullptr), &s->fProtectedValue_)}
    {
        RequireNotNull (s);
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"ReadableReference::CTOR -- locks (_eExternallyLocked)");
#endif
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            this->fDbgTraceLocksName = s->fDbgTraceLocksName;
        }
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const Synchronized* s, ReadLockType_&& readLock)
        : fT{(RequireExpression (s != nullptr), &s->fProtectedValue_)}
        , fSharedLock_{move (readLock)}
    {
        RequireNotNull (s);
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"ReadableReference::CTOR -- locks (readLock)");
#endif
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            this->fDbgTraceLocksName = s->fDbgTraceLocksName;
        }
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (const Synchronized* s)
        : fT{(RequireExpression (s != nullptr), &s->fProtectedValue_)}
        , fSharedLock_{(RequireExpression (s != nullptr), s->fMutex_)}
    {
        RequireNotNull (fT);
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"ReadableReference::CTOR -- locks (fSharedLock_ with mutex_=%p)", &s->fMutex_);
#endif
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            this->fDbgTraceLocksName = s->fDbgTraceLocksName;
        }
        _NoteLockStateChanged (L"ReadableReference Locked");
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::ReadableReference::ReadableReference (ReadableReference&& src)
        : fT{src.fT} // its a pointer so move same as copy
        , fSharedLock_{std::move (src.fSharedLock_)}
    {
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            this->fDbgTraceLocksName = move (src.fDbgTraceLocksName);
        }
        _NoteLockStateChanged (L"ReadableReference move-Locked");
#if qDebug
        src.fT = nullptr;
#endif
        //src.fSharedLock_ = nullptr;
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::ReadableReference::~ReadableReference ()
    {
#if Stroika_Foundation_Execution_Synchronized_USE_NOISY_TRACE_IN_THIS_MODULE_
        DbgTrace (L"ReadableReference::DTOR -- locks (fSharedLock_ mutex_=%p)", fSharedLock_);
#endif
        if (fSharedLock_.owns_lock ()) {
            _NoteLockStateChanged (L"ReadableReference Unlocked");
        }
    }
    template <typename T, typename TRAITS>
    inline const T* Synchronized<T, TRAITS>::ReadableReference::operator->() const
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
    template <typename T, typename TRAITS>
    inline void Synchronized<T, TRAITS>::ReadableReference::_NoteLockStateChanged ([[maybe_unused]] const wchar_t* m) const
    {
        if constexpr (TRAITS::kDbgTraceLockUnlockIfNameSet) {
            if (this->fDbgTraceLocksName) {
#if qStroika_Foundation_Debug_Trace_DefaultTracingOn
                Private_::DbgTraceHelper_ (m, this->fDbgTraceLocksName);
#endif
            }
        }
    }

    /*
     ********************************************************************************
     *************** Synchronized<T, TRAITS>::WritableReference *********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (Synchronized* s)
        : ReadableReference{s, ReadableReference::_ExternallyLocked::_eExternallyLocked}
        , fWriteLock_{s->fMutex_}
    {
        RequireNotNull (s);
        this->_NoteLockStateChanged (L"WritableReference Locked");
        ++s->fWriteLockCount_;
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (Synchronized* s, WriteLockType_&& writeLock)
        : ReadableReference{s, ReadableReference::_ExternallyLocked::_eExternallyLocked}
        , fWriteLock_{std::move (writeLock)}
    {
        RequireNotNull (s);
        this->_NoteLockStateChanged (L"WritableReference move-Locked");
        ++s->fWriteLockCount_; // update lock count cuz though not a new lock, new to WritableReference
                               // and just used outside construct of WritableRefernce to control how lock acquired
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (Synchronized* s, Time::DurationSeconds timeout)
        : ReadableReference{s, ReadableReference::_ExternallyLocked::_eExternallyLocked}
        , fWriteLock_{s->fMutex_, timeout}
    {
        RequireNotNull (s);
        if (not fWriteLock_.owns_lock ()) [[unlikely]] {
            Execution::ThrowTimeOutException ();
        }
        this->_NoteLockStateChanged (L"WritableReference Locked");
        ++s->fWriteLockCount_;
    }
    template <typename T, typename TRAITS>
    inline Synchronized<T, TRAITS>::WritableReference::WritableReference (WritableReference&& src)
        : ReadableReference{std::move (src)}
        , fWriteLock_{std::move (src.fWriteLock_)}
    {
        // no change to writelockcount cuz not a new lock - just moved
        this->_NoteLockStateChanged (L"WritableReference move-Locked");
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
    inline T* Synchronized<T, TRAITS>::WritableReference::operator->()
    {
        // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
        EnsureNotNull (this->fT);
        return const_cast<T*> (this->fT);
    }
    template <typename T, typename TRAITS>
    inline const T* Synchronized<T, TRAITS>::WritableReference::operator->() const
    {
        return ReadableReference::operator->();
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
        rwref () = std::move (v);
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
