/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"

#include    "SpinLock.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * Description:
 *
 * TODO:
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
            class   Synchronized : public T {
            private:
                using   inherited = T;

            public:
                Synchronized ();
                Synchronized (const T& from);
                Synchronized (const Synchronized& from);
                const Synchronized& operator= (const Synchronized& rhs);

            public:
                nonvirtual  operator T () const;

            private:
                SpinLock    fLock_;
            };


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
