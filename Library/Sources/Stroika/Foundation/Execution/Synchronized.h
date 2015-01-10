/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "../Configuration/Common.h"

#include    "Common.h"



//#define qSUPPORT_LEGACY_SYNCHO  1
#ifndef qSUPPORT_LEGACY_SYNCHO
#define qSUPPORT_LEGACY_SYNCHO  0
#endif


#if qSUPPORT_LEGACY_SYNCHO
#include    "SpinLock.h"        // just needed while we support LEGACY_Synchronized
#endif



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * Description:
 *
 * TODO:
 *      @todo   More operator<, etc overloads
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  This class template it denotes a pattern for creating automatically synchronized classes.
             *
             *  It contains no generic implementation of synchonizaiton, but instead, (partial) specializations
             *  are provided throughout Stroika, for classes that are automatically synchronized.
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
             *  This is ESPECAILLY critical for maintaining reference counts (as many Stroika objects
             *  are just reference counting containers).
             *
             *  EXAMPLE:
             *      static Optional<int>                sThisVarIsNotThreadsafe;
             *      static Synchronized<Optional<int>>  sThisIsSameAsAboveButIsThreadSafe;
             *
             *      // the variable sThisIsSameAsAboveButIsThreadSafe and be gotten or set from any thread
             *      // without fear of occurption. Each individual API is threadsafe. However, you CANNOT
             *      // safely do
             *      //      if (sThisIsSameAsAboveButIsThreadSafe.IsPresent()) { print (*sThisIsSameAsAboveButIsThreadSafe); }
             *      //  Instead do
             *      //      print (sThisIsSameAsAboveButIsThreadSafe.Value ()); OR
             *      //      auto tmp = sThisIsSameAsAboveButIsThreadSafe;
             *      //      if (tm.IsPresent()) { print (*tmp); }
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
#if qSUPPORT_LEGACY_SYNCHO
            template    <typename   T>
            class   LEGACY_Synchronized {
            public:
                LEGACY_Synchronized ();
                LEGACY_Synchronized (const T& from);
                LEGACY_Synchronized (const LEGACY_Synchronized& from);
                const LEGACY_Synchronized& operator= (const LEGACY_Synchronized& rhs);

            public:
                nonvirtual  operator T () const;

            private:
                T           fDelegate_;
                SpinLock    fLock_;
            };
#endif


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
                using  MutexType = MUTEX;
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
                struct  WritableReference {
                    T* fT;
                    unique_lock<MutexType> l;
                    WritableReference (T* t, MutexType* m)
                        : fT (t)
                        , l (*m)
                    {
                        RequireNotNull (t);
                        RequireNotNull (m);
                    }
                    WritableReference (const WritableReference& src) = default;
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

            public:
                template    <typename ...ARGUMENT_TYPES>
                Synchronized (ARGUMENT_TYPES&& ...args)
                    : fDelegate_ (std::forward<ARGUMENT_TYPES> (args)...)
                {
                }
                Synchronized (const Synchronized& src)
                    : fDelegate_ (src.load ())
                {
                }

            public:
                // use template forwarding variadic CTOR formward
                Synchronized&   operator= (const Synchronized& rhs)
                {
                    if (&rhs != this) {
                        MACRO_LOCK_GUARD_CONTEXT (fLock_);
                        fDelegate_ = rhs.load ();
                    }
                    return *this;
                }
                Synchronized&   operator= (const T& rhs)
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    fDelegate_ = rhs;
                    return *this;
                }

            public:
                /**
                 *  Synonym for load ()
                 ****
                 ***    @todo DECIDE IF WE WANT TO USE EXPLICIT
                 */
                nonvirtual   explicit operator T () const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_;
                }

            public:
                /**
                 */
                nonvirtual  T load () const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_;
                }

            public:
                /**
                 */
                nonvirtual  void    store (const T& v)
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    fDelegate_ = v;
                }

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
                nonvirtual  WritableReference GetReference ()
                {
                    return move (WritableReference (&fDelegate_, &fLock_));
                }
                nonvirtual  const WritableReference GetReference () const
                {
                    return move (WritableReference (&fDelegate_, &fLock_));
                }

            public:
                nonvirtual  const WritableReference operator-> () const
                {
                    return GetReference ();
                }
            public:
                nonvirtual  WritableReference operator-> ()
                {
                    return GetReference ();
                }

            public:
                void    lock ()
                {
                    fLock_.lock ();
                }

            public:
                void    unlock ()
                {
                    fLock_.unlock ();
                }

            private:
                T                   fDelegate_;
                mutable MutexType   fLock_;
            };



            template    <typename   T, typename TRAITS>
            inline  bool    operator< (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () < rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator< (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs < rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator< (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () < rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () <= rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs <= rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator<= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () <= rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  bool    operator== (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () == rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator== (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs == rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator== (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () == rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () != rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (T lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs != rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  bool    operator!= (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs)
            {
                return lhs.load () != rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  bool    operator>= (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () >= rhs;
            }


            template    <typename   T, typename TRAITS>
            inline  bool    operator> (const Synchronized<T, TRAITS>& lhs, T rhs)
            {
                return lhs.load () > rhs;
            }


            // @todo EXPERIMENTAL...
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} ^ T {})
            {
                return lhs.load () ^ rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {})
            {
                return lhs ^ rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator^ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} ^ T {})
            {
                return lhs.load () ^ rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  auto    operator* (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} * T {})
            {
                return lhs.load () * rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator* (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {})
            {
                return lhs * rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator* (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} * T {})
            {
                return lhs.load () * rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} + T {})
            {
                return lhs.load () + rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {})
            {
                return lhs + rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator+ (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} + T {})
            {
                return lhs.load () + rhs.load ();
            }


            template    <typename   T, typename TRAITS>
            inline  auto    operator- (const Synchronized<T, TRAITS>& lhs, T rhs) -> decltype (T {} - T {})
            {
                return lhs.load () + rhs;
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator- (T lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {})
            {
                return lhs - rhs.load ();
            }
            template    <typename   T, typename TRAITS>
            inline  auto    operator- (const Synchronized<T, TRAITS>& lhs, const Synchronized<T, TRAITS>& rhs) -> decltype (T {} - T {})
            {
                return lhs.load () - rhs.load ();
            }



            //migrate names support
#if 0
            template    <typename   T, typename MUTEX = recursive_mutex>
            using nu_Synchronized_Traits =   Synchronized_Traits<T, MUTEX>;
            template    <typename   T, typename TRAITS = Synchronized_Traits<T>>
            using   nu_Synchronized  = Synchronized<T, TRAITS>;
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
