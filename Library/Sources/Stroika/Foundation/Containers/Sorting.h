/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Sorting_h_
#define _Stroika_Foundation_Containers_Sorting_h_  1

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

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      A Sorting is a collection whose elements are ordered by an external
             *  comparison function (operator <). The src to can be added to, removed
             *  from, and iterated over. Adding and removing specify only the item -
             *  where it is added is implied by its value, and the operator < function.
             *  Iteration is defined to go forwards from lesser to greater values -
             *  again, as defined by the ordering function (operator <).
             *
             *      We also require an operator== too be able to remove a given item
             *  from a src, and to check whether or not an item is contained in the src.
             *
             *      Sortings do allow redundencies - that is the same element may be
             *  inserted more than once, and still increase the length of the src.
             *
             *
             */
            template    <class T>
            class   Sorting {
            public:
                Sorting ();
                Sorting (const Sorting<T>& src);
                Sorting (const T* items, size_t size);

            protected:
                Sorting (SortingRep<T>* src);

            public:
                nonvirtual  Sorting<T>& operator= (const Sorting<T>& src);

            public:
                nonvirtual  Boolean     Contains (T item) const;

            public:
                nonvirtual  void        RemoveAll ();

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (const Iterator<T>& itemsIterator);

            public:
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& itemsIterator);

            public:
                nonvirtual  Sorting<T>& operator+= (T item);
                nonvirtual  Sorting<T>& operator+= (const Iterator<T>& itemsIterator);
                nonvirtual  Sorting<T>& operator-= (T item);
                nonvirtual  Sorting<T>& operator-= (const Iterator<T>& itemsIterator);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Sorting_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Sorting.inl"
