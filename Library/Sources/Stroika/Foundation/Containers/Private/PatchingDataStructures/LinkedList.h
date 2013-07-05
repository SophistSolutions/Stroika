/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_

#include    "../../../StroikaPreComp.h"

#include    "../../../Memory/SmallStackBuffer.h"

#include    "../DataStructures/LinkedList.h"



/**
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
                     *  versions of LinkedList that maintain a list of all Patching iterators.
                     *
                     *      LinkedList<T, TRAITS> is a LinkedList<T,TRAITS> with the ability to keep track of
                     *  owned patching iterators. These patching iterators will automatically be
                     *  adjusted when the link list is adjusted. This is the class of LinkedList
                     *  most likely to be used in implementing a concrete container class.
                     */
                    template      <typename  T, typename TRAITS = DataStructures::LinkedList_DefaultTraits<T>>
                    class   LinkedList : public DataStructures::LinkedList<T, TRAITS> {
                    private:
                        typedef typename DataStructures::LinkedList<T, TRAITS>  inherited;

                    public:
                        LinkedList ();
                        LinkedList (const LinkedList<T, TRAITS>& from);
                        ~LinkedList ();

                    public:
                        nonvirtual  LinkedList<T, TRAITS>& operator= (const LinkedList<T, TRAITS>& list);

                    public:
                        typedef typename DataStructures::LinkedList<T, TRAITS>::Link    Link;

                    public:
                        class   ForwardIterator;

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
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI);

                    public:
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
                        nonvirtual  void    RemoveAt (const ForwardIterator& i);
                        nonvirtual  void    AddBefore (const ForwardIterator& i, T newValue);
                        nonvirtual  void    AddAfter (const ForwardIterator& i, T newValue);

                    public:
                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                        nonvirtual  void    Invariant () const;

                    protected:
                        ForwardIterator*    fActiveIteratorsListHead_;

                    private:
                        friend  class   ForwardIterator;

#if     qDebug
                    protected:
                        virtual     void    Invariant_ () const override;
                        nonvirtual  void    InvariantOnIterators_ () const;
#endif
                    };


                    /*
                     *      LinkedList<T, TRAITS>::ForwardIterator is a ForwardIterator that allows
                     *  for updates to the LinkedList<T,TRAITS> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the LinkedList<T, TRAITS>, and takes care
                     *  of all patching details.
                     */
                    template      <typename  T, typename TRAITS>
                    class   LinkedList<T, TRAITS>::ForwardIterator : public DataStructures::LinkedList<T, TRAITS>::ForwardIterator {
                    private:
                        typedef typename DataStructures::LinkedList<T, TRAITS>::ForwardIterator   inherited;

                    public:
                        ForwardIterator (const LinkedList<T, TRAITS>& data);
                        ForwardIterator (const ForwardIterator& from);
                        ~ForwardIterator ();

                        nonvirtual  ForwardIterator&    operator= (const ForwardIterator& rhs);

                    public:
                        typedef typename DataStructures::LinkedList<T, TRAITS>::Link    Link;
                        typedef PatchingDataStructures::LinkedList<T, TRAITS>           ContainerType;

                    public:
                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual  bool    More (T* current, bool advance);

                        nonvirtual  void    PatchAdd (const Link* link);     //  call after add
                        nonvirtual  void    PatchRemove (const Link* link);  //  call before remove
                        nonvirtual  void    PatchRemoveAll ();                  //  call after removeall

                        nonvirtual  void    TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch);
                        nonvirtual  void    TwoPhaseIteratorPatcherPass2 (Link* newI);

                    private:
                        nonvirtual  const ContainerType&  GetPatchingContainer_ () const;
                        nonvirtual  ContainerType&    GetPatchingContainer_ ();

                    protected:
                        ForwardIterator*            _fNextActiveIterator;
                        //const Link*              fPrev;      // keep extra previous link for fast patchremove
                        // Nil implies fCurrent == fData->fFirst or its invalid,
                        // and must be recomputed (it was removed itself)...

                    private:
                        friend  class   LinkedList<T, TRAITS>;
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
#include    "LinkedList.inl"

#endif  /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_ */
