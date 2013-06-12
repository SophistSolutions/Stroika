/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "../../Common.h"



/*
 *
 * TODO:
 *
 *      @todo   Add MUTATOR object - (or similar) - so we can make fLength/fFirst protected below!!!
 *
 *      @todo   FIX DoubleLink to be nested class inside DoublyLinkedList
 *
 *      @todo   Somheow when this got ported from old code - we lost fLast??? Maybe irrelevant if we switch to
 *              using STL list??? But ..??
 *
 *      @todo   MAJOR cleanup needed - nearly an entire rewrite. This code is very old and worn...
 *
 *      @todo   WARNING - NOT REAL DOUBLE LINKED LIST IMPL - REALLY SINGLE - SEE STROIKA CODE FOR REAL DOUBLE LINK LIST IMPL
 *
 *
 * Notes:
 *
 *          <<< WARNING - NOT REAL DOUBLE LINKED LIST IMPL - REALLY SINGLE - SEE STROIKA CODE FOR REAL DOUBLE LINK LIST IMPL>>>>
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


                    template    <typename   T>
                    class   DoublyLinkedListIterator;


                    template    <typename   T>
                    class   DoubleLink {
                    public:
                        DECLARE_USE_BLOCK_ALLOCATION (DoubleLink);
                    public:
                        DoubleLink (T item, DoubleLink<T>* next);

                    public:
                        T               fItem;
                        DoubleLink<T>*  fNext;
                    };


                    /*
                     *      DoublyLinkedList<T> is a generic link (non-intrusive) list implementation.
                     *  It keeps a length count so access to its length is rapid. We provide
                     *  no public means to access the links themselves.
                     *
                     *      Since this class provides no patching support, it is not generally
                     *  used - more likely you want to use DoublyLinkedList_Patch<T>. Use this if you
                     *  will manage all patching, or know that none is necessary.
                     */
                    template    <typename   T>
                    class   DoublyLinkedList {
                    public:
                        DoublyLinkedList ();
                        DoublyLinkedList (const DoublyLinkedList<T>& from);
                        ~DoublyLinkedList ();

                    public:
                        nonvirtual  DoublyLinkedList<T>& operator= (const DoublyLinkedList<T>& list);

                    public:
                        nonvirtual  size_t  GetLength () const;

                        /*
                         *      Basic access to the head of the list. This is what is most
                         *  commonly used, and is most efficient.
                         */
                    public:
                        nonvirtual  T       GetFirst () const;
                        nonvirtual  T       GetLast () const;

                        nonvirtual  void    Prepend (T item);
                        nonvirtual  void    RemoveFirst ();
                        nonvirtual  void    RemoveLast ();

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

                    protected:
                    public:         // To make this protected we need to write (without patch stuff) a mutator
                        size_t              fLength;
                    protected:
                    public:         // To make this protected we need to write (without patch stuff) a mutator
                        DoubleLink<T>*      fFirst;

                    protected:
#if     qDebug
                        virtual     void    Invariant_ () const;
#endif

                    private:
                        friend  class   DoublyLinkedListIterator<T>;
                    };


                    /*
                     *      DoublyLinkedListIterator<T> allows you to iterate over a DoublyLinkedList<T>. Its API
                     *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                     *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListMutator_Patch<T>
                     *  for that.
                     */
                    template    <class T>
                    class   DoublyLinkedListIterator {
                    public:
                        DoublyLinkedListIterator (const DoublyLinkedListIterator<T>& from);
                        DoublyLinkedListIterator (const DoublyLinkedList<T>& data);

                    public:
                        nonvirtual  DoublyLinkedListIterator<T>& operator= (const DoublyLinkedListIterator<T>& list);

                    public:
                        nonvirtual  bool    Done () const;
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  T       Current () const;

                        nonvirtual  void    Invariant () const;

                    protected:
                        const DoubleLink<T>*  fCurrent;
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
#include    "DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_ */
