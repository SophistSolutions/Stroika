/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/DoublyLinkedList.h"



/*
 *
 * TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace Private {
                namespace   PatchingDataStructures {


                    /*
                     *  Patching Support:
                     *
                     *      Here we provide Patching Versions of each iterator, and for convienience
                     *  versions of DoublyLinkedList that maintain a list of all Patching iterators.
                     *
                     *      DoublyLinkedList_Patch<T> is a DoublyLinkedList<T> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of DoublyLinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template    <class T>
                    class   DoublyLinkedListIterator_Patch;
                    template    <typename   T>
                    class   DoublyLinkedList_Patch : public DataStructures::DoublyLinkedList<T> {
                    public:
                        DoublyLinkedList_Patch ();
                        DoublyLinkedList_Patch (const DoublyLinkedList_Patch<T>& from);
                        ~DoublyLinkedList_Patch ();

                    public:
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
                        nonvirtual  void    PatchViewsAdd (const DataStructures::DoubleLink<T>* link) const;      //  call after add
                        nonvirtual  void    PatchViewsRemove (const DataStructures::DoubleLink<T>* link) const;   //  call before remove
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
                    template    <class T>
                    class   DoublyLinkedListIterator_Patch : public DataStructures::DoublyLinkedListIterator<T> {
                    public:
                        DoublyLinkedListIterator_Patch (const DoublyLinkedList_Patch<T>& data);
                        DoublyLinkedListIterator_Patch (const DoublyLinkedListIterator_Patch<T>& from);
                        ~DoublyLinkedListIterator_Patch ();

                        nonvirtual  DoublyLinkedListIterator_Patch<T>&    operator= (const DoublyLinkedListIterator_Patch<T>& rhs);

                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual  bool    More (T* current, bool advance);

                        nonvirtual  void    PatchAdd (const DataStructures::DoubleLink<T>* link);     //  call after add
                        nonvirtual  void    PatchRemove (const DataStructures::DoubleLink<T>* link);  //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        // Probably create subclass which tracks index internally, as with Stroika v1 but this will do for now
                        // and maybe best (depending on frequency of calls to current index
                        nonvirtual size_t CurrentIndex () const {
                            RequireNotNull (fData);
                            RequireNotNull (this->fCurrent);
                            size_t i = 0;
                            for (const DataStructures::DoubleLink<T>* l = fData->fFirst; l != this->fCurrent; l = l->fNext, ++i) {
                                AssertNotNull (l);
                            }
                            return i;
                        }

                    protected:
                        const DoublyLinkedList_Patch<T>*        fData;
                        DoublyLinkedListIterator_Patch<T>*      fNext;
                        const DataStructures::DoubleLink<T>*                    fPrev;      // keep extra previous link for fast patchremove
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
                    template    <class T>
                    class   DoublyLinkedListMutator_Patch : public DoublyLinkedListIterator_Patch<T> {
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



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "DoublyLinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_ */
