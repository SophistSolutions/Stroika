/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_PriorityQueue_h_
#define _Stroika_Foundation_Containers_PriorityQueue_h_ 1

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Memory/SharedByValue.h"
#include "Stroika/Foundation/Traversal/Iterable.h"

/**
 *
 *  STATUS: NOT EVEN DRAFT IMPL READY YET TO TEST.
 *
 *  \note Code-Status:  <a href="Code-Status.md#Draft">Draft</a> -- CODE NO WHERE NEAR COMPILING - just rough draft of API based on 1992 Stroika...
 *
 *
 *  TODO:
 *
 *      @todo   Use TRAITS mechanism - like with Bag<>
 *
 *      @todo   Implement first draft of code based on
 *              http://github.com/SophistSolutions/Stroika/blob/master/Archive/Stroika_FINAL_for_STERL_1992/Library/Foundation/Headers/PriorityQueue.hh
 *
 *      @todo   FIX so can remove top prioty itme to return btoih at a time.
 *
 *      @todo   Add Traits, and part of triats is priority type.
 *              min/max comes from numeric_limits<T>::min/max, and memmbers of the traits - not global
 *
 *      @todo   Rename PQEntry to PriorityQueueEntry
 *              (document why not nested type, and why not using pair<> - and then put docs into MultiSet code,
 *              and then redo Mapping to use KeyValuePair<> instead of pair<> - and have .fKey first elt!
 *
 *      @todo   redo operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs); as member function
 */

namespace Stroika::Foundation::Containers {

    using Traversal::Iterable;
    using Traversal::Iterator;

    using Priority              = uint16_t;
    const Priority kMinPriority = kMinUInt16;
    const Priority kMaxPriority = kMaxUInt16;

    // Someday this should be renamed ...
    template <typename T>
    class PQEntry {
    public:
        PQEntry (T item, Priority p);

        T        fItem;
        Priority fPriority;
    };
    template <typename T>
    Boolean operator== (const PQEntry<T>& lhs, const PQEntry<T>& rhs);

    /*
     *  PriorityQueues are a like a Queue that allows retrieval based the priority assigned an item.
     *  This priority is given either at the time when the item is Enqueueed to the PriorityQueue, or
     *  by a function. The default function always assigns the lowest possible priority to an item.
     *  Priority start at zero and work upwards, so a zero priority item will be the last item
     *  removed from the PriorityQueue.
     * 
     *  PriorityQueues support two kinds of iteration: over type T, or over ProirityQueueEntrys of
     *  type T. A PriorityQueueEntry is a simple structure that couples together the item and its
     *  priority.
     *
     *
     *  PriorityQueues always iterate from highest to lowest priority.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   See <a href="./ReadMe.md">ReadMe.md</a> for common features of all Stroika containers (especially
     *          constructors, iterators, etc)
     *
     *
     */
    template <typename T>
    class [[nodiscard]] PriorityQueue : public Iterable<pair<T, Priority>> {
    protected:
        class _IRep;

    public:
        /**
         *  Use this typedef in templates to recover the basic functional container pattern of concrete types.
         */
        using ArchetypeContainerType = PriorityQueue;

    public:
        /**
         */
        PriorityQueue ();
        PriorityQueue (const PriorityQueue& src) = default;

    protected:
        explicit PriorityQueue (const shared_ptr<_IRep>& rep);

#if qDebug
    public:
        ~PriorityQueue ();
#endif

    public:
        /**
         */
        nonvirtual PriorityQueue<T>& operator= (const PriorityQueue<T>& src);
        nonvirtual PriorityQueue<T>& operator= (PriorityQueue<T>&& rhs) = default;

    public:
        nonvirtual void RemoveAll ();

    public:
        nonvirtual Iterable<T> Elements () const;

    public:
        nonvirtual void Enqueue (T item, Priority priority);

    public:
        nonvirtual T Dequeue ();

    public:
        nonvirtual T Head () const;

    public:
        nonvirtual Priority TopPriority () const;

    public:
        nonvirtual void RemoveHead ();

    public:
        nonvirtual PriorityQueue<T>& operator+= (T item);
        nonvirtual PriorityQueue<T>& operator+= (IteratorRep<PQEntry<T>>* it);
        nonvirtual PriorityQueue<T>& operator-- ();

    protected:
        nonvirtual const _IRep& _ConstGetRep () const;

    protected:
        nonvirtual const _IRep& _GetRep () const;
        nonvirtual _IRep&       _GetRep ();
    };

    /**
     *  \brief  Implementation detail for PriorityQueue<T> implementors.
     *
     *  Protected abstract interface to support concrete implementations of
     *  the PriorityQueue<T> container API.
     */
    template <typename T>
    class PriorityQueue<T>::_IRep : public Iterable<pair<T, Priority>>::_IRep {
    protected:
        _IRep ();

    public:
        virtual ~_IRep ();

    public:
        virtual shared_ptr<_IRep> CloneEmpty () const                 = 0;
        virtual void              Enqueue (T item, Priority priority) = 0;
        virtual T                 Dequeue ()                          = 0;
        virtual T                 Head () const                       = 0;
        virtual Iterable<T>       Elements () const                   = 0;
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

#include "PriorityQueue.inl"

#endif /*_Stroika_Foundation_Containers_PriorityQueue_h_ */
