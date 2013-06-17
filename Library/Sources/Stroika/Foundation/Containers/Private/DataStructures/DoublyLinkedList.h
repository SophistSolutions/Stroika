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
 *              (NO MUTATOR OBJECT - INSTEAD - UPDATECURRENT (ITERATOR) MEHTOD ON OBJECT ITSELF.
 *
 *      @todo   Major cleanup needed - not doubly-linked list. Look at old Stroika code. Somehow the double link
 *              part got lost.
 *
 *
 * Notes:
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {


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
                        nonvirtual  bool    IsEmpty () const;

                    public:
                        nonvirtual  size_t  GetLength () const;

                        /*
                         *      Basic access to the head of the list. This is what is most
                         *  commonly used, and is most efficient.
                         */
                    public:
                        nonvirtual  T       GetFirst () const;
                        nonvirtual  T       GetLast () const;

                    public:
                        nonvirtual  void    Prepend (T item);
                        nonvirtual  void    RemoveFirst ();
                        nonvirtual  void    RemoveLast ();

                    public:
                        /*
                         *  Utility to search the list for the given item using operator==
                         */
                        nonvirtual  bool    Contains (T item) const;

                    public:
                        /**
                         *  Note - does nothing if item not found.
                         */
                        nonvirtual  void    Remove (T item);

                    public:
                        nonvirtual  void    RemoveAll ();

                        /*
                         *      Not alot of point in having these, as they are terribly slow,
                         *  but the could be convienient.
                         */
                    public:
                        nonvirtual  T       GetAt (size_t i) const;
                        nonvirtual  void    SetAt (size_t i, T item);

                    public:
                        nonvirtual  void    Invariant () const;

                    public:
                        // for now public... but soon protected - just for helper iterator classes...
                        class   Link;

                    public:
                        class   ForwardIterator;

                    protected:
                        Link*      _fFirst;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const;
#endif

                    private:
                        friend  class   ForwardIterator;
                    };


                    /*
                     *  for now public... but soon protected - just for helper iterator classes...
                     */
                    template    <typename   T>
                    class   DoublyLinkedList<T>::Link {
                    public:
                        DECLARE_USE_BLOCK_ALLOCATION (Link);
                    public:
                        Link (T item, Link* next);

                    public:
                        T       fItem;
                        Link*   fNext;
                    };


                    /*
                     *      ForwardIterator<T> allows you to iterate over a DoublyLinkedList<T>. Its API
                     *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                     *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListMutator_Patch<T>
                     *  for that.
                     */
                    template    <typename   T>
                    class   DoublyLinkedList<T>::ForwardIterator {
                    public:
                        ForwardIterator (const ForwardIterator& from);
                        ForwardIterator (const DoublyLinkedList<T>& data);

                    public:
                        typedef typename DoublyLinkedList<T>::Link    Link;

                    public:
                        nonvirtual  ForwardIterator& operator= (const ForwardIterator& list);

                    public:
                        nonvirtual  bool    Done () const;
                        nonvirtual  bool    More (T* current, bool advance);
                        nonvirtual  T       Current () const;

                    public:
                        nonvirtual  void    Invariant () const;

                    protected:
                        static  Link*       _GetFirstDataLink (DoublyLinkedList<T>* data);
                        static  const Link* _GetFirstDataLink (const DoublyLinkedList<T>* data);
                        static  void        _SetFirstDataLink (DoublyLinkedList<T>* data, Link* newFirstLink);

                    protected:
                        const Link*     fCurrent;
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
