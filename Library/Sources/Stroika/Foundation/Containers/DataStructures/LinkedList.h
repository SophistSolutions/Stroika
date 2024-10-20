/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_LinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_LinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

/**
 *      LinkedList<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 * TODO:
 *      @todo   Include Performance numbers for each operation (done for many).
 *      @todo   https://stroika.atlassian.net/browse/STK-1016 - ranges/sentinel support
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
     *      LinkedList<T,TRAITS> is a generic link (non-intrusive) list implementation (similar to std::forward_list).
     *   We provide no public means to access the links themselves.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class LinkedList : public Debug::AssertExternallySynchronizedMutex {
    public:
        using value_type = T;

    public:
        /**
         */
        LinkedList ();
        LinkedList (LinkedList&& src);
        LinkedList (const LinkedList& src);
        ~LinkedList ();

    public:
        nonvirtual LinkedList& operator= (const LinkedList& list);

    public:
        class ForwardIterator;

    private:
        class Link_;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = const Link_*;

    public:
        /*
         *  Support for COW (CopyOnWrite):
         *
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const LinkedList* movedFrom) const;

    public:
        /**
         */
        nonvirtual ForwardIterator begin () const;

    public:
        /**
         */
        constexpr ForwardIterator end () const noexcept;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual optional<T> GetFirst () const;

    public:
        /**
         *  \alias Prepend
         * 
         *  Complexity:
         *      Always: constant
         * 
         *  \see push_back
         */
        nonvirtual void push_front (ArgByValueType<T> item);

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         *  @todo add function concept
         */
        template <invocable<T> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         * 
         *  @todo add predicate
         * 
         *  *  Complexity EQUALS_COMPARER OVERLOAD:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *   EQUALS_COMPARER OVERLOAD : Returns pointer to T (or nullptr if not found). Lifetime of T* only til next call on this.
         */
        template <predicate<T> FUNCTION>
        nonvirtual UnderlyingIteratorRep Find (FUNCTION&& firstThat) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual const T* Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {}) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual T* Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {});

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
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item, ForwardIterator* newLinkCreatedAt);

    public:
        /**
         *  Complexity:
         *      Always: constant
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  EQUALS_COMPARER overload: Note - does nothing if item not found.
         * 
         *  Complexity (ForwardIterator overload):
         *      Always: constant
         */
        template <typename EQUALS_COMPARER>
        nonvirtual void Remove (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer);
        nonvirtual void Remove (const ForwardIterator& i);

    public:
        /**
         *  Complexity:
         *      Always: constant
         * 
         *  Returns iterator pointing at next item.
         */
        nonvirtual ForwardIterator erase (const ForwardIterator& i);

    public:
        /**
         *  Complexity:
         *      Always: O(N) - but generally quite quick cuz uses block (de-)allocation
         * 
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \alias Append
         * 
         *  Complexity:
         *      Always: O(N)
         *
         *      Not a lot of point in having this method, as is terribly slow, but the could be convenient.
         *
         *  \see push_front
         */
        nonvirtual void push_back (ArgByValueType<T> item);

    public:
        /*
         *  Complexity:
         *      Always: O(N)
         *
         *      Not a lot of point in having this method, as is terribly slow, but the could be convenient.
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /*
         *  Complexity:
         *      Always: O(i)
         *
         *      Not a lot of point in having this method, as is terribly slow, but the could be convenient.
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
    class LinkedList<T>::Link_ : public Memory::UseBlockAllocationIfAppropriate<Link_, sizeof (T) <= 256> {
    public:
        Link_ () = delete;
        constexpr Link_ (ArgByValueType<T> item, Link_* next);
        Link_ (const Link_&) = delete;

    public:
        T      fItem;
        Link_* fNext{nullptr};
    };

    /*
     *      ForwardIterator allows you to iterate over a LinkedList<T>. It is not safe to use a ForwardIterator after any
     *      update to the LinkedList.
     */
    template <typename T>
    class LinkedList<T>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = LinkedList::value_type;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    public:
        /**
         *  /0 overload: sets iterator to 'end' - sentinel
         *  /1 (data) overload: sets iterator to begin
         *  /2 (data,startAt) overload: sets iterator to startAt
         */
        constexpr ForwardIterator () noexcept = default;
        explicit constexpr ForwardIterator (const LinkedList* data) noexcept;
        explicit constexpr ForwardIterator (const LinkedList* data, UnderlyingIteratorRep startAt) noexcept;
        constexpr ForwardIterator (const ForwardIterator&) noexcept = default;
        constexpr ForwardIterator (ForwardIterator&&) noexcept      = default;

    public:
        nonvirtual ForwardIterator& operator= (const ForwardIterator&)     = default;
        nonvirtual ForwardIterator& operator= (ForwardIterator&&) noexcept = default;

    public:
        /**
         *  return true if iterator not Done
         */
        explicit operator bool () const;

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;
        nonvirtual ForwardIterator  operator++ (int) noexcept;

    public:
        nonvirtual T operator* () const;

    public:
        nonvirtual const T* operator->() const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Average/WorseCase:  O(N)        - super slow cuz have to traverse on average half the list
         * 
         *  \req data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const LinkedList* data) const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (const UnderlyingIteratorRep l);

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const LinkedList* data) const;

    public:
        nonvirtual bool operator== (const ForwardIterator& rhs) const;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        const Link_* fCurrent_{nullptr};
#if qDebug
        const LinkedList* fData_{nullptr};
#endif

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        friend class LinkedList;
    };

    static_assert (ranges::input_range<LinkedList<int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "LinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_LinkedList_h_ */
