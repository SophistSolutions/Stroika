/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2016.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>
#if     !qCompilerAndStdLib_shared_mutex_module_Buggy
#include    <shared_mutex>
#endif

#include    "../Configuration/Common.h"
#include    "../Debug/Assertions.h"         // while RequireNotNull etc in headers --LGP 2015-06-11

#include    "Common.h"


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


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
            ***** SCRAP NOTES - SOME OF THIS TEXT APPLIES TO NEW SYNCHO... CLEANUP

            * @todo
            *
            *
             *  The idea behind any of these synchronized classes is that they can be used freely from
             *  different threads without worry of data corruption. It is almost as if each operation were
             *  preceeded with a mutex lock (on that object) and followed by an unlock.
             *
             *  Note - we say 'almost' because there is no guarantee of atomicity: just consistency and safety.
             *
             *  For example, an implementation of something like "Apply(function)" might do atomic locks
             *  between each functional call, or some other way. But corruption or logical inconsistency
             *  will be avoided.
             *
             *  If one thread does a Read operation on Synchronized<T> while another does a write (modification)
             *  operation on Synchronized<T>, the Read will always return a consistent reasonable value, from
             *  before the modification or afterwards, but never a distorted invalid value.
             *
             *  This is very much closely logically the java 'synchronized' attribute, except that its
             *  not a language extension/attribute here, but rather a class wrapper. Its also implemented
             *  in the library, not the programming language.
             *
             *  Its also in a way related to std::atomic<> - in that its a class wrapper on another type,
             *  but the semantics it implements are moderately different than those in std::atomic,
             *  which is really just intended to operate on integers, and integer type things, and not on
             *  objects with methods.
             *
             *  \note   LIKE JAVA SYNCHRONIZED
             *          This is SIMPLE to use like the Java (and .net) synchonized attribute(lock) mechanism.
             *          But why does it not suffer from the same performance deficiit?
             *
             *          Because with Java - you mixup exceptions and assertions. With Stroika, we have builtin
             *          checking for races (Debug::AssertExternallySynchronizedLock) in most objects, so
             *          you only use Synchonized<> (or some other more performant mechanism) in the few places
             *          you need it.
             */


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
             *      Synchonized<String> n;                                                  // SAME
             *      Synchonized<String,Synchronized_Traits<>> n;                            // SAME
             *      Synchonized<String,Synchronized_Traits<recursive_mutex>>    n;          // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      Synchonized<String,Synchronized_Traits<SpinLock>>   n;
             */
            template    <typename MUTEX = recursive_mutex>
            struct  Synchronized_Traits {
                using   MutexType   =   MUTEX;
            };


            /**
             *  \par Example Usage
             *      \code
             *      Synchonized<String> n;                                                  // SAME
             *      Synchonized<String,Synchronized_Traits<>> n;                            // SAME
             *      Synchonized<String,Synchronized_Traits<recursive_mutex>>    n;          // SAME
             *      \endcode
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      \code
             *      Synchonized<String,Synchronized_Traits<SpinLock>>           n;
             *      \endcode
             *
             *  or to allow timed locks
             *      \code
             *      Synchonized<String,Synchronized_Traits<timed_recursive_mutex>>  n;
             *      \endcode
             */
            template    <typename   T, typename TRAITS = Synchronized_Traits<>>
            class   Synchronized {
            public:
                using   MutexType = typename TRAITS::MutexType;

            public:
                struct  ReadableReference;

            public:
                struct  WritableReference;

            public:
                template    <typename ...ARGUMENT_TYPES>
                Synchronized (ARGUMENT_TYPES&& ...args);
                Synchronized (const Synchronized& src);

            public:
                nonvirtual  Synchronized&   operator= (const Synchronized& rhs);
                nonvirtual  Synchronized&   operator= (const T& rhs);

            public:
                /**
                 *  Synonym for load ()
                 ****
                 ***    @todo DECIDE IF WE WANT TO USE EXPLICIT
                 ****           http://stackoverflow.com/questions/27573928/are-explicit-conversion-operators-allowed-in-braced-initializer-lists
                 *** VERY UNCELAR - FIUDDLE WITH THIS
                 */
                nonvirtual   /*explicit*/ operator T () const;

            public:
                /**
                 */
                nonvirtual  T   load () const;

            public:
                /**
                 */
                nonvirtual  void    store (const T& v);

            public:
                /**
                 *  \par Example Usage
                 *      auto    lockedConfigData = fConfig_.GetReference ();
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
                nonvirtual  const WritableReference GetReference () const;
                nonvirtual  WritableReference GetReference ();

            public:
                /*
                 *UNCLEAR BUT MAYBE DEPRECATE??? CONFUSING? OR overload const readonly and no-const writable, but with other stuff
                 * like SharedByValue() I found that I accidentally got the wrong one alot.
                 */
                nonvirtual  const WritableReference operator-> () const;
                nonvirtual  WritableReference operator-> ();

            public:
                /**
                 */
                nonvirtual  void    lock ();

            public:
                /**
                 */
                nonvirtual  void    unlock ();

            private:
                T                   fDelegate_;
                mutable MutexType   fLock_;
            };


            /**
            * @todo KEY TODO IS MAKE READABLE REFERENCE USE shared_lock iff available
             */
            template    <typename   T, typename TRAITS>
            struct  Synchronized<T, TRAITS>::ReadableReference {
                const T*                fT;
                unique_lock<MutexType>  l;
                ReadableReference (const T* t, MutexType* m)
                    : fT (t)
                    , l (*m)
                {
                    RequireNotNull (t);
                    RequireNotNull (m);
                }
                ReadableReference (const ReadableReference& src) = delete;      // must move because both src and dest cannot have the unique lock
                ReadableReference (ReadableReference&& src)
                    : fT (src.fT)
                    , l { move (src.l) }
                {
                    src.fT = nullptr;
                }
                const ReadableReference& operator= (const ReadableReference& rhs) = delete;
                const T* operator-> () const
                {
                    EnsureNotNull (fT);
                    return fT;
                }
                operator const T& () const
                {
                    EnsureNotNull (fT);
                    return *fT;
                }
                T   load () const
                {
                    EnsureNotNull (fT);
                    return *fT;
                }
            };


            /**
             */
            template    <typename   T, typename TRAITS>
            struct  Synchronized<T, TRAITS>::WritableReference : Synchronized<T, TRAITS>::ReadableReference {
                WritableReference (T* t, MutexType* m)
                    : ReadableReference (t, m)
                {
                }
                WritableReference (const WritableReference& src) = delete;      // must move because both src and dest cannot have the unique lock
                WritableReference (WritableReference&& src)
                    : ReadableReference (move (src))
                {
                }
                const WritableReference& operator= (const WritableReference& rhs) = delete;
                const WritableReference& operator= (T rhs)
                {
                    RequireNotNull (this->fT);
                    // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                    *const_cast<T*> (this->fT) = rhs;
                    return *this;
                }
                T* operator-> ()
                {
                    // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                    EnsureNotNull (this->fT);
                    return const_cast<T*> (this->fT);
                }
                const T* operator-> () const
                {
                    return ReadableReference::operator-> ();
                }
                void    store (const T& v)
                {
                    // const_cast Safe because the only way to construct one of these is from a non-const pointer, or another WritableReference
                    *const_cast<T*> (this->fT) = v;
                }

            };


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator< (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator< (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator<= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator<= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator== (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator== (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator!= (const Synchronized<T, TRAITS>& lhs, T rhs);
            template    <typename   T, typename TRAITS>
            bool    operator!= (T lhs, const Synchronized<T, TRAITS>& rhs);
            template    <typename   T, typename TRAITS>
            bool    operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator>= (const Synchronized<T, TRAITS>& lhs, T rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            bool    operator> (const Synchronized<T, TRAITS>& lhs, T rhs);


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} ^ T {});
            template    <typename   T, typename TRAITS>
            auto    operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {});
            template    <typename   T, typename TRAITS>
            auto    operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {});


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} * T {});
            template    <typename   T, typename TRAITS>
            auto    operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {});
            template    <typename   T, typename TRAITS>
            auto    operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {});


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} + T {});
            template    <typename   T, typename TRAITS>
            auto    operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {});
            template    <typename   T, typename TRAITS>
            auto    operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {});


            /**
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            auto    operator-= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator-= (rhs));


            /**
             */
            template    <typename   T, typename TRAITS, typename RHSTYPE>
            auto    operator+= (Synchronized<T, TRAITS>& lhs, RHSTYPE rhs) -> decltype (lhs->operator+= (rhs));


            /**
             */
            template    <typename   T, typename TRAITS>
            auto    operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} - T {});
            template    <typename   T, typename TRAITS>
            auto    operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {});
            template    <typename   T, typename TRAITS>
            auto    operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {});



            /**
             * @todo  prototype - think out
             *  maybe based on ifdef use mutex or SpinLock.
             *
             *  Tentative definition is quick non recursive. Use like 'atomic'.
             */
            template    <typename   T>
            using   QuickSynchonized = Synchronized<T, Synchronized_Traits<mutex>>;


            /**
             * @todo  prototype - think out
             */
#if     qCompilerAndStdLib_shared_mutex_module_Buggy
            template    <typename   T>
            using   RWSynchonized = Synchronized<T, Synchronized_Traits<recursive_mutex>>;
#else
            template    <typename   T>
            using   RWSynchonized = Synchronized<T, Synchronized_Traits<shared_timed_mutex>>;
#endif


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Synchronized.inl"

#endif  /*_Stroika_Foundation_Execution_Synchronized_h_*/
