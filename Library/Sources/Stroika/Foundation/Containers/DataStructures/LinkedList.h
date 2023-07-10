/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2023.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_LinkedList_h_

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Configuration/TypeHints.h"
#include "../../Debug/AssertExternallySynchronizedMutex.h"
#include "../../Memory/BlockAllocated.h"

#include "../Common.h"

/**
 *
 *
 *      LinkedList<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *
 * TODO:
 *      @todo   In ForwardIterator object - maintain cached prev - so as we navigate - we can often
 *              avoid the back nav. Maybe make this a configurable class option? Anyhow - mostly include
 *              and use as cahce. ALready there mostly - but commented out (fPrev)
 *
 *      @todo   Include Performance numbers for each operation (done for many).
 *
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
     *      LinkedList<T,TRAITS> is a generic link (non-intrusive) list implementation.
     *   We provide
     *  no public means to access the links themselves.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class LinkedList : public Debug::AssertExternallySynchronizedMutex {
    public:
        using value_type = T;

    public:
        LinkedList ();
        LinkedList (const LinkedList& from);
        ~LinkedList ();

    public:
        nonvirtual LinkedList& operator= (const LinkedList& list);

    public:
        class ForwardIterator;

    public:
    private:
        class Link_;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = const Link_*;

    public:
        /*
         *  Take iteartor 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iteartor from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList* movedFrom) const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual T GetFirst () const;

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void Prepend (ArgByValueType<T> item);

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Returns pointer to T (or nullptr if not found). Lifetime of T* only til next call on this.
         *
         *  Contains(T item) == Lookup (item) != nullptr;
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual const T* Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {}) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual T* Lookup (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {});

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <typename FUNCTION>
        nonvirtual UnderlyingIteratorRep Find (FUNCTION&& doToElement) const;

    public:
        /**
         *  Complexity:
         *      Always: constant
         * 
         *  Returns iterator pointing at next item.
         */
        nonvirtual ForwardIterator RemoveAt (const ForwardIterator& i);

    public:
        /**
         *  Complexity:
         *      Always: constant
         * 
         *  \note lifetime of returned pointer only extends til the start of the next non-const call to this LinkedList object
         */
        nonvirtual T* PeekAt (const ForwardIterator& i);

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);

    public:
        /**
         *  Complexity:
         *      Always: constant
         *
         *  NB: Can be called if done
         */
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  Note - does nothing if item not found.
         */
        template <typename EQUALS_COMPARER>
        nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer);

    public:
        nonvirtual void RemoveAll ();

    public:
        /**
         *  Complexity:
         *      Always: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual void Append (ArgByValueType<T> item);

    public:
        /*
         *  Complexity:
         *      Always: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /*
         *  Complexity:
         *      Always: O(N)
         *
         *      Not alot of point in having these, as they are terribly slow,
         *  but the could be convienient.
         */
        nonvirtual void SetAt (T item, size_t i);

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        Link_* fHead_{nullptr};

#if qDebug
    private:
        virtual void Invariant_ () const noexcept;
#endif

    private:
        friend class ForwardIterator;
    };

    /**
     *  dont use block allocation for link sizes too large
     */
    template <typename T>
    class LinkedList<T>::Link_ : public Memory::UseBlockAllocationIfAppropriate<Link_, sizeof (T) <= 1024> {
    public:
        Link_ () = delete;
        Link_ (ArgByValueType<T> item, Link_* next);
        Link_ (const Link_&) = delete;

    public:
        T      fItem;
        Link_* fNext{nullptr};
    };

    /*
     *      ForwardIterator<T> allows you to iterate over a LinkedList<T,TRAITS>. Its API
     *  is designed to make easy implementations of subclasses of IteratorRep<T>.
     *  It is unpatched - use LinkedListIterator_Patch<T> or LinkedListMutator_Patch<T>
     *  for that.
     */
    template <typename T>
    class LinkedList<T>::ForwardIterator {
    public:
        ForwardIterator (const ForwardIterator& from) = default;
        ForwardIterator (const LinkedList* data);
        ForwardIterator (const LinkedList* data, UnderlyingIteratorRep startAt);

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator& it);

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        nonvirtual T Current () const;

    public:
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (const UnderlyingIteratorRep l);

    public:
        nonvirtual bool Equals (const ForwardIterator& rhs) const;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        const LinkedList* fData_;
        const Link_*      fCurrent_;

#if qDebug
    private:
        virtual void Invariant_ () const noexcept;
#endif

    private:
        friend class LinkedList;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_LinkedList_h_ */
