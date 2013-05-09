/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_h_
#define _Stroika_Foundation_Containers_Set_h_  1

/*
 *
 *
 *  STATUS:
 *      Minimally functional (alpha)
 *
 *
 *  TODO:
 *
 *      @todo   Implement first draft of code based on
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/Set.hh
 *
 *      @todo   Implement more backends
 *              >   Set_BitString
 *              >   Set_Array
 *              >   Set_LinkedList
 *              >   Set_HashTable
 *              >   Set_RedBlackTree
 *              >   Set_stlset
 *              >   Set_stlunordered_set (really is hashset)
 *              >   Set_Treap
 *
 *      @todo   Thread safety (/locking)
 *
 */


#include    "../StroikaPreComp.h"

#include    "../Configuration/Common.h"
#include    "../Memory/SharedByValue.h"

#include    "Iterable.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *      The Set class is based on SmallTalk-80, The Language & Its Implementation,
             *      page 148.
             *
             *      The basic idea here is that you cannot have multiple copies of the same
             *      thing into the set (like a mathemactical set).
             *
             */
            template    <typename   T>
            class   Set : public Iterable<T> {
            private:
                typedef Iterable<T> inherited;

            protected:
                class   _IRep;
                typedef shared_ptr<_IRep>   _SharedPtrIRep;

            public:
                /**
                 */
                Set ();
                Set (const Set<T>& s);
                template <typename CONTAINER_OF_T>
                explicit Set (const CONTAINER_OF_T& s);
                template <typename COPY_FROM_ITERATOR>
                explicit Set (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);

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
                nonvirtual  void    Add (T item);

            public:
                /**
                 */
                template    <typename COPY_FROM_ITERATOR>
                nonvirtual  void    AddAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    AddAll (const CONTAINER_OF_T& s);

            public:
                /**
                 */
                nonvirtual  void    Remove (T item);
                nonvirtual  void    Remove (const Iterator<T>& i);

            public:
                template    <typename COPY_FROM_ITERATOR>
                nonvirtual  void    RemoveAll (COPY_FROM_ITERATOR start, COPY_FROM_ITERATOR end);
                template    <typename CONTAINER_OF_T>
                nonvirtual  void    RemoveAll (const CONTAINER_OF_T& s);
                nonvirtual  void    RemoveAll ();

            public:
                /**
                 * \brief STL-ish alias for RemoveAll ().
                 */
                nonvirtual  void    clear ();

            public:
                /*
                 *  Convert Set<T> losslessly into a standard supported C++ type.
                 *  Supported types include:
                 *      o   set<T>
                 *      o   vector<T>
                 *      o   list<T>
                 *      (maybe any container that takes CTOR (IT BEGIN, IT END) - but dont count on that yet...
                 */
                template    <typename   CONTAINER_OF_T>
                nonvirtual  CONTAINER_OF_T   As () const;
                template    <typename   CONTAINER_OF_T>
                nonvirtual  void    As (CONTAINER_OF_T* into) const;


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
