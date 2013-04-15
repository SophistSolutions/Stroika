/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

/*
 *
 *
 *  STATUS:
 *      CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika
 *
 *
 *
 *  TODO:
 *
 *      (o)         Implement first draft of code based on
 *                  http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Set.hh
 *
 *      (o)         Should inherit from Iterable<T>
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
             *      The Set class is based on SmallTalk-80, The Language & Its Implementation,
             *      page 148.  Also, see documentation for Collection<T>.
             *
             *      The basic idea here is that you cannot have multiple entries of the same
             *      thing into the set.
             *
             */
            template    <template   T>
            class Set : public Iterable<T> {
            public:
                Set ();
                Set (const Set<T>& src);
                // redo as iterator<T> like STL
                Set (const T* start, const T* end);

            protected:
                Set (SetRep<T>* rep);

            public:
                nonvirtual  Set<T>& operator= (const Set<T>& src);

            public:
                nonvirtual  Boolean Contains (T item) const;

            public:
                nonvirtual  void    RemoveAll ();

            public:
                nonvirtual  void    Compact ();

            public:
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (Set<T> items); // note passed by value to avoid s.Add(s) problems

            public:
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Set<T>& items);
                nonvirtual  void    Remove (Iterator<T> item);

            public:
                nonvirtual  Set<T>& operator+= (T item);
                nonvirtual  Set<T>& operator+= (const Set<T>& items);
                nonvirtual  Set<T>& operator-= (T item);
                nonvirtual  Set<T>& operator-= (const Set<T>& items);

            protected:
                nonvirtual  void    AddItems (const T* items, size_t size);

                nonvirtual  const SetRep<T>*    GetRep () const;
                nonvirtual  SetRep<T>*          GetRep ();

            private:
                Shared<SetRep<T> >  fRep;

                static  SetRep<T>*  Clone (const SetRep<T>& rep);

                friend  Boolean operator== (const Set<T>& lhs, const Set<T>& rhs);
            };


        }
    }
}

#endif  /*_Stroika_Foundation_Containers_Set_h_ */



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include    "Set.inl"
