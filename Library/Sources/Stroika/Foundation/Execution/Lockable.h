/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Execution_Lockable_h_
#define _Stroika_Foundation_Execution_Lockable_h_   1

#include    "../StroikaPreComp.h"

#include    <mutex>



/**
 *  \file
 *
 * TODO:
 *      @todo   Support for POD-types is incomplete, and requires lots of typing. It would be nice to find
 *              a better way. Baring that, implement the remaining cases...
 *
 *      @todo   Two styles - one where critical section stored with EACH instance, and one where its stored
 *              per-class. These can be used somewhat interchangably, except that the per-class one is
 *              much cheaper, BUT prevents concurency allowed by per-instance locks. Maybe make this choice
 *              a (default) mixin paramter?
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Execution {


            /**
             *  This template provides essentially no functionality - except that its a handy way to associate
             *  a CriticalSection with a piece of data it locks (making it easier and more practical to have
             *  a single critical section for each peice of data)
             *
             *  You can use Lockable<T> in most places like a "T" - but with the added ability to use it with
             *  lock_guard<recursive_mutex>.
             *
             *      @note this works with POD-types like bool etc, only through explicit template specialization.
             */
            template    <typename BASE, typename LOCKTYPE = std::recursive_mutex>
            class   Lockable : public BASE, public LOCKTYPE {
            public:
                Lockable ();
                Lockable (const BASE& from);
                Lockable (const Lockable<BASE, LOCKTYPE>& from);

                Lockable& operator= (const BASE& rhs);
                Lockable& operator= (const Lockable<BASE, LOCKTYPE>& rhs);
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Lockable.inl"

#endif  /*_Stroika_Foundation_Execution_Lockable_h_*/
