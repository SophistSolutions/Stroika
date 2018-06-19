/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_ 1

#include "../StroikaPreComp.h"

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <type_traits>

#include "../Configuration/Common.h"
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
 *      @todo   https://stroika.atlassian.net/browse/STK-613 - Synchronized<>::ReadableReference and WriteableReference could be more efficent if not subclassing each other
 *
 *      @todo   More operator<, and other operator overloads
 *
 *      @todo   Tons of cleanups, orthoganiality, docs, etc.
 *
 *      @todo   consider/doc choice on explicit operator T ()
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Execution {

            /**
             *  The type InternallySyncrhonized is intended to be used as a flag to mark whether or not a given class/type/object
             *  is internally syncrhonized, or not.
             *
             *  It is typically provided as an optional argument to static New () methods, such as 
             *  MemoryStream<>::New ()
             *
             *  \note soemthing marked eNotKnownInternallySynchronized - may in fact be internally synchronized.
             */
            enum class InternallySyncrhonized {
                eInternallySynchronized,
                eNotKnownInternallySynchronized
            };
            constexpr InternallySyncrhonized eInternallySynchronized         = InternallySyncrhonized::eInternallySynchronized;
            constexpr InternallySyncrhonized eNotKnownInternallySynchronized = InternallySyncrhonized::eNotKnownInternallySynchronized;

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
             *      Synchronized<String> n;                                                 // SAME
             *      Synchronized<String,Synchronized_Traits<>> n;                           // SAME
             *      Synchronized<String,Synchronized_Traits<recursive_mutex>>   n;          // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      Synchronized<String,Synchronized_Traits<SpinLock>>   n;
             */
            template <typename MUTEX           = recursive_mutex,
                      bool IS_RECURSIVE        = std::is_same<MUTEX, recursive_mutex>::value or std::is_same<MUTEX, recursive_timed_mutex>::value,
                      typename READ_LOCK_TYPE  = conditional_t<is_same<MUTEX, shared_timed_mutex>::value or is_same<MUTEX, shared_mutex>::value, shared_lock<MUTEX>, unique_lock<MUTEX>>,
                      typename WRITE_LOCK_TYPE = unique_lock<MUTEX>>
            struct Synchronized_Traits {
                using MutexType = MUTEX;

                static void LOCK_SHARED (MutexType& m);
                static void UNLOCK_SHARED (MutexType& m);

                /// PROTOTYPE - TO REPLACE LOCK_SHARED_UNLOCK_SHARED
                using ReadLockType  = READ_LOCK_TYPE;
                using WriteLockType = WRITE_LOCK_TYPE;

                // Used internally for assertions that the synchonized object is used safely. If you mean to use differntly, specailize
                static constexpr bool kAllowRecursive = IS_RECURSIVE;
            };

            /**
             *  \brief  Wrap any object with Synchonized<> and it can be used similarly to the base type,
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
             *          checking for races (Debug::AssertExternallySynchronizedLock) in most objects, so
             *          you only use Synchronized<> (or some other more performant mechanism) in the few places
             *          you need it.
             *
             *  \note   Synchonized<> is similar to std::atomic, except that
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
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
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
             *  \note   We consider supporting operator-> for Synchonized<> - and overloading on const to see if we use a Read Lock or a Write lock.
             *          The problem is - that IF its called through a non-const object, it will select the non-const (write lock) even though all that
             *          was needed was the read lock! So this paradigm - though more terse and clear - just encourages inefficient coding (so we
             *          have no read locks - all locks write locks).
             *
             *          So ONLY support operator-> const overload (brevity and more common than for write). To write - use rwget().
             */
            template <typename T, typename TRAITS = Synchronized_Traits<>>
            class Synchronized {
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
                 */
                template <typename... ARGUMENT_TYPES>
                Synchronized (ARGUMENT_TYPES&&... args);
                Synchronized (const Synchronized& src);

            public:
                /**
                 */
                nonvirtual Synchronized& operator= (const Synchronized& rhs);
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
                 */
                nonvirtual operator T () const;

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
                 *      Synchronized<Thread::Ptr> sharedData;
                 *      sharedData.load ().AbortAndWaitTilDone ();  // copies thread and doesn't maintain lock during wait
                 *      sharedData->AbortAndWaitTilDone ();         // works off internal copy of thread object, and maintains the lock while accessing
                 *      \endcode
                 */
                nonvirtual T load () const;

            public:
                /**
                 * @see load ()
                 *
                 *  Save the given value into this synchronized object, acquiring the needed write lock first.
                 */
                nonvirtual void store (const T& v);
                nonvirtual void store (T&& v);

            public:
                /**
                 *  \brief  get a read-only smart pointer to the underlying Synchronized<> object, holding the readlock the whole
                 *          time the return (often temporary) ReadableReference exists.
                 *
                 *  \note   this supports multiple readers/single writer, iff the mutex used with Syncrhonized<> supports it (@see Synchronized)
                 *
                 *  \par Example Usage
                 *      auto    lockedConfigData = fConfig_.cget ();
                 *      fCurrentCell_ = lockedConfigData->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = lockedConfigData->fPressure.Value (Pressure::Low);
                 *
                 *  This is roughly equivilent (if using a recursive mutex) to (COUNTER_EXAMPLE):
                 *      lock_guard<Synchronized<T,TRAITS>>  critSec (fConfig_);
                 *      fCurrentCell_ = fConfig_->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = fConfig_->fPressure.Value (Pressure::Low);
                 *
                 *  Except that this works whether using a shared_mutex or regular mutex. Also - this provides only read-only access
                 *  (use rwget for read-write access).
                 */
                nonvirtual ReadableReference cget () const;

            public:
                /**
                 *  \brief  get a read-rwite smart pointer to the underlying Synchronized<> object, holding the full lock the whole
                 *          time the (often temporary) WritableReference exists.
                 */
                nonvirtual WritableReference rwget ();

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
                nonvirtual ReadableReference operator-> () const;

            public:
                /**
                 *  If shared lock syncronization - does shared_lock<>::lock_shared, and otherwise just lock ()
                 */
                nonvirtual void lock_shared () const;

            public:
                /**
                 *  If shared lock syncronization - does shared_lock<>::unlock_shared, and otherwise just unlock ()
                 */
                nonvirtual void unlock_shared () const;

            public:
                /**
                 */
                nonvirtual void lock () const;

            public:
                /**
                 */
                nonvirtual void unlock () const;

            public:
                // @todo - DOCUMENT that this RELEASES the read lock, so whatever values you checked need to be RECHECEKD.
                // @todo - when teh resturned WritableReference reference goes out of scope, this SHOULD (but doesn't yet)
                // RE-LCOK the shared_lock
                nonvirtual WritableReference Experimental_UnlockUpgradeLock (ReadableReference* readReference)
                {
                    AssertNotNull (readReference);
                    AssertNotNull (readReference->fSharedLock_);
                    if (readReference->fSharedLock_->owns_lock ()) {
                        readReference->fSharedLock_->unlock ();
                    }
                    // @todo maybe need todo try_lock here?? Or maybe this is OK - as is - so long as we release lock first
                    return WritableReference (&fProtectedValue_, &fLock_);
                }

            public:
                /**
                 *  A DEFEFCT with this implementation, is that you cannot count on values computed with the read lock to remiain
                 *  true in the upgrade lock (since we unlock and then re-lock).
                 *
                 *  But other than that, this approach seems pretty usable/testable.
                 *
                 *  NOTE - this guarantees readreference remains locked after the call (though due to defects in impl for now - maybe with unlock/relock)
                 *
                 *  @todo - MAYBE add 'ReadableReference* readReference' arg, but maybe not
                 *
                 *  \par Example Usage
                 *      \code
                 *          Execution::RWSynchronized<Status_> fStatus_;
                 *          auto lockedStatus = fStatus_.cget ();
                 *          // do a bunch of code that only needs read access
                 *          if (some rare event) {
                 *              fStatus_.Experimental_UpgradeLock2 ([=](auto&& writeLock) {
                 *                  writeLock.rwref ().fCompletedScans.Add (scan);
                 *              });
                 *          }
                 */
                nonvirtual void Experimental_UpgradeLock2 (const function<void(WritableReference&&)>& doWithWriteLock)
                {
                    // AssertNotNull (readReference);
                    // Assert (readReference->fSharedLock_ == &fLock_);
                    //Require (fLock_.owns_shared_lock ());
                    TRAITS::UNLOCK_SHARED (fLock_);
                    // @todo maybe need todo try_lock here?? Or maybe this is OK - as is - so long as we release lock first
                    [[maybe_unused]] auto&& cleanup = Execution::Finally ([this]() { TRAITS::LOCK_SHARED (fLock_); });
                    doWithWriteLock (WritableReference (&fProtectedValue_, &fLock_));
                }
#if 0
            public:
                // @todo - this will try to keep the readlock in place, but doing that is not legal with C++ std stuff, so must use
                // boost upgrade-lock
                nonvirtual Memory::Optional<WritableReference> Experimental_UpgradeLock (ReadableReference* readReference)
                {
                    AssertNotImplemented ();
                    return WritableReference (&fProtectedValue_, &fLock_);
                }
#endif

            private:
                T                 fProtectedValue_;
                mutable MutexType fLock_;
            };

            /**
             *  Anything that constructs a ReadableReference - for example - Synchronized<T, TRAITS>::cget () - is suitable for multiple readers at the same time,
             *  so long as using Synchonized<> of a type that supports shared locks (@see RWSynchronized<>)
             */
            template <typename T, typename TRAITS>
            class Synchronized<T, TRAITS>::ReadableReference {
            protected:
                /**
                 *  Can construct ReadableReference with nullptr_t mutex, in which case its the subclasses responsability to manage locking
                 *
                 *  \req t != nullptr, and this class holds onto that pointer.
                 */
                ReadableReference (const T* t, nullptr_t);

            public:
                ReadableReference (const T* t, MutexType* m);
                ReadableReference (const ReadableReference& src) = delete; // must move because both src and dest cannot have the unique lock
                ReadableReference (ReadableReference&& src);

            public:
                nonvirtual const ReadableReference& operator= (const ReadableReference& rhs) = delete;

            public:
                ~ReadableReference ();

            public:
                /**
                 *  \note   Considerd losing operator-> here as possibly confusing (e.g. when mixed with Synchronized<Optional<xxx>>>).
                 *          But you dont need to use it, and this really does act as a smart pointer so it should most often just be
                 *          more clear.
                 */
                nonvirtual const T* operator-> () const;

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

            private:
                MutexType* fSharedLock_{}; // cannot use shared_lock<> because must call TRAITS::LOCK_SHARED/UNLOCK_SHARED - this must work with shared_mutex or non-shared mutex
            };

            /**
             */
            template <typename T, typename TRAITS>
            class Synchronized<T, TRAITS>::WritableReference : public Synchronized<T, TRAITS>::ReadableReference {
            public:
                /**
                 */
                WritableReference (T* t, MutexType* m);
                WritableReference (const WritableReference& src) = delete; // must move because both src and dest cannot have the unique lock
                WritableReference (WritableReference&& src);
                nonvirtual WritableReference& operator= (const WritableReference& rhs) = delete;
                nonvirtual const WritableReference& operator                           = (T rhs);

            public:
                /**
                 *  \note   Considerd losing operator-> here as possibly confusing (e.g. when mixed with Synchronized<Optional<xxx>>>).
                 *          But you dont need to use it, and this really does act as a smart pointer so it should most often just be
                 *          more clear.
                 */
                nonvirtual T*    operator-> ();
                nonvirtual const T* operator-> () const;

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
                unique_lock<MutexType> fUniqueLock_;
            };

            /**
             */
            template <typename T, typename TRAITS>
            bool operator< (const Synchronized<T, TRAITS>& lhs, T rhs);
            template <typename T, typename TRAITS>
            bool operator< (T lhs, const Synchronized<T, TRAITS>& rhs);
            template <typename T, typename TRAITS>
            bool operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);

            /**
             */
            template <typename T, typename TRAITS>
            bool operator<= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template <typename T, typename TRAITS>
            bool operator<= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template <typename T, typename TRAITS>
            bool operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);

            /**
             */
            template <typename T, typename TRAITS>
            bool operator== (const Synchronized<T, TRAITS>& lhs, T rhs);
            template <typename T, typename TRAITS>
            bool operator== (T lhs, const Synchronized<T, TRAITS>& rhs);
            template <typename T, typename TRAITS>
            bool operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);

            /**
             */
            template <typename T, typename TRAITS>
            bool operator!= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template <typename T, typename TRAITS>
            bool operator!= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template <typename T, typename TRAITS>
            bool operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);

            /**
             */
            template <typename T, typename TRAITS>
            bool operator>= (const Synchronized<T, TRAITS>& lhs, T rhs);

            /**
             */
            template <typename T, typename TRAITS>
            bool operator> (const Synchronized<T, TRAITS>& lhs, T rhs);

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
             * implemented using a std::mutex, or a SpinLock, whichever is faster. So - dont use this where you hold
             * onto the lock for extended periods ;-).
             */
            template <typename T>
            using QuickSynchronized = conditional_t<kSpinLock_IsFasterThan_mutex, Synchronized<T, Synchronized_Traits<SpinLock>>, Synchronized<T, Synchronized_Traits<mutex>>>;

            /**
             * RWSynchronized will always use some sort of mutex which supports multiple readers, and a single writer. Typically, using shared_mutex (or shared_timed_mutex).
             */
            template <typename T>
            using RWSynchronized = Synchronized<T, Synchronized_Traits<shared_mutex>>;
        }
    }
}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Synchronized.inl"

#endif /*_Stroika_Foundation_Execution_Synchronized_h_*/
