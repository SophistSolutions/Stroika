/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"

#include    "../DataStructures/DoublyLinkedList.h"




/*
 *
 * TODO:
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
                     *      PatchingDataStructures::DoublyLinkedList<T, TRAITS> is a DoublyLinkedList<T, TRAITS> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of DoublyLinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template      <typename  T, typename TRAITS = DataStructures::DoublyLinkedList_DefaultTraits<T>>
                    class   DoublyLinkedList : public DataStructures::DoublyLinkedList<T, TRAITS> {
                    private:
                        typedef typename DataStructures::DoublyLinkedList<T, TRAITS> inherited;

                    public:
                        DoublyLinkedList ();
                        DoublyLinkedList (const DoublyLinkedList<T, TRAITS>& from);
                        ~DoublyLinkedList ();

                    public:
                        nonvirtual  DoublyLinkedList<T, TRAITS>& operator= (const DoublyLinkedList<T, TRAITS>& rhs);

                    public:
                        typedef typename DataStructures::DoublyLinkedList<T, TRAITS>::Link    Link;

                    public:
                        class   ForwardIterator;

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

                    public:
                        nonvirtual  void    RemoveAt (const ForwardIterator& i);
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T newValue);
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T newValue);

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perfrom patching.
                         */
                    public:
                        nonvirtual  bool    HasActiveIterators () const;                    //  are there any iterators to be patched?
                        nonvirtual  void    PatchViewsAdd (const Link* link) const;         //  call after add
                        nonvirtual  void    PatchViewsRemove (const Link* link) const;      //  call before remove
                        nonvirtual  void    PatchViewsRemoveAll () const;                   //  call after removeall

                    public:
                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const;
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const;

                    public:
                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                        nonvirtual  void    Invariant () const;

                    protected:
                        ForwardIterator*    fActiveIteratorsListHead_;

                    protected:
                        friend  class   ForwardIterator;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };


                    /*
                     *      ForwardIterator is a .... that allows
                     *  for updates to the DoublyLinkedList<T, TRAITS> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the DoublyLinkedList<T, TRAITS>, and takes care
                     *  of all patching details.
                     */
                    template      <typename  T, typename TRAITS>
                    class   DoublyLinkedList<T, TRAITS>::ForwardIterator : public DataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator {
                    private:
                        typedef typename DataStructures::DoublyLinkedList<T, TRAITS>::ForwardIterator inherited;

                    public:
                        ForwardIterator (const DoublyLinkedList<T, TRAITS>* data);
                        ForwardIterator (const ForwardIterator& from);
                        ~ForwardIterator ();

                    public:
                        nonvirtual  ForwardIterator&    operator= (const ForwardIterator& rhs);

                    public:
                        typedef typename DataStructures::DoublyLinkedList<T, TRAITS>::Link    Link;
                        typedef PatchingDataStructures::DoublyLinkedList<T, TRAITS>           ContainerType;

#if 0
                    public:
                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual  bool    More (T* current, bool advance);
#endif

                    public:
                        nonvirtual  void    PatchAdd (const Link* link);        //  call after add
                        nonvirtual  void    PatchRemove (const Link* link);     //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch);
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (Link* newI);

                    private:
                        nonvirtual  const ContainerType&    GetPatchingContainer_ () const;
                        nonvirtual  ContainerType&          GetPatchingContainer_ ();

                    private:
                        ForwardIterator*    fNextActiveIterator_;

                    private:
                        friend  class   DoublyLinkedList<T, TRAITS>;

#if     qDebug
                    protected:
                        virtual void    Invariant_ () const override;
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

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_DoublyLinkedList_h_ */
