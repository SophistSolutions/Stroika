/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Configuration/TypeHints.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

/*
 *
 *  Description:
 *
 *      DoublyLinkedList<T,TRAITS> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *
 * TODO:
 *
 *  Long-Term TODO:
 *      @todo   Could add iterator subclass (or use traits to control) which tracks index internally, as with Stroika v1
 *              but this will do for and maybe best (depending on frequency of calls to CurrentIndex ()
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /*
     *      DoublyLinkedList<T> is a generic link (non-intrusive) list implementation.
     *  We provide no public means to access the links themselves.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename T>
    class DoublyLinkedList : public Debug::AssertExternallySynchronizedMutex {
    public:
        using value_type = T;

    public:
        /**
         */
        DoublyLinkedList ();
        DoublyLinkedList (DoublyLinkedList&& src);
        DoublyLinkedList (const DoublyLinkedList& src);
        ~DoublyLinkedList ();

    public:
        nonvirtual DoublyLinkedList& operator= (const DoublyLinkedList& list);

    private:
        class Link_;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = const Link_*;

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
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual optional<T> GetFirst () const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual optional<T> GetLast () const;

    public:
        /**
         *  \alias Prepend
         * 
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void push_front (ArgByValueType<T> item);

    public:
        /**
         *  \alias Append
         * 
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void push_back (ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         *
         *  \req not empty ()
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         *
         *  \req not empty ()
         */
        nonvirtual void RemoveLast ();

    public:
        /*
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Utility to search the list for the given item using EQUALS_COMPARER
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer = {}) const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        template <invocable<T> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <typename FUNCTION>
        nonvirtual UnderlyingIteratorRep Find (FUNCTION&& firstThat) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Note - does nothing if item not found.
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {});

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        class ForwardIterator;

    public:
        /*
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (ForwardIterator* pi, const DoublyLinkedList<T>* movedFrom) const;

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
         *  \note Complexity:
         *      Always: constant
         * 
         *  returns the next link
         */
        nonvirtual ForwardIterator RemoveAt (const ForwardIterator& i);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         *
         *  \req not i.Done ()
         */
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        Link_* fHead_{};
        Link_* fTail_{};

#if qDebug
    private:
        virtual void Invariant_ () const noexcept;
#endif

    private:
        friend class ForwardIterator;
    };

    /**
     *  Just an implementation detail. Don't use directly except in helper classes.
     *  dont use block allocation for link sizes too large
     */
    template <typename T>
    class DoublyLinkedList<T>::Link_ : public Memory::UseBlockAllocationIfAppropriate<Link_, sizeof (T) <= 1024> {
    public:
        Link_ ()             = delete;
        Link_ (const Link_&) = delete;
        constexpr Link_ (ArgByValueType<T> item, Link_* prev, Link_* next);

    public:
        T      fItem;
        Link_* fPrev{nullptr};
        Link_* fNext{nullptr};
    };

    /**
     *      ForwardIterator<T> allows you to iterate over a DoublyLinkedList<T>. Its API
     *  is designed to make easy implementations of subclasses of IteratorRep<T>.
     *  It is unpatched - use DoublyLinkedListIterator_Patch<T> or DoublyLinkedListIterator_Patch<T>
     *  for that.
     */
    template <typename T>
    class DoublyLinkedList<T>::ForwardIterator {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = DoublyLinkedList::value_type;
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
        explicit constexpr ForwardIterator (const DoublyLinkedList* data) noexcept;
        explicit constexpr ForwardIterator (const DoublyLinkedList* data, UnderlyingIteratorRep startAt) noexcept;
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
        nonvirtual const T& operator* () const;

    public:
        nonvirtual const T* operator->() const;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;
        nonvirtual ForwardIterator  operator++ (int) noexcept;

        /**
         *  \note Complexity:
         *      Average/WorseCase:  O(N)        - super slow cuz have to traverse on average half the list
         * 
         *  \req data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const DoublyLinkedList* data) const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (UnderlyingIteratorRep l);

    public:
        nonvirtual bool operator== (const ForwardIterator& rhs) const;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        const Link_* fCurrent_{nullptr};
#if qDebug
        const DoublyLinkedList* fData_{nullptr};
#endif

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        friend class DoublyLinkedList;
    };

    static_assert (ranges::input_range<DoublyLinkedList<int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_ */
