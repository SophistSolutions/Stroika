/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    <mutex>

#include    "Common.h"
#include    "SpinLock.h"        // just needed while we support LEGACY_Synchronized



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * Description:
 *
 * TODO:
 *      @todo   Testing new nu_Synchronized design and if it works, covnert everything!
 *
 *      @todo   Test that
 *              Possible design flaw with new synchronized.
 *
 *              F(const container & )
 *              Synchronized <container> g
 *              F(g)
 *
 *              always works. We use operator T() to fix this but I'm not 100% sure that works.
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


            /*
             *  MUTEX:
             *      We chose to make the default MUTEX recursive_mutex - since most patterns of use will be supported
             *      by this safely.
             *
             *      To use timed-locks, use timed_recursive_mutex.
             *
             *      If recursion is not necessary, and for highest performance, SpinLock will often work best.
             */
            template    <typename   T, typename MUTEX = recursive_mutex>
            struct  nu_Synchronized_Traits {
                using  MutexType = MUTEX;
            };

            template    <typename   T, typename TRAITS = nu_Synchronized_Traits<T>>
            class   nu_Synchronized {
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
                nu_Synchronized (ARGUMENT_TYPES&& ...args)
                    : fDelegate_ (std::forward<ARGUMENT_TYPES> (args)...)
                {
                }

            public:
                // use template forwarding variadic CTOR formward
                const nu_Synchronized& operator= (const T& rhs)
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    fDelegate_ = rhs;
                    return *this;
                }

            public:
                nonvirtual  operator T () const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_;
                }

            public:
                nonvirtual  WritableReference GetReference ()
                {
                    return move (WritableReference (&fDelegate_, &fLock_));
                }

            public:
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
                bool operator== (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ == rhs;
                }
                bool operator!= (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ != rhs;
                }
                bool operator< (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ < rhs;
                }
                bool operator<= (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ <= rhs;
                }
                bool operator> (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ > rhs;
                }
                bool operator>= (T rhs) const
                {
                    MACRO_LOCK_GUARD_CONTEXT (fLock_);
                    return fDelegate_ >= rhs;
                }
#if 1
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
#endif
            private:
                T                   fDelegate_;
                mutable MutexType   fLock_;
            };



            //migrate names support
            template    <typename   T, typename MUTEX = recursive_mutex>
            using Synchronized_Traits =   nu_Synchronized_Traits<T, MUTEX>;
            template    <typename   T, typename TRAITS = Synchronized_Traits<T>>
            using   Synchronized  = nu_Synchronized<T, TRAITS>;


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
