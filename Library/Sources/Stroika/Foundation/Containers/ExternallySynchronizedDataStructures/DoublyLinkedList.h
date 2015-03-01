/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_h_

#include    "../../StroikaPreComp.h"

#include    "../../Configuration/Common.h"
#include    "../../Common/Compare.h"
#include    "../../Memory/BlockAllocated.h"
#include    "../../Memory/Optional.h"

#include    "../Common.h"



/*
 *
 * TODO:
 *
 *      @todo   DataStructures::DoublyLinkedList::ForwardIterator has the 'suporesMode' in the
 *              datastrcutre code and we have it here in the patching code. Note SURE what is better
 *              probably patching code) - but make them consistent!
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Major cleanup needed - not doubly-linked list. Look at old Stroika code. Somehow the double link
 *              part got lost.
 *
 *  Long-Term TODO:
 *      @todo   Could add iteartor subclass (or use traits to control) which tracks index internally, as with Stroika v1
 *              but this will do for and maybe best (depending on frequency of calls to CurrentIndex ()
 *
 * Notes:
 *
 */


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   ExternallySynchronizedDataStructures {


                /**
                 * VERY PRELIMINARY DRAFT OF HOW TO HANDLE THIS - UNSURE ABOUT ISSUE OF FORWARDABILITY AND COPYABILIUTY OF COMPARERES!!!!
                 */
                template    <typename T, typename EQUALS_COMPARER = Common::ComparerWithEqualsOptionally <T>>
                struct   DoublyLinkedList_DefaultTraits {
                    using   EqualsCompareFunctionType       =   EQUALS_COMPARER;
                };


                /*
                 *      DoublyLinkedList<T, TRAITS> is a generic link (non-intrusive) list implementation.
                 *  It keeps a length count so access to its length is rapid. We provide
                 *  no public means to access the links themselves.
                 *
                 *      Since this class provides no patching support, it is not generally
                 *  used - more likely you want to use DoublyLinkedList<T, TRAITS>. Use this if you
                 *  will manage all patching, or know that none is necessary.
                 */
                template      <typename  T, typename TRAITS = DoublyLinkedList_DefaultTraits<T>>
                class   DoublyLinkedList {
                public:
                    using   value_type  =   T;

                public:
                    DoublyLinkedList ();
                    DoublyLinkedList (const DoublyLinkedList<T, TRAITS>& from);
                    ~DoublyLinkedList ();

                public:
                    nonvirtual  DoublyLinkedList<T, TRAITS>& operator= (const DoublyLinkedList<T, TRAITS>& list);

                public:
                    class   Link;

                public:
                    nonvirtual  bool    IsEmpty () const;

                public:
                    nonvirtual  size_t  GetLength () const;

                public:
                    /**
                     *  Efficient.
                     *
                     *  \req not IsEmpty ()
                     */
                    nonvirtual  T       GetFirst () const;

                public:
                    /**
                     *  Efficient:      ?? depedns on if I store last link?
                     *
                     *  \req not IsEmpty ()
                     */
                    nonvirtual  T       GetLast () const;

                public:
                    /**
                     *  Efficient.
                     */
                    nonvirtual  void    Prepend (T item);

                public:
                    /**
                     *  Efficient.
                     *
                     *  \req not IsEmpty ()
                     */
                    nonvirtual  void    RemoveFirst ();

                public:
                    /**
                     *  Efficient.
                     *
                     *  \req not IsEmpty ()
                     */
                    nonvirtual  void    RemoveLast ();

                public:
                    /*
                     *  Performance:
                     *      Worst Case: O(N)
                     *      Average Case: O(N)
                     *
                     *  Utility to search the list for the given item using operator==
                     */
                    nonvirtual  bool    Contains (T item) const;

                public:
                    /*
                     */
                    template    <typename FUNCTION>
                    nonvirtual  void    Apply (FUNCTION doToElement) const;
                    template    <typename FUNCTION>
                    nonvirtual  Link*   FindFirstThat (FUNCTION doToElement) const;

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(N)
                     *      Average Case: O(N)
                     *
                     *  Note - does nothing if item not found.
                     */
                    nonvirtual  void    Remove (T item);

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(N)
                     *      Average Case: O(N)
                     */
                    nonvirtual  void    RemoveAll ();

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(N)
                     *      Average Case: O(N)
                     */
                    nonvirtual  T       GetAt (size_t i) const;

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(N)
                     *      Average Case: O(N)
                     */
                    nonvirtual  void    SetAt (size_t i, T item);

                public:
                    class   ForwardIterator;

                public:
                    using   IteratorBaseType    =   ForwardIterator;
                    nonvirtual  void    MoveIteratorHereAfterClone (IteratorBaseType* pi, const DoublyLinkedList<T, TRAITS>* movedFrom);

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(1)
                     */
                    nonvirtual  void    RemoveAt (const ForwardIterator& i);

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(1)
                     */
                    nonvirtual  void    SetAt (const ForwardIterator& i, T newValue);

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(1)
                     *
                     *  NB: Can be called if done
                     */
                    nonvirtual  void    AddBefore (const ForwardIterator& i, T item);

                public:
                    /**
                     *  Performance:
                     *      Worst Case: O(1)
                     */
                    nonvirtual  void    AddAfter (const ForwardIterator& i, T item);

                public:
                    nonvirtual  void    Invariant () const;

                protected:
                    Link*      _fHead;

#if     qDebug
                protected:
                    virtual     void    Invariant_ () const;
#endif

                private:
                    friend  class   ForwardIterator;
                };


                /**
                 *  Just an implementation detail. Don't use directly except in helper classes.
                 */
                template      <typename  T, typename TRAITS>
                class   DoublyLinkedList<T, TRAITS>::Link {
                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Link);
                public:
                    Link (T item, Link* next);

                public:
                    T       fItem;
                    Link*   fNext;
                };


                /**
                 *      ForwardIterator<T> allows you to iterate over a DoublyLinkedList<T, TRAITS>. Its API
                 *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                 *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListIterator_Patch<T>
                 *  for that.
                 */
                template      <typename  T, typename TRAITS>
                class   DoublyLinkedList<T, TRAITS>::ForwardIterator {
                public:
                    ForwardIterator (const ForwardIterator& from);
                    ForwardIterator (const DoublyLinkedList<T, TRAITS>* data);

                public:
                    using   Link    =   typename DoublyLinkedList<T, TRAITS>::Link;

                public:
                    nonvirtual  ForwardIterator& operator= (const ForwardIterator& list);

                public:
                    nonvirtual  bool    Done () const;
                    nonvirtual  bool    More (T* current, bool advance);
                    nonvirtual  void    More (Memory::Optional<T>* result, bool advance);
                    nonvirtual  bool    More (nullptr_t, bool advance);
                    nonvirtual  T       Current () const;

                public:
                    // Warning - intrinsically slow
                    nonvirtual  size_t  CurrentIndex () const;

                public:
                    nonvirtual  void    SetCurrentLink (Link* l);

                public:
                    nonvirtual  bool    Equals (const typename DoublyLinkedList<T, TRAITS>::ForwardIterator& rhs) const;

                public:
                    nonvirtual  void    Invariant () const;

                protected:
                    static  Link*       _GetFirstDataLink (DoublyLinkedList<T, TRAITS>* data);
                    static  const Link* _GetFirstDataLink (const DoublyLinkedList<T, TRAITS>* data);
                    static  void        _SetFirstDataLink (DoublyLinkedList<T, TRAITS>* data, Link* newFirstLink);

                protected:
                public: /// TEMPORARILY MAKE PUBLIC SO ACCESSIBLE IN ``<> - until those cleaned up a bit
                    const DoublyLinkedList<T, TRAITS>*  _fData;
                    const Link*                         _fCurrent;
                    bool                                _fSuppressMore;  // Indicates if More should do anything, or if were already Mored...

#if     qDebug
                protected:
                    virtual void    Invariant_ () const;
#endif

                private:
                    friend  class   DoublyLinkedList<T, TRAITS>;
                };


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

#endif  /*_Stroika_Foundation_Containers_ExternallySynchronizedDataStructures_DoublyLinkedList_h_ */
