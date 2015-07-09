/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Configuration/Common.h"
#include    "../Debug/Assertions.h"         // while RequireNotNull etc in headers --LGP 2015-06-11

#include    "Common.h"


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * Description:
 *
 * TODO:
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
             *  EXAMPLE:
             *      Synchonized<String> n;                                                  // SAME
             *      Synchonized<String,Synchronized_Traits<String>> n;                      // SAME
             *      Synchonized<String,Synchronized_Traits<String, recursive_mutex>>    n;  // SAME
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      Synchonized<String,Synchronized_Traits<String, SpinLock>>   n;
             */
            template    <typename   T, typename MUTEX = recursive_mutex>
            struct  Synchronized_Traits {
                using   MutexType   =   MUTEX;
            };


            /**
             *  EXAMPLE:
             *      Synchonized<String> n;                                                  // SAME
             *      Synchonized<String,Synchronized_Traits<String>> n;                      // SAME
             *      Synchonized<String,Synchronized_Traits<String, recursive_mutex>>    n;  // SAME
             *
             *  or slightly faster, but possibly slower or less safe (depnding on usage)
             *      Synchonized<String,Synchronized_Traits<String, SpinLock>>   n;
             *
             *  or to allow timed locks
             *      Synchonized<String,Synchronized_Traits<String, timed_recursive_mutex>>  n;
             */
            template    <typename   T, typename TRAITS = Synchronized_Traits<T>>
            class   Synchronized {
            public:
                using   MutexType = typename TRAITS::MutexType;

            public:
                struct  WritableReference;

            public:
                template    <typename ...ARGUMENT_TYPES>
                Synchronized (ARGUMENT_TYPES&& ...args);
                Synchronized (const Synchronized& src);

            public:
                // use template forwarding variadic CTOR formward
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
                 * EXAMPLE:
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
                nonvirtual  WritableReference GetReference ();
                nonvirtual  const WritableReference GetReference () const;

            public:
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
             */
            template    <typename   T, typename TRAITS>
            struct  Synchronized<T, TRAITS>::WritableReference {
                T*                      fT;
                unique_lock<MutexType>  l;
                WritableReference (T* t, MutexType* m)
                    : fT (t)
                    , l (*m)
                {
                    RequireNotNull (t);
                    RequireNotNull (m);
                }
                WritableReference (const WritableReference& src) = delete;      // must move because both src and dest cannot have the unique lock
                WritableReference (WritableReference&& src)
                    : fT (src.fT)
                    , l { move (src.l) } {
                    src.fT = nullptr;
                }
                const WritableReference& operator= (const WritableReference& rhs) = delete;
                const WritableReference& operator= (T rhs)
                {
                    RequireNotNull (fT);
                    *fT = rhs;
                    return *this;
                }
                T* operator-> ()
                {
                    EnsureNotNull (fT);
                    return fT;
                }
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
