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
 *      @todo   Implement first draft of code based on
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Set.hh
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
            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                Set ();
                Set (const Set<T>& s);
                explicit Set (const T* start, const T* end);

            protected:
                explicit Set (const _SharedPtrIRep& rep);

            public:
                nonvirtual  Set<T>& operator= (const Set<T>& src);

            public:
                /**
                 */
                nonvirtual  bool Contains (T item) const;

            public:
                /**
                 */
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 */
                nonvirtual  void    Add (T item);
                nonvirtual  void    Add (Set<T> items); // note passed by value to avoid s.Add(s) problems (makes little sense so see if we can undo this - but think through carefully)

            public:
                /**
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Set<T>& items);
                nonvirtual  void    Remove (Iterator<T> item);

            public:
                nonvirtual  Set<T>& operator+= (T item);
                nonvirtual  Set<T>& operator+= (const Set<T>& items);
                nonvirtual  Set<T>& operator-= (T item);
                nonvirtual  Set<T>& operator-= (const Set<T>& items);

            protected:
                nonvirtual  const _IRep&    _GetRep () const;
                nonvirtual  _IRep&          _GetRep ();
            };


            /**
             *  \brief  Implementation detail for Set<T> implementors.
             *
             *  Protected abstract interface to support concrete implementations of
             *  the Set<T> container API.
             */
            template    <typename T>
            class   Set<T>::_IRep : public Iterable<T>::_IRep {
            protected:
                _IRep ();

            public:
                virtual ~_IRep ();

            public:
                virtual bool    Contains (T item) const                     =   0;
                virtual void    RemoveAll ()                                =   0;
                virtual void    Add (T item)                                =   0;
                virtual void    Remove (T item)                             =   0;
                virtual void    Remove (const Iterator<T>& i)               =   0;
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
