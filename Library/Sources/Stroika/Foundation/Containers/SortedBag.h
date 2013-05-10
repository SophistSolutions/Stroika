/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedBag_h_
#define _Stroika_Foundation_Containers_SortedBag_h_  1

/*
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Sorting.hh
 *
 *
 */


#include    "../StroikaPreComp.h"

#include    "Bag.h"




namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A SortedBag is a collection whose elements are ordered by an external
             *  comparison function (operator <). The src to can be added to, removed
             *  from, and iterated over. Adding and removing specify only the item -
             *  where it is added is implied by its value, and the operator < function.
             *  Iteration is defined to go forwards from lesser to greater values -
             *  again, as defined by the ordering function (operator <).
             *
             *      We also require an operator== too be able to remove a given item
             *  from a src, and to check whether or not an item is contained in the src.
             *
             *      SortedBags do allow redundencies - that is the same element may be
             *  inserted more than once, and still increase the length of the src.
             *
             *
             */
            template    <class T>
            class   SortedBag : public Bag<T> {
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                SortedBag ();
                SortedBag (const Set<T>& s);
                explicit SortedBag (const T* start, const T* end);

            protected:
                explicit SortedBag (const _SharedPtrIRep& rep);

            public:
                nonvirtual  SortedBag<T>& operator= (const SortedBag<T>& src);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_SortedBag_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "SortedBag.inl"
