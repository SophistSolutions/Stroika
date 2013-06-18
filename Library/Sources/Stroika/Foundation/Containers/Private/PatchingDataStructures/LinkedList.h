/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../DataStructures/LinkedList.h"


/**
 *
 * TODO:
 *      @todo   GET RID OF LinkedListMutator_Patch - as we did for DoublyLinkedList..
 *
 *      @todo   THEN - redo LinkedList_Patching<T>::ForwardIterator as nested... then more cleanups...
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
                     *  versions of LinkedList that maintain a list of all Patching iterators.
                     *
                     *      LinkedList_Patch<T> is a LinkedList<T> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of LinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template    <typename   T>
                    class   LinkedListIterator_Patch;
                    template    <typename   T>
                    class   LinkedList_Patch : public DataStructures::LinkedList<T> {
                    private:
                        typedef typename DataStructures::LinkedList<T>  inherited;

                    public:
                        LinkedList_Patch ();
                        LinkedList_Patch (const LinkedList_Patch<T>& from);
                        ~LinkedList_Patch ();

                        nonvirtual  LinkedList_Patch<T>& operator= (const LinkedList_Patch<T>& list);

                    public:
                        typedef typename DataStructures::LinkedList<T>::Link    Link;

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
                        nonvirtual  void    Append (T item);

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perfrom patching.
                         */
                    public:
                        nonvirtual  bool    HasActiveIterators () const;                    //  are there any iterators to be patched?
                        nonvirtual  void    PatchViewsAdd (const Link* link) const;      //  call after add
                        nonvirtual  void    PatchViewsRemove (const Link* link) const;   //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;                   //  call after removeall

                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                    public:
                        nonvirtual  void    Invariant () const;

                    protected:
                        LinkedListIterator_Patch<T>*    fIterators;

                        friend  class   LinkedListIterator_Patch<T>;
#if     qDebug
                        virtual     void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };


                    /*
                     *      LinkedListIterator_Patch<T> is a LinkedListIterator_Patch<T> that allows
                     *  for updates to the LinkedList<T> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the LinkedList_Patch<T>, and takes care
                     *  of all patching details.
                     */
                    template    <typename   T>
                    class   LinkedListIterator_Patch : public DataStructures::LinkedList<T>::ForwardIterator {
                    private:
                        typedef typename DataStructures::LinkedList<T>::ForwardIterator   inherited;

                    public:
                        LinkedListIterator_Patch (const LinkedList_Patch<T>& data);
                        LinkedListIterator_Patch (const LinkedListIterator_Patch<T>& from);
                        ~LinkedListIterator_Patch ();

                        nonvirtual  LinkedListIterator_Patch<T>&    operator= (const LinkedListIterator_Patch<T>& rhs);

                    public:
                        typedef typename DataStructures::LinkedList<T>::Link    Link;

                    public:
                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual  bool    More (T* current, bool advance);

                        nonvirtual  void    PatchAdd (const Link* link);     //  call after add
                        nonvirtual  void    PatchRemove (const Link* link);  //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        // Probably create subclass which tracks index internally, as with Stroika v1 but this will do for now
                        // and maybe best (depending on frequency of calls to current index
                        nonvirtual size_t CurrentIndex () const {
                            RequireNotNull (fData);
                            RequireNotNull (this->fCurrent);
                            size_t i = 0;
                            for (const Link* l = fData->fFirst; l != this->fCurrent; l = l->fNext, ++i) {
                                AssertNotNull (l);
                            }
                            return i;
                        }

                    protected:
                        const LinkedList_Patch<T>*  fData;
                        LinkedListIterator_Patch<T>*    fNext;
                        const Link*              fPrev;      // keep extra previous link for fast patchremove
                        // Nil implies fCurrent == fData->fFirst or its invalid,
                        // and must be recomputed (it was removed itself)...

                        friend  class   LinkedList_Patch<T>;
#if     qDebug
                        virtual void    Invariant_ () const override;
#endif
                    };


                    /*
                     *      LinkedListMutator_Patch<T> is a LinkedListIterator_Patch<T> that allows
                     *  for changes to the LinkedList<T> relative to the position we are at
                     *  currently in the iteration. This is be used most commonly in
                     *  implementing concrete iterators for Stroika containers.
                     */
                    template    <typename   T>
                    class   LinkedListMutator_Patch : public LinkedListIterator_Patch<T> {
                    public:
                        LinkedListMutator_Patch (LinkedList_Patch<T>& data);
                        LinkedListMutator_Patch (const LinkedListMutator_Patch<T>& from);

                    public:
                        nonvirtual  LinkedListMutator_Patch<T>& operator= (LinkedListMutator_Patch<T>& rhs);

                    public:
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
#include    "LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_ */
