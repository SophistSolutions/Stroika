/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "../../Common.h"


/**
 *
 * TODO:
 *
 *
 *      @todo   Move LinkedListIterator to LinkedList<T>::ForwardIterator
 *              (in other words - change name AND make nested)
 *
 *      @todo   Consider losing fSize data member - and storing it just as needed in things like Sequence_LinkedList (not even sure there - dont use LL
 *              if you want to know the size!!!)
 *
 *
 *      @todo   Do MUTATORS (or some such) to avoid making

                    public://////WORKRARBOUND - NEED MUTATOR TO ACCESS THIS SO OUR PROTECTED STUFF NOT NEEDED BY PATCHING CODE


 *      @todo   MAJOR cleanup needed - nearly an entire rewrite. This code is very old and worn...
 *
 *
 *          +   Consider adding LinkedListMutator<T> since otherwise we cannot do InsertAt,
 *              / AddAfter() etc without using a LinkedList_Patch.
 *
 * Notes:
 *
 *      Slightly unusual behaviour for LinkedListMutator_Patch<T>::AddBefore () -
 *  allow it to be called when we are Done() - otherwise there is NO
 *  WAY TO APPEND TO A LINK LIST!!!
 *
 *
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    template    <typename   T>
                    class   LinkedListIterator;


                    /*
                     *      LinkedList<T> is a generic link (non-intrusive) list implementation.
                     *  It keeps a length count so access to its length is rapid. We provide
                     *  no public means to access the links themselves.
                     *
                     *      Since this class provides no patching support, it is not generally
                     *  used - more likely you want to use LinkedList_Patch<T>. Use this if you
                     *  will manage all patching, or know that none is necessary.
                     */
                    template    <typename   T>
                    class   LinkedList {
                    public:
                        LinkedList ();
                        LinkedList (const LinkedList<T>& from);
                        ~LinkedList ();

                    public:
                        nonvirtual  LinkedList<T>& operator= (const LinkedList<T>& list);

                    public:
                        nonvirtual  size_t  GetLength () const;

                        /*
                         *      Basic access to the head of the list. This is what is most
                         *  commonly used, and is most efficient.
                         */
                    public:
                        nonvirtual  T       GetFirst () const;
                        nonvirtual  void    Prepend (T item);
                        nonvirtual  void    RemoveFirst ();

                        /*
                         *  Utility to search the list for the given item using operator==
                         */
                    public:
                        nonvirtual  bool    Contains (T item) const;


                    public:
                        nonvirtual  void    Remove (T item);
                        nonvirtual  void    RemoveAll ();


                        /*
                         *      Not alot of point in having these, as they are terribly slow,
                         *  but the could be convienient.
                         */
                    public:
                        nonvirtual  T       GetAt (size_t i) const;
                        nonvirtual  void    SetAt (T item, size_t i);

                    public:
                        nonvirtual  void    Invariant () const;

                    public:
                        class   Link {
                        public:
                            DECLARE_USE_BLOCK_ALLOCATION (Link);
                        public:
                            Link (T item, Link* next);

                        public:
                            T        fItem;
                            Link*    fNext;
                        };


                    public://////WORKRARBOUND - NEED MUTATOR TO ACCESS THIS SO OUR PROTECTED STUFF NOT NEEDED BY PATCHING CODE
                        size_t      fLength;
                        Link*       fFirst;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const;
#endif

                    private:
                        friend  class   LinkedListIterator<T>;
                    };


                    /*
                     *      LinkedListIterator<T> allows you to iterate over a LinkedList<T>. Its API
                     *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                     *  It is unpatched - use LinkedListIterator_Patch<T> or LinkedListMutator_Patch<T>
                     *  for that.
                     */
                    template    <class T>
                    class   LinkedListIterator {
                    public:
                        LinkedListIterator (const LinkedListIterator<T>& from);
                        LinkedListIterator (const LinkedList<T>& data);

                    public:
                        nonvirtual  LinkedListIterator<T>& operator= (const LinkedListIterator<T>& list);

                    public:
                        nonvirtual  bool    Done () const;
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  T       Current () const;

                        nonvirtual  void    Invariant () const;

                    protected:
                        const typename LinkedList<T>::Link*     fCurrent;
                        bool            fSuppressMore;  // Indicates if More should do anything, or if were already Mored...

#if     qDebug
                        virtual void    Invariant_ () const;
#endif
                    };


                }
            }
        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_LinkedList_h_ */
