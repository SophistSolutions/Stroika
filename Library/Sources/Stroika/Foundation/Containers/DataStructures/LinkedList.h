/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        class Link;

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
        nonvirtual bool IsEmpty () const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        nonvirtual size_t GetLength () const;

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
        nonvirtual void Apply (FUNCTION doToElement) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <typename FUNCTION>
        nonvirtual Link* FindFirstThat (FUNCTION doToElement) const;

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
        nonvirtual void Invariant () const;

    private:
        Link* fHead_{nullptr};

#if qDebug
    private:
        virtual void Invariant_ () const;
#endif

    private:
        friend class ForwardIterator;
    };

    /**
     *  dont use block allocation for link sizes too large
     */
    template <typename T>
    class LinkedList<T>::Link : public Memory::UseBlockAllocationIfAppropriate<Link, sizeof (T) <= 1024> {
    public:
        Link () = delete;
        Link (ArgByValueType<T> item, Link* next);
        Link (const Link&) = delete;

    public:
        T     fItem;
        Link* fNext{nullptr};
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

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator& it);

    public:
        nonvirtual bool Done () const;
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual ForwardIterator& operator++ () noexcept;

    public:
        nonvirtual T Current () const;

    public:
        nonvirtual size_t CurrentIndex () const;

    public:
        nonvirtual const Link* GetCurrentLink () const;

    public:
        nonvirtual void SetCurrentLink (const Link* l);

    public:
        nonvirtual bool Equals (const ForwardIterator& rhs) const;

    public:
        nonvirtual void PatchBeforeRemove (const ForwardIterator* adjustmentAt);

    public:
        nonvirtual void Invariant () const;

    private:
        const LinkedList* fData_;
        const Link*       fCurrent_;

#if qDebug
    private:
        virtual void Invariant_ () const;
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
