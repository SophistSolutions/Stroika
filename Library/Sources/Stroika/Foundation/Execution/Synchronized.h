/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Synchronized_h_
#define _Stroika_Foundation_Execution_Synchronized_h_    1

#include    "../StroikaPreComp.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Early-Alpha">Early-Alpha</a>
 *
 * Description:
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  This class template cannot be 'used' directly. Rather it denotes a pattern, for creating
             *  automatically synchronized classes.
             *
             *  Instead, (partial) specializations are provided throughout Stroika, for classes that
             *  are automatically synchonized.
             *
             *  The idea behind any of these synchonized classes is that they can be used freely from
             *  different threads without worry of data corruption. It is as if each operation were
             *  preceeded with a mutex lock (on that object) and followed by an unlock.
             *
             *  This is ESPECAILLY critical for maintaining reference counts (as many Stroika objects
             *  are just reference counting containers).
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
            public:
                Synchronized() = delete;
                Synchronized(const Synchronized&) = delete;
                const Synchronized& operator= (const Synchronized&) = delete;
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
