/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_

#include "../../../StroikaPreComp.h"

#include "../../../Memory/SmallStackBuffer.h"
#include "../../../Traversal/Iterator.h"

#include "../../DataStructures/LinkedList.h"

#include "PatchableContainerHelper.h"

/**
 *
 * TODO:
 *      @todo   Track 'prev' in iterator, for greater performance on addbefore (cacehd prevptr)
 *
 */

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Private {
                namespace PatchingDataStructures {

                    using Traversal::IteratorOwnerID;

                    /*
                     *  Patching Support:
                     *
                     *      This class wraps a basic container (in this case DataStructures::LinkedList)
                     *  and adds in patching support (tracking a list of iterators - and managing thier
                     *  patching when appropriately wrapped changes are made to the data structure container.
                     *
                     *      This code leverages PatchableContainerHelper<> to do alot of the book-keeping.
                     *
                     *      This code is NOT threadsafe. It assumes a wrapper layer provides thread safety, but it
                     *  DOES provide 'deletion'/update safety.
                     *
                     *      Note: Disallow X(const X&), and operator=() (copy constructor/assignement operator), and
                     *  instead require use of X(X*,IteratorOwnerID) for copying - so we always get both values -
                     *  the source to copy from and the newOwnerID to copy INTO.
                     */
                    template <typename T, typename TRAITS = DataStructures::LinkedList_DefaultTraits<T>>
                    class LinkedList : public PatchableContainerHelper<DataStructures::LinkedList<T, TRAITS>> {
                    private:
                        using inherited = PatchableContainerHelper<DataStructures::LinkedList<T, TRAITS>>;

                    public:
                        LinkedList ();
                        LinkedList (LinkedList<T, TRAITS>* rhs, IteratorOwnerID newOwnerID);
                        LinkedList (const LinkedList<T, TRAITS>& from) = delete;

                    public:
                        nonvirtual LinkedList<T, TRAITS>& operator= (const LinkedList<T, TRAITS>& list) = delete;

                    public:
                        using Link = typename DataStructures::LinkedList<T, TRAITS>::Link;

                    public:
                        class ForwardIterator;

                    public:
                        using UnpatchedForwardIterator = typename inherited::ForwardIterator;

                        /*
                         * Methods to do the patching yourself. Iterate over all the iterators and
                         * perfrom patching.
                         */
                    public:
                        nonvirtual void PatchViewsAdd (const Link* link) const;    //  call after add
                        nonvirtual void PatchViewsRemove (const Link* link) const; //  call before remove
                        nonvirtual void PatchViewsRemoveAll () const;              //  call after removeall

                    public:
                        nonvirtual void TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch) const;
                        nonvirtual void TwoPhaseIteratorPatcherPass2 (const Memory::SmallStackBuffer<ForwardIterator*>* items2Patch, Link* newI) const;

                        /*
                         * Methods we shadow so that patching is done. If you want to circumvent the
                         * patching, thats fine - use scope resolution operator to call this's base
                         * class version.
                         */
                    public:
                        nonvirtual void Prepend (T item);
                        template <typename EQUALS_COMPARER = typename TRAITS::EqualsCompareFunctionType::NEW_EQUALS_COMPARER>
                        nonvirtual void Remove (T item, const EQUALS_COMPARER& equalsComparer = EQUALS_COMPARER{});
                        nonvirtual void RemoveFirst ();
                        nonvirtual void RemoveAll ();
                        nonvirtual void Append (T item);
                        nonvirtual void RemoveAt (const ForwardIterator& i);
                        nonvirtual void AddBefore (const ForwardIterator& i, T newValue);
                        nonvirtual void AddAfter (const ForwardIterator& i, T newValue);

                    public:
                        /*
                         *  Check Invariants for this class, and all the iterators we own.
                         */
                        nonvirtual void Invariant () const;

#if qDebug
                    protected:
                        virtual void    Invariant_ () const override;
                        nonvirtual void InvariantOnIterators_ () const;
#endif

                    private:
                        friend class ForwardIterator;
                    };

                    /*
                     *      LinkedList<T, TRAITS>::ForwardIterator is a ForwardIterator that allows
                     *  for updates to the LinkedList<T,TRAITS> to be dealt with properly. It maintains a
                     *  link list of iterators headed by the LinkedList<T, TRAITS>, and takes care
                     *  of all patching details.
                     *
                     *  \note   Subtle - but PatchableIteratorMixIn must come last in bases so it gets constructed (added to list of patchable stuff) after
                     *          and removed before destruction of other bases
                     */
                    template <typename T, typename TRAITS>
                    class LinkedList<T, TRAITS>::ForwardIterator
                        : public DataStructures::LinkedList<T, TRAITS>::ForwardIterator,
                          public PatchableContainerHelper<DataStructures::LinkedList<T, TRAITS>>::PatchableIteratorMixIn {
                    private:
                        using inherited_DataStructure = typename DataStructures::LinkedList<T, TRAITS>::ForwardIterator;
                        using inherited_PatchHelper   = typename PatchableContainerHelper<DataStructures::LinkedList<T, TRAITS>>::PatchableIteratorMixIn;

                    public:
                        ForwardIterator (IteratorOwnerID ownerID, const LinkedList<T, TRAITS>* data);
                        ForwardIterator (const ForwardIterator& from);

                    public:
                        ~ForwardIterator ();

                    public:
                        nonvirtual ForwardIterator& operator= (const ForwardIterator& rhs) = delete;

                    public:
                        using ContainerType = PatchingDataStructures::LinkedList<T, TRAITS>;
                        using Link          = typename ContainerType::Link;

                    public:
                        /*
                         * Shadow more to keep track of prev.
                         */
                        nonvirtual void PatchAdd (const Link* link);    //  call after add
                        nonvirtual void PatchRemove (const Link* link); //  call before remove
                        nonvirtual void PatchRemoveAll ();              //  call after removeall

                        nonvirtual void TwoPhaseIteratorPatcherPass1 (Link* oldI, Memory::SmallStackBuffer<ForwardIterator*>* items2Patch);
                        nonvirtual void TwoPhaseIteratorPatcherPass2 (Link* newI);

                    protected:
                        //const Link*              fPrev;      // keep extra previous link for fast patchremove
                        // Nil implies fCurrent == fData->fFirst or its invalid,
                        // and must be recomputed (it was removed itself)...

#if qDebug
                    protected:
                        virtual void Invariant_ () const override;
#endif

                    private:
                        friend class LinkedList<T, TRAITS>;
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
#include "LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_Private_PatchingDataStructures_LinkedList_h_ */
