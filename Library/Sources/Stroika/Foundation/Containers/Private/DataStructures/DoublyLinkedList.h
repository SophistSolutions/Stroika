/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_

/*
 *
 * TODO:
 *      @todo   MAJOR cleanup needed - nearly an entire rewrite. This code is very old and worn...
 *
 *      @todo   WARNING - NOT REAL DOUBLE LINKED LIST IMPL - REALLY SINGLE - SEE STROIKA CODE FOR REAL DOUBLE LINK LIST IMPL
 *
 *
 *
 *
 * Notes:
 *
 *          <<< WARNING - NOT REAL DOUBLE LINKED LIST IMPL - REALLY SINGLE - SEE STROIKA CODE FOR REAL DOUBLE LINK LIST IMPL>>>>
 *
 */


#include    "../../../StroikaPreComp.h"

#include    "../../../Configuration/Common.h"
#include    "../../../Memory/BlockAllocated.h"

#include    "../../Common.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   DataStructures {

                    template    <typename   T>  class   DoublyLinkedListIterator;
                    template    <typename   T>  class   DoublyLinkedListIterator_Patch;
                    template    <typename   T>  class   DoublyLinkedListMutator_Patch;

                    template    <typename   T>  
					class   DoubleLink {
					public:
						DECLARE_USE_BLOCK_ALLOCATION (DoubleLink);
                    public:
                        DoubleLink (T item, DoubleLink<T>* next);

                    public:
                        T				fItem;
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
                    template    <typename   T>  class   DoublyLinkedList {
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

                    protected:
                        size_t      fLength;
                        DoubleLink<T>*    fFirst;

#if     qDebug
                        virtual     void    Invariant_ () const;
#endif

                        friend  class   DoublyLinkedListIterator<T>;
                        friend  class   DoublyLinkedListIterator_Patch<T>;
                        friend  class   DoublyLinkedListMutator_Patch<T>;
                    };



                    /*
                     *      DoublyLinkedListIterator<T> allows you to iterate over a DoublyLinkedList<T>. Its API
                     *  is designed to make easy implemenations of subclasses of IteratorRep<T>.
                     *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListMutator_Patch<T>
                     *  for that.
                     */
                    template    <class T>   class   DoublyLinkedListIterator {
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






                    /*
                     *  Patching Support:
                     *
                     *      Here we provide Patching Versions of each iterator, and for convienience
                     *  versions of DoublyLinkedList that maintain a list of all Patching iterators.
                     */



                    /*
                     *      DoublyLinkedList_Patch<T> is a DoublyLinkedList<T> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of DoublyLinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template    <class T>   class   DoublyLinkedListIterator_Patch;
                    template    <typename   T>  class   DoublyLinkedList_Patch : public DoublyLinkedList<T> {
                    public:
                        DoublyLinkedList_Patch ();
                        DoublyLinkedList_Patch (const DoublyLinkedList_Patch<T>& from);
                        ~DoublyLinkedList_Patch ();

                        nonvirtual  DoublyLinkedList_Patch<T>& operator= (const DoublyLinkedList_Patch<T>& list);

                        /*
                         * Methods we shadow so that patching is done. If you want to circumvent the
                         * patching, thats fine - use scope resolution operator to call this's base
                         * class version.
                         */
                    public:
                        nonvirtual  void    Prepend (T item);
                        nonvirtual  void    Remove (T item);
                        nonvirtual  void    RemoveFirst ();
                        nonvirtual  void    RemoveAll ();

                    public:
                        nonvirtual  void    Append (T item);

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perfrom patching.
                         */
                    public:
                        nonvirtual  bool    HasActiveIterators () const;                    //  are there any iterators to be patched?
                        nonvirtual  void    PatchViewsAdd (const DoubleLink<T>* link) const;      //  call after add
                        nonvirtual  void    PatchViewsRemove (const DoubleLink<T>* link) const;   //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;                   //  call after removeall

                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                    public:
                        nonvirtual  void    Invariant () const;

                    protected:
                        DoublyLinkedListIterator_Patch<T>*    fIterators;

                        friend  class   DoublyLinkedListIterator_Patch<T>;
#if     qDebug
                        virtual     void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };




                    /*
                     *      DoublyLinkedListIterator_Patch<T> is a DoublyLinkedListIterator_Patch<T> that allows
                     *  for updates to the DoublyLinkedList<T> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the DoublyLinkedList_Patch<T>, and takes care
                     *  of all patching details.
                     */
                    template    <class T>   class   DoublyLinkedListIterator_Patch : public DoublyLinkedListIterator<T> {
                    public:
                        DoublyLinkedListIterator_Patch (const DoublyLinkedList_Patch<T>& data);
                        DoublyLinkedListIterator_Patch (const DoublyLinkedListIterator_Patch<T>& from);
                        ~DoublyLinkedListIterator_Patch ();

                        nonvirtual  DoublyLinkedListIterator_Patch<T>&    operator= (const DoublyLinkedListIterator_Patch<T>& rhs);

                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual  bool    More (T* current, bool advance);

                        nonvirtual  void    PatchAdd (const DoubleLink<T>* link);     //  call after add
                        nonvirtual  void    PatchRemove (const DoubleLink<T>* link);  //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        // Probably create subclass which tracks index internally, as with Stroika v1 but this will do for now
                        // and maybe best (depending on frequency of calls to current index
                        nonvirtual size_t CurrentIndex () const {
                            RequireNotNull (fData);
                            RequireNotNull (this->fCurrent);
                            size_t i = 0;
                            for (const DoubleLink<T>* l = fData->fFirst; l != this->fCurrent; l = l->fNext, ++i) {
                                AssertNotNull (l);
                            }
                            return i;
                        }

                    protected:
                        const DoublyLinkedList_Patch<T>*  fData;
                        DoublyLinkedListIterator_Patch<T>*    fNext;
                        const DoubleLink<T>*              fPrev;      // keep extra previous link for fast patchremove
                        // Nil implies fCurrent == fData->fFirst or its invalid,
                        // and must be recomputed (it was removed itself)...

                        friend  class   DoublyLinkedList_Patch<T>;
#if     qDebug
                        virtual void    Invariant_ () const override;
#endif
                    };




                    /*
                     *      DoublyLinkedListMutator_Patch<T> is a DoublyLinkedListIterator_Patch<T> that allows
                     *  for changes to the DoublyLinkedList<T> relative to the position we are at
                     *  currently in the iteration. This is be used most commonly in
                     *  implementing concrete iterators for Stroika containers.
                     */
                    template    <class T>   class   DoublyLinkedListMutator_Patch : public DoublyLinkedListIterator_Patch<T> {
                    public:
                        DoublyLinkedListMutator_Patch (DoublyLinkedList_Patch<T>& data);
                        DoublyLinkedListMutator_Patch (const DoublyLinkedListMutator_Patch<T>& from);

                        nonvirtual  DoublyLinkedListMutator_Patch<T>& operator= (DoublyLinkedListMutator_Patch<T>& rhs);

                        nonvirtual  void    RemoveCurrent ();
                        nonvirtual  void    UpdateCurrent (T newValue);
                        nonvirtual  void    AddBefore (T item);
                        nonvirtual  void    AddAfter (T item);
                    };

                }
            }
        }
    }
}



#endif  /*_Stroika_Foundation_Containers_Private_DataStructures_DoublyLinkedList_h_ */


/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DoublyLinkedList.inl"
