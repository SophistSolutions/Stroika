/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_ 1

#include "../StroikaPreComp.h"

#include <compare>
#include <functional>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <type_traits>

#include "../Configuration/Common.h"
#include "../Configuration/Empty.h"
#include "../Configuration/TypeHints.h"

#include "Common.h"
#include "Finally.h"
#include "SpinLock.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Late</a>
 *
 * TODO:
 *      @todo   Deadlock from two threads doing UpgradeLockNonAtomically(quietly) is ??? detectable, so in DEBUG builds translate that to an
 *              assert erorr?
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-613 - Synchronized<>::ReadableReference and WritableReference could be more efficent if not subclassing each other
 *
 *      @todo   https://stroika.atlassian.net/browse/STK-657 - experiment with some sort of shared_recursive_mutex - not sure good idea in general, but maybe a limited form can be used in synchronized
 *
 *      @todo   Tons of cleanups, orthoganality, docs, etc.
 *
 *      @todo   consider/doc choice on explicit operator T ()
 *
 */

namespace Stroika::Foundation::Execution {

    /**
     *  The type InternallySynchronized is intended to be used as a flag to mark whether or not a given class/type/object
     *  is internally synchronized, or not.
     *
     *  It is typically provided as an optional argument to static New () methods, such as 
     *  MemoryStream<>::New ()
     *
     *  \note something marked eNotKnownInternallySynchronized - may in fact be internally synchronized.
     */
    enum class InternallySynchronized {
        eInternallySynchronized,
        eNotKnownInternallySynchronized
    };
    constexpr InternallySynchronized eInternallySynchronized         = InternallySynchronized::eInternallySynchronized;
    constexpr InternallySynchronized eNotKnownInternallySynchronized = InternallySynchronized::eNotKnownInternallySynchronized;

    /**
     *  MUTEX:
     *      We chose to make the default MUTEX recursive_mutex - since most patterns of use will be supported
     *      by this safely.
     *
     *      To use timed-locks, use timed_recursive_mutex.
     *
     *      If recursion is not necessary, and for highest performance, SpinLock will often work best.
     *
     *  \par Example Usage
     *      \code
     *          Synchronized<String> n;                                                 // SAME
     *          Synchronized<String,Synchronized_Traits<>> n;                           // SAME
     *          Synchronized<String,Synchronized_Traits<recursive_mutex>>   n;          // SAME
     *      \endcode
     *
     *  or slightly faster, but possibly slower or less safe (depnding on usage)
     *      Synchronized<String,Synchronized_Traits<SpinLock>>   n;
     *
     *  \note Use of SUPPORTS_SHARED_LOCKS has HIGH PERFORMANCE OVERHEAD, and only makes sense when you have
     *        read locks held for a long time (and multiple threads doing so).
     *
     *        @see http://joeduffyblog.com/2009/02/11/readerwriter-locks-and-their-lack-of-applicability-to-finegrained-synchronization/
     *
     *  \note To change the value of one of the constexpr or type members of Synchronized_Traits, use template
     *        specialization (or pass a completely different traits object to Synchronized<>).
     *
     */
    template <typename MUTEX = recursive_mutex>
    struct Synchronized_Traits {
        using MutexType = MUTEX;

        /**
         *  Used internally for assertions that the synchronized object is used safely. If you mean to use differently, specialize
         */
        static constexpr bool kIsRecursiveReadMutex = is_same_v<MUTEX, recursive_mutex> or is_same_v<MUTEX, recursive_timed_mutex> or
                                                      is_same_v<MUTEX, shared_timed_mutex> or is_same_v<MUTEX, shared_mutex>;

        /**
         *  Used internally for assertions that the synchronized object is used safely. If you mean to use differently, specialize
         */
        static constexpr bool kIsRecursiveLockMutex = is_same_v<MUTEX, recursive_mutex> or is_same_v<MUTEX, recursive_timed_mutex>;

        /**
         *  If enabled, fTraceLocksName field available, and if its set, DbgTrace calls on lock/unlock.
         */
        static constexpr bool kDbgTraceLockUnlockIfNameSet = qDefaultTracingOn;

        /**
         */
        static constexpr bool kSupportsTimedLocks =
            is_same_v<MUTEX, shared_timed_mutex> or is_same_v<MUTEX, recursive_timed_mutex> or is_same_v<MUTEX, timed_mutex>;

        /**
         */
        static constexpr bool kSupportSharedLocks = is_same_v<MUTEX, shared_timed_mutex> or is_same_v<MUTEX, shared_mutex>;

        /**
         */
        using ReadLockType = conditional_t<kSupportSharedLocks, shared_lock<MUTEX>, unique_lock<MUTEX>>;

        /**
         */
        using WriteLockType = unique_lock<MUTEX>;
    };

    namespace Private_ {
        struct DbgTraceNameObj_ {
            // avoid use of Stroika String, due to mutual dependencies - this is low level code
            optional<std::wstring> fDbgTraceLocksName;
        };
    }

    /**
     */
    template <typename MUTEX>
    struct Tracing_Synchronized_Traits : Synchronized_Traits<MUTEX> {
        static constexpr bool kDbgTraceLockUnlockIfNameSet = true;
    };

    /**
     *  \brief  Wrap any object with Synchronized<> and it can be used similarly to the base type,
     *          but safely in a thread safe manner, from multiple threads. This is similar to std::atomic.
     *
     *  The idea behind any of these synchronized classes is that they can be used freely from
     *  different threads without worry of data corruption. It is almost as if each operation were
     *  preceeded with a mutex lock (on that object) and followed by an unlock.
     *
     *  If one thread does a Read operation on Synchronized<T> while another does a write (modification)
     *  operation on Synchronized<T>, the Read will always return a consistent reasonable value, from
     *  before the modification or afterwards, but never a distorted invalid value.
     *
     *  This is very much closely logically the java 'synchronized' attribute, except that its
     *  not a language extension/attribute here, but rather a class wrapper. Its also implemented
     *  in the library, not the programming language.
     *
     *  \note   LIKE JAVA SYNCHRONIZED
     *          This is SIMPLE to use like the Java (and .net) synchronized attribute(lock) mechanism.
     *          But why does it not suffer from the same performance deficiit?
     *
     *          Because with Java - you mixup exceptions and assertions. With Stroika, we have builtin
     *          checking for races (Debug::AssertExternallySynchronizedMutex) in most objects, so
     *          you only use Synchronized<> (or some other more performant mechanism) in the few places
     *          you need it.
     *
     *  \note   Synchronized<> is similar to std::atomic, except that
     *          *   You can use it as a mutex with lock_guard and lock for an extended period of time.
     *          *   This supports read/write locks.
     *          *   This supports locking objects and updated bits of them - not just replacing all at one go
     *
     *  \par Example Usage
     *      \code
     *          Synchronized<String> n;                                                 // SAME
     *          Synchronized<String,Synchronized_Traits<>> n;                           // SAME
     *          Synchronized<String,Synchronized_Traits<recursive_mutex>>       n;      // SAME
     *      \endcode
     *
     *  or slightly faster, but possibly slower or less safe (depending on usage)
     *      \code
     *          Synchronized<String,Synchronized_Traits<SpinLock>>              n;
     *      \endcode
     *
     *  or to allow timed locks
     *      \code
     *          Synchronized<String,Synchronized_Traits<timed_recursive_mutex>> n;
     *      \endcode
     *
     *  or to read-modify-update in place
     *      \code
     *          //nb: lock lasts til end of enclosing scope
     *          auto    lockedConfigData = fConfig_.rwget ();
     *          lockedConfigData->SomeValueChanged = 1;
     *      \endcode
     * 
     *  or to read-modify-update with explicit temporary
     *      \code
     *          //nb: lock lasts til end of enclosing scope
     *          auto    lockedConfigData = fConfig_.rwget ();
     *          T       value = lockedConfigData;
     *          value.SomeValueChanged = 1;
     *          lockedConfigData.store (value);
     *      \endcode
     *
     *  or, because Synchronized<> has lock/unlock methods, it can be used with a lock_guard (if associated mutex recursive), as in:
     *      \code
     *          auto&& critSec = lock_guard{n};
     *          // lock to make sure someone else doesn't change n after we made sure it looked good
     *          if (looks_good (n)) {
     *              String a = n;
     *              a = a + a;
     *              n.store (a);
     *          }
     *      \endcode
     *
     *  \note   We consider supporting operator-> for Synchronized<> - and overloading on const to see if we use a Read Lock or a Write lock.
     *          The problem is - that IF its called through a non-const object, it will select the non-const (write lock) even though all that
     *          was needed was the read lock! So this paradigm - though more terse and clear - just encourages inefficient coding (so we
     *          have no read locks - all locks write locks).
     *
     *          So ONLY support operator-> const overload (brevity and more common than for write). To write - use rwget().
     * 
     *  \note Upgrading a shared_lock to a full lock
     *        We experimented with using boost upgrade_lock code to allow for a full upgrade capability, but this intrinsically
     *        can (easily) yield deadlocks (e.g. thread A holds read lock and tries to upgrade, while thread B holds shared_lock
     *        and waits on something from thread A), and so I decided to abandon this approach.
     * 
     *        Instead, just have upgradeLock release the shared_lock, and re-acquire the mutex as a full lock. BUT - this has problems
     *        too. Typically - you compute something with the shared_lock and notice you want to commit a change, and so upgrade to get
     *        the full lock. But when you do the upgrade, someone else could sneak in and do the same thing invalidating your earlier
     *        computation.
     * 
     *        So - the Upgrade lock APIS have the word "NON_ATOMICALLY" in the name to emphasize this issue, and either return a boolean
     *        indicating failure, or take a callback that gets notified of the need to recompute the cached value/data.
     * 
     *
     *  \note <a href="Design Overview.md#Comparisons">Comparisons</a>:
     *      o   Standard Stroika Comparison support (operator<=>,operator==, etc);
     *          (but these are ONLY defined if TRAITS::kIsRecursiveReadMutex)
     */
    template <typename T, typename TRAITS = Synchronized_Traits<>>
    class Synchronized : public conditional_t<TRAITS::kDbgTraceLockUnlockIfNameSet, Private_::DbgTraceNameObj_, Configuration::Empty> {
    public:
        using element_type = T;

    public:
        using MutexType = typename TRAITS::MutexType;

    public:
        class ReadableReference;

    public:
        class WritableReference;

    public:
        /**
         *  Create a Syncrhonized with any argument type the underlying type will be constructed with the same
         *  (perfectly) forwarded arguments.
         * 
         *  And plain copy constructor.
         * 
         *  FOR NOW, avoid MOVE constructor (cuz synchronized is a combination of original data with lock, and not sure what moving
         *  the lock means).
         */
        template <typename... ARGUMENT_TYPES>
        Synchronized (ARGUMENT_TYPES&&... args);
        Synchronized (const Synchronized& src);

#if qStroika_FeatureSupported_Valgrind
    public:
        ~Synchronized ();
#endif

    public:
        /**
         */
        nonvirtual Synchronized& operator= (const Synchronized& rhs);
        nonvirtual Synchronized& operator= (T&& rhs);
        nonvirtual Synchronized& operator= (const T& rhs);

    public:
        /**
         *  Synonym for load ()
         *
         *  \note   Tentatively (as of v2.0a155) - decided to do with non-explicit. This makes usage more terse
         *          for cases like:
         *              Synchronized<T> sX_;
         *              T Accessor ()  { return sX_; }
         *          and so far has caused no obvious problems.
         *
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (say with RWSynchronized) - use cget ().load ();
         *          The reason this is only defined for recursive mutexes is so that it can be used in a context where this thread
         *          already has a lock (e.g. called rwget ()).
         */
        nonvirtual operator T () const
            requires (TRAITS::kIsRecursiveReadMutex);

    public:
        /**
         *  Note - unlike operator->, load () returns a copy of the internal data, and only locks while fetching it, so that the
         *  lock does not persist while using the result.
         *
         *  \note   Using load () can be most efficient (least lock contention) with read/write locks (@see RWSynchronized),
         *          since it just uses a read lock and releases it immediately.
         *
         *  \par Example Usage
         *      \code
         *          Synchronized<Thread::Ptr> sharedData;
         *          sharedData.load ().AbortAndWaitTilDone ();  // copies Thread::Ptr and doesn't maintain lock during wait
         *          sharedData->AbortAndWaitTilDone ();         // works off internal copy of thread object, and maintains the lock while accessing
         *      \endcode
         *
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (e.g. with RWSynchronized<T>) - use cget ().load (), or existingLock.load ();
         *          The reason this is only defined for recursive mutexes is so that it can be used in a context where this thread
         *          already has a lock (e.g. called rwget ()).
         */
        nonvirtual T load () const
            requires (TRAITS::kIsRecursiveReadMutex);
        nonvirtual T load (const chrono::duration<double>& tryFor) const
            requires (TRAITS::kIsRecursiveReadMutex and TRAITS::kSupportsTimedLocks);

    public:
        /**
         * @see load ()
         *
         *  Save the given value into this synchronized object, acquiring the needed write lock first.
         *
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (say with RWSynchronized) - use rwget ().store ();
         *          The reason this is only defined for recursive mutexes is so that it can be used in a context where this thread
         *          already has a lock (e.g. called rwget ()).
         */
        nonvirtual void store (const T& v)
            requires (TRAITS::kIsRecursiveLockMutex);
        nonvirtual void store (T&& v)
            requires (TRAITS::kIsRecursiveLockMutex);
        nonvirtual void store (const T& v, const chrono::duration<double>& tryFor)
            requires (TRAITS::kIsRecursiveLockMutex and TRAITS::kSupportsTimedLocks);
        nonvirtual void store (T&& v, const chrono::duration<double>& tryFor)
            requires (TRAITS::kIsRecursiveLockMutex and TRAITS::kSupportsTimedLocks);

    public:
        /**
         *  \brief  get a read-only smart pointer to the underlying Synchronized<> object, holding the readlock the whole
         *          time the return (often temporary) ReadableReference exists.
         *
         *  \note   this supports multiple readers/single writer, iff the mutex used with Synchronized<> supports it (@see Synchronized)
         *
         *  \par Example Usage
         *      \code
         *          auto    lockedConfigData = fConfig_.cget ();
         *          fCurrentCell_ = lockedConfigData->fCell.Value (Cell::Short);
         *          fCurrentPressure_ = lockedConfigData->fPressure.Value (Pressure::Low);
         *      \endcode
         *
         *  This is roughly equivilent (if using a recursive mutex) to (COUNTER_EXAMPLE):
         *      \code
         *          lock_guard<Synchronized<T,TRAITS>>  critSec (fConfig_);
         *          fCurrentCell_ = fConfig_->fCell.Value (Cell::Short);
         *          fCurrentPressure_ = fConfig_->fPressure.Value (Pressure::Low);
         *      \endcode
         *
         *  Except that this works whether using a shared_mutex or regular mutex. Also - this provides only read-only access
         *  (use rwget for read-write access).
         *
         *  \note - this creates a lock, so be sure TRAITS::kIsRecursiveReadMutex if using this in a place where the same thread may have a lock.
         */
        nonvirtual ReadableReference cget () const;
        nonvirtual ReadableReference cget (const chrono::duration<double>& tryFor) const
            requires (TRAITS::kSupportsTimedLocks);

    public:
        /**
         *  \brief  get a read-rwite smart pointer to the underlying Synchronized<> object, holding the full lock the whole
         *          time the (often temporary) WritableReference exists.
         *
         *  \note - this creates a lock, so be sure TRAITS::kIsRecursiveLockMutex if using this in a place where the same thread may have a lock.
         */
        nonvirtual WritableReference rwget ();
        nonvirtual WritableReference rwget (const chrono::duration<double>& tryFor)
            requires (TRAITS::kSupportsTimedLocks);

    public:
        /*
         *  \brief  alias for cget ()
         *
         *  \note   Considered deprecating, due to confusion between the const and non-const overload of operator->. That was confusing
         *          because the overload was not based on need, but based on the constness of the underlying object.
         *
         *          But just having operator-> always return a const const ReadableReference is just convenient. Don't use it if
         *          you don't like it, but it adds to clarity.
         *
         *          And for the more rare 'write locks' - you are forced to use rwget ().
         *
         *  @see load ()
         */
        nonvirtual ReadableReference operator->() const;

    public:
        /**
         *  \note   lock_shared () only works for 'recursive' mutexes which supported 'shared lock'. To avoid the absence of this
         *          feature (say with RWSynchronized) - use rwget () or cget ();
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveReadMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex and TRAITS::kSupportSharedLocks>* = nullptr>
        nonvirtual void lock_shared () const;

    public:
        /**
         *  \note   unlock_shared () only works for 'recursive' mutexes which supported 'shared lock'. To avoid the absence of this
         *          feature (say with RWSynchronized) - use rwget () or cget ();
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveReadMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex and TRAITS::kSupportSharedLocks>* = nullptr>
        nonvirtual void unlock_shared () const;

    public:
        /**
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (e.g. with RWSynchronized<T>) - use cget (), or rwget ().
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveLockMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveLockMutex>* = nullptr>
        nonvirtual void lock () const;

    public:
        /**
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (e.g. with RWSynchronized<T>) - use cget (), or rwget ().
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveLockMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveLockMutex>* = nullptr>
        nonvirtual bool try_lock () const;

    public:
        /**
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (e.g. with RWSynchronized<T>) - use cget (), or rwget ().
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveLockMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveLockMutex and TEST_TYPE::kSupportsTimedLocks>* = nullptr>
        nonvirtual bool try_lock_for (const chrono::duration<double>& tryFor) const;

    public:
        /**
         *  \note   This works only for 'recursive' mutexes (the default, except for RWSynchronized). To avoid the absence of this
         *          feature (e.g. with RWSynchronized<T>) - use cget (), or rwget ().
         *
         *  \note - This is only usable with TRAITS::kIsRecursiveLockMutex, because there would be no way to access the underlying value
         *          otherwise.
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveLockMutex>* = nullptr>
        nonvirtual void unlock () const;

    public:
        /**
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex>* = nullptr>
        nonvirtual bool operator== (const Synchronized& rhs) const;
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex>* = nullptr>
        nonvirtual bool operator== (const T& rhs) const;

    public:
        /**
         */
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex>* = nullptr>
        nonvirtual auto operator<=> (const Synchronized& rhs) const;
        template <typename TEST_TYPE = TRAITS, enable_if_t<TEST_TYPE::kIsRecursiveReadMutex>* = nullptr>
        nonvirtual auto operator<=> (const T& rhs) const;

    public:
        /**
         *  \brief Upgrade a shared_lock (ReadableReference) to a (WritableReference) full lock temporarily in the context of argument function; return true if succeeds, and false if fails (timeout trying to full-lock) or argument doWithWriteLock () returns false
         * 
         *  @see UpgradeLockNonAtomically - to just calls UpgradeLockNonAtomicallyQuietly () and throws timeout on timeout inteveningWriteLock or doWithWriteLock returns false
         *
         *  A DEFEFCT with (this) UpgradeLockNonAtomically API, is that you cannot count on values computed with the read lock to remain
         *  valid in the upgrade lock (since we unlock and then re-lock). We resolve this by having two versions of UpgradeLockNonAtomically,
         *  one where the callback gets notified there was an interening writelock, and one where the entire call fails and you have to
         *  re-run.
         *
         *  \note optional 'bool interveningWriteLock' parameter - if present, intevening locks are flagged with this paraemter, and if 
         *        the parameter is NOT present, intevening locks are treated as timeouts (even if infinite timeout specified)
         *
         *  \note - also returns false on intevening lock IFF doWithWriteLock/1 passed in has no inteveningWriteLock parameter.
         *
         *  \note optional 'bool interveningWriteLock' parameter - if present, intevening locks are flagged with this paraemter, and if 
         *        the parameter is NOT present, intevening locks are treated as timeouts (even if infinite timeout specified)
         *
         *  \note - This does NOT require the mutex be recursive  - just supporting both lock_shared and lock ()
         * 
         *  \note - This function takes as argument an existing ReadableReference, which MUST come from a cget on this Syncrhonized object
         *          (and therefore must be locked) and DURING the context of this function call that becomes invalid, but when this call returns
         *          it will still be locked READONLY. This does NOT change the lock to writable (after the call) - but ONLY during the call
         *          of the argument function.
         * 
         *  \par Example Usage
         *      \code
         *          Execution::RWSynchronized<Status_> fStatus_;
         *          ...
         *          again:
         *          auto lockedStatus = fStatus_.cget ();
         *          // do a bunch of code that only needs read access
         *          if (some rare event) {
         *              if (not fStatus_.UpgradeLockNonAtomicallyQuietly (&lockedStatus, [=](auto&& writeLock) {
         *                      writeLock.rwref ().fCompletedScans.Add (scan);
         *                  }
         *              )) {
         *                  Execution::Sleep (1s);
         *                  goto again;   // important to goto before we acquire readlock to avoid deadlock when multiple threads do this
         *              }
         *          }
         *      \endcode
         */
        nonvirtual bool UpgradeLockNonAtomicallyQuietly (
            ReadableReference* lockBeingUpgraded, const function<void (WritableReference&&)>& doWithWriteLock,
            const chrono::duration<Time::DurationSecondsType>& timeout = chrono::duration<Time::DurationSecondsType>{Time::kInfinite})
            requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks);
        nonvirtual bool UpgradeLockNonAtomicallyQuietly (
            ReadableReference* lockBeingUpgraded, const function<bool (WritableReference&&, bool interveningWriteLock)>& doWithWriteLock,
            const chrono::duration<Time::DurationSecondsType>& timeout = chrono::duration<Time::DurationSecondsType>{Time::kInfinite})
            requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks);
        ;

    public:
        /**
         *  \brief Same as UpgradeLockNonAtomicallyQuietly, but throws on failure (either timeout or if argument function returns false)
         * 
         *  \note - the 'ReadableReference' must be shared_locked coming in, and will be identically shared_locked on return.
         *
         *  \note - throws on timeout OR if interveningWriteLock and doWithWriteLock/1 passed, or if doWithWriteLock returns false
         *
         *  \note - This does NOT require the mutex be recursive  - just supporting both lock_shared and lock ()
         *
         *  \note - the timeout refers ONLY the acquiring the upgrade - not the time it takes to re-acquire the shared lock or perform
         *          the argument operation
         *
         *  \par Example Usage
         *      \code
         *          Execution::RWSynchronized<Status_> fStatus_;
         *          auto lockedStatus = fStatus_.cget ();
         *          // do a bunch of code that only needs read access
         *          if (some rare event) {
         *              // This COULD fail/throw only if called from intervening lock
         *              fStatus_.UpgradeLockNonAtomically ([=](auto&& writeLock) {
         *                  writeLock.rwref ().fCompletedScans.Add (scan);
         *              });
         *          }
         *      \endcode
         */
        nonvirtual void
        UpgradeLockNonAtomically (ReadableReference* lockBeingUpgraded, const function<void (WritableReference&&)>& doWithWriteLock,
                                  const chrono::duration<Time::DurationSecondsType>& timeout = chrono::duration<Time::DurationSecondsType>{Time::kInfinite})
            requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks);
        nonvirtual void UpgradeLockNonAtomically (
            ReadableReference* lockBeingUpgraded, const function<bool (WritableReference&&, bool interveningWriteLock)>& doWithWriteLock,
            const chrono::duration<Time::DurationSecondsType>& timeout = chrono::duration<Time::DurationSecondsType>{Time::kInfinite})
            requires (TRAITS::kSupportSharedLocks and TRAITS::kSupportsTimedLocks);
        ;

    private:
        nonvirtual void NoteLockStateChanged_ (const wchar_t* m) const noexcept;

    private:
        using ReadLockType_ = typename TRAITS::ReadLockType;

    private:
        using WriteLockType_ = typename TRAITS::WriteLockType;

    private:
        T                 fProtectedValue_;
        mutable MutexType fMutex_;
        /*
         *  note: we don't need to use atomic/or extra lock for updating this because
         *  its ONLY INCREMENETED and EXAMINED when fMutex_ is owned.
         */
        mutable unsigned int fWriteLockCount_{0};
    };

    /**
     *  Anything that constructs a ReadableReference - for example - Synchronized<T, TRAITS>::cget () - is suitable for multiple readers at the same time,
     *  so long as using Synchronized<> of a type that supports shared locks (@see RWSynchronized<>)
     */
    template <typename T, typename TRAITS>
    class Synchronized<T, TRAITS>::ReadableReference
        : public conditional_t<TRAITS::kDbgTraceLockUnlockIfNameSet, Private_::DbgTraceNameObj_, Configuration::Empty> {
    protected:
        /**
         *  If specified, either subclass, or external lock used for lifetime of this object. So this class
         *  just provides access, but doesn't actually do a lock.
         */
        enum class _ExternallyLocked {
            _eExternallyLocked
        };

    protected:
        /**
         *  Can construct ReadableReference with nullptr_t mutex, in which case its the subclasses responsability to manage locking
         *
         *  \req t != nullptr, and this class holds onto that pointer.
         */
        ReadableReference (const Synchronized* s);
        ReadableReference (const Synchronized* s, _ExternallyLocked);
        ReadableReference (const Synchronized* s, ReadLockType_&& readLock);

    public:
        ReadableReference (const ReadableReference& src) = delete; // must move because both src and dest cannot have the unique lock

    public:
        ReadableReference (ReadableReference&& src);

    public:
        nonvirtual const ReadableReference& operator= (const ReadableReference& rhs) = delete;

    public:
        ~ReadableReference ();

    public:
        /**
         *  \note   Considerd losing operator-> here as possibly confusing (e.g. when mixed with Synchronized<optional<xxx>>>).
         *          But you don't need to use it, and this really does act as a smart pointer so it should most often just be
         *          more clear.
         */
        nonvirtual const T* operator->() const;

    public:
        /**
         */
        nonvirtual const T& cref () const;

    public:
        /**
         *  \brief  alias for cref () - but allows often simpler short-hand. Use explicit .cref () if you run into ambiguities.
         *
         *  \note   OK to return const reference here because we own a lock anyhow
         */
        nonvirtual operator const T& () const;

    public:
        /**
         *  \brief  more or less identical to cref () - except return value is value, not reference.
         */
        nonvirtual T load () const;

    protected:
        const T* fT;

    protected:
        nonvirtual void _NoteLockStateChanged (const wchar_t* m) const;

    private:
        ReadLockType_ fSharedLock_{}; // MAYBE unused if actaul readlock held elsewhere, like in subclass

    private:
        friend class Synchronized;
    };

    /**
     */
    template <typename T, typename TRAITS>
    class Synchronized<T, TRAITS>::WritableReference : public Synchronized<T, TRAITS>::ReadableReference {
    private:
        using inherited = typename Synchronized<T, TRAITS>::ReadableReference;

    protected:
        /**
         */
        WritableReference (Synchronized* s);
        WritableReference (Synchronized* s, WriteLockType_&& writeLock);
        WritableReference (Synchronized* s, const chrono::duration<Time::DurationSecondsType>& timeout);
        WritableReference (const WritableReference& src) = delete; // must move because both src and dest cannot have the unique lock

    public:
        WritableReference (WritableReference&& src);
        nonvirtual WritableReference&       operator= (const WritableReference& rhs) = delete;
        nonvirtual const WritableReference& operator= (T rhs);

    public:
        /**
         *  \note   Considerd losing operator-> here as possibly confusing (e.g. when mixed with Synchronized<optional<xxx>>>).
         *          But you don't need to use it, and this really does act as a smart pointer so it should most often just be
         *          more clear.
         */
        nonvirtual T*       operator->();
        nonvirtual const T* operator->() const;

    public:
        /**
         *  Get readable and writable reference to the underlying object.
         *
         *  \note   We experimented with overloading just ref() with const and non const versions, but the trouble with this
         *          is that from the caller, its not obvious which version you are getting, and it often matters alot, so
         *          this appeared - though less convenient - less confusing.
         *
         *          We can always add (back) a ref () - so overloaded - method.
         */
        nonvirtual T& rwref ();

    public:
        /**
         */
        nonvirtual void store (const T& v);
        nonvirtual void store (T&& v);

    private:
        WriteLockType_ fWriteLock_; // can be empty if actual lock held elsewhere (like in caller/subclass)

    private:
        friend class Synchronized;
    };

    /**
     */
    template <typename T, typename TRAITS>
    auto operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} ^ T{});
    template <typename T, typename TRAITS>
    auto operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{});
    template <typename T, typename TRAITS>
    auto operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} ^ T{});

    /**
     */
    template <typename T, typename TRAITS>
    auto operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} * T{});
    template <typename T, typename TRAITS>
    auto operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{});
    template <typename T, typename TRAITS>
    auto operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} * T{});

    /**
     */
    template <typename T, typename TRAITS>
    auto operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} + T{});
    template <typename T, typename TRAITS>
    auto operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{});
    template <typename T, typename TRAITS>
    auto operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} + T{});

    /**
     */
    template <typename T, typename TRAITS, typename RHSTYPE>
    auto operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator-= (rhs));

    /**
     */
    template <typename T, typename TRAITS, typename RHSTYPE>
    auto operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs.rwget ()->operator+= (rhs));

    /**
     */
    template <typename T, typename TRAITS>
    auto operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T{} - T{});
    template <typename T, typename TRAITS>
    auto operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{});
    template <typename T, typename TRAITS>
    auto operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T{} - T{});

    /**
     * QuickSynchronized will always use a mutex which is quick, and not a cancelation point. It will typically be
     * implemented using a std::mutex, or a SpinLock, whichever is faster. So - don't use this where you hold
     * onto the lock for extended periods ;-).
     */
    template <typename T>
    using QuickSynchronized =
        conditional_t<kSpinLock_IsFasterThan_mutex, Synchronized<T, Synchronized_Traits<SpinLock>>, Synchronized<T, Synchronized_Traits<mutex>>>;

    /**
     */
    template <typename T>
    using TimedSynchronized = Synchronized<T, Synchronized_Traits<recursive_timed_mutex>>;

    /**
     * RWSynchronized will always use some sort of mutex which supports multiple readers, and a single writer.
     * Typically, using shared_mutex (or shared_timed_mutex).
     *
     *  \note Use of RWSynchronized has HIGH PERFORMANCE OVERHEAD, and only makes sense when you have
     *        read locks held for a long time (and multiple threads doing so)
     */
    template <typename T>
    using RWSynchronized = Synchronized<T, Synchronized_Traits<shared_timed_mutex>>;

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Synchronized.inl"

#endif /*_Stroika_Foundation_Execution_Synchronized_h_*/
