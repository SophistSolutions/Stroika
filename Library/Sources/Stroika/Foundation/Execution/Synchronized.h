/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_ 1

#include "../StroikaPreComp.h"

#include <mutex>
#if !qCompilerAndStdLib_shared_mutex_module_Buggy
#include <shared_mutex>
#endif

#include "../Configuration/Common.h"
#include "../Configuration/TypeHints.h"
#include "../Debug/Assertions.h" // while RequireNotNull etc in headers --LGP 2015-06-11

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha-Early">Alpha-Early</a>
 *
 * Description:
 *
 * TODO:
 *      @todo   KEY TODO IS MAKE READABLE REFERENCE USE shared_lock iff available
 *              and WritableReference pass in lock not using base class, and have it construct writable lock
 *              But all in a way that works when shared_lock not avaialble and degrades gracefully with template SFINAE traits crap.
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
            template <typename MUTEX = recursive_mutex>
            struct Synchronized_Traits {
                using MutexType = MUTEX;

                static void LOCK_SHARED (MutexType& m);
                static void UNLOCK_SHARED (MutexType& m);
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
             *      Synchronized<String> n;                                                 // SAME
             *      Synchronized<String,Synchronized_Traits<>> n;                           // SAME
             *      Synchronized<String,Synchronized_Traits<recursive_mutex>>       n;      // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      \code
             *      Synchronized<String,Synchronized_Traits<SpinLock>>              n;
             *      \endcode
             *
             *  or to allow timed locks
             *      \code
             *      Synchronized<String,Synchronized_Traits<timed_recursive_mutex>> n;
             *      \endcode
             *
             *  or to read-modify-update in place
             *      \code
             *      //nb: lock lasts til end of enclosing scope
             *      auto    lockedConfigData = fConfig_.rwget ();
             *      lockedConfigData->SomeValueChanged = 1;
             *      \endcode
             *
             *  or to read-modify-update with explicit temporary
             *      \code
             *      //nb: lock lasts til end of enclosing scope
             *      auto    lockedConfigData = fConfig_.rwget ();
             *      T       value = lockedConfigData;
             *      value.SomeValueChanged = 1;
             *      lockedConfigData.store (value);
             *      \endcode
             *
             *  or, because Synchronized<> has lock/unlock methods, it can be used with a lock_guard/make_unique_lock (if associated mutex recursive), as in:
             *      \code
             *      auto    critSec { Execution::make_unique_lock (n) };
             *      // lock to make sure someone else doesn't change n after we made sure it looked good
             *      if (looks_good (n)) {
             *          String a = n;
             *          a = a + a;
             *          n.store (a);
             *      }
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
                 *  \note   Using load () can be most efficient (least lock contention) with read/write locks (RWSynchronized),
                 *          since it just uses a read lock and releases it immediately.
                 *
                 *  \par Example Usage
                 *      \code
                 *      Synchronized<Thread> sharedData;
                 *      sharedData.load ().AbortAndWaitTilDone ();  // copies thread and doesnt maintain lock during wait
                 *      sharedData->AbortAndWaitTilDone ();         // works off internal copy of thread object, and maintains the lock while accessing
                 *      \endcode
                 */
                nonvirtual T load () const;

            public:
                /**
                 */
                nonvirtual void store (Configuration::ArgByValueType<T> v);

            public:
                /**
                 *  \brief  get a read-only smart pointer to the underlying Synchronized<> object, holding the readlock the whole
                 *          time the return (often temporary) ReadableReference exists.
                 *
                 *  \par Example Usage
                 *      auto    lockedConfigData = fConfig_.cget ();
                 *      fCurrentCell_ = lockedConfigData->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = lockedConfigData->fPressure.Value (Pressure::Low);
                 *
                 *  This is equivilent (if using a recursive mutex) to (COUNTER_EXAMPLE):
                 *      lock_guard<Synchronized<T,TRAITS>>  critSec (fConfig_);
                 *      fCurrentCell_ = fConfig_->fCell.Value (Cell::Short);
                 *      fCurrentPressure_ = fConfig_->fPressure.Value (Pressure::Low);
                 *
                 *  Except that the former only does the lock once, and works even with a non-recursive mutex.
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

            private:
                T                 fDelegate_;
                mutable MutexType fLock_;
            };

            /**
             * @todo KEY TODO IS MAKE READABLE REFERENCE USE shared_lock iff available
             *          https://stroika.atlassian.net/browse/STK-491
             */
            template <typename T, typename TRAITS>
            class Synchronized<T, TRAITS>::ReadableReference {
            public:
                /**
                 */
                ReadableReference (const T* t, MutexType* m);
                ReadableReference (const ReadableReference& src) = delete; // must move because both src and dest cannot have the unique lock
                ReadableReference (ReadableReference&& src);
                const ReadableReference& operator= (const ReadableReference& rhs) = delete;

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
                const T*               fT;
                unique_lock<MutexType> l;
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
                nonvirtual       T* operator-> ();
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
                nonvirtual void store (Configuration::ArgByValueType<T> v);
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
             * @todo  prototype - think out
             *  maybe based on ifdef use mutex or SpinLock.
             *
             *  Tentative definition is quick non recursive. Use like 'atomic'.
             */
            template <typename T>
            using QuickSynchronized = Synchronized<T, Synchronized_Traits<mutex>>;

/**
             * @todo  prototype - think out
             */
#if qCompilerAndStdLib_shared_mutex_module_Buggy
            template <typename T>
            using RWSynchronized = Synchronized<T, Synchronized_Traits<recursive_mutex>>;
#else
            template <typename T>
            using RWSynchronized = Synchronized<T, Synchronized_Traits<shared_timed_mutex>>;
#endif
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
