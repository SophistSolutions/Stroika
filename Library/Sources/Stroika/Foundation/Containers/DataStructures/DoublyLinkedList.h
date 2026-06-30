/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_
#define _Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Common/TypeHints.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

/*
 *
 *  Description:
 *
 *      DoublyLinkedList<T> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 * TODO:
 *
 *  Long-Term TODO:
 *      @todo   Could add iterator subclass (or use traits to control) which tracks index internally, as with Stroika v1
 *              but this will do for and maybe best (depending on frequency of calls to CurrentIndex ()
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Common::ArgByValueType;

    /**
     *      DoublyLinkedList<T> is a generic link (non-intrusive) list implementation.
     *  We provide no public means to access the links themselves.
     * 
     *  \note Satisfies Concepts:
     *      o   static_assert (ranges::input_range<DoublyLinkedList<T>>)
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
        DoublyLinkedList (DoublyLinkedList&& src) noexcept;
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
        class ForwardIterator;

    public:
        class BidirectionalIterator;

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
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual optional<T> GetFirst () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual optional<T> GetLast () const;

    public:
        /**
         *  @aliases Prepend
         * 
         *  \note Runtime performance/complexity:
         *      Always: constant
         * 
         *  \note for push_front(span) - this puts the span elements in front in the same order in
         *        which they appears in the span.
         */
        nonvirtual void push_front (ArgByValueType<T> item);
        template <Memory::ISpanOfT<T> SPAN_T>
        nonvirtual void push_front (const SPAN_T& copyFrom);

    public:
        /**
         *  @aliases Append
         * 
         *  \note Runtime performance/complexity:
         *      Always: constant
         * 
         *  \note for push_back(span) - this puts the span elements in back in the same order in
         *        which they appears in the span.
         */
        nonvirtual void push_back (ArgByValueType<T> item);
        template <Memory::ISpanOfT<T> SPAN_T>
        nonvirtual void push_back (const SPAN_T& copyFrom);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         *
         *  \pre not empty ()
         */
        nonvirtual void RemoveFirst ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         *
         *  \pre not empty ()
         */
        nonvirtual void RemoveLast ();

    public:
        /*
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Utility to search the list for the given item using EQUALS_COMPARER
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {}) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         */
        template <invocable<T> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of container
         */
        template <typename FUNCTION>
        nonvirtual UnderlyingIteratorRep Find (FUNCTION&& firstThat) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         *
         *  Note - does nothing if item not found.

         ForwardIterator OVERLOAD:
         *  \note Runtime performance/complexity:
         *      Always: constant
         * 
         *  returns the next link
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual void Remove (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {});
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
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void clear ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Average Case: O(N)
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        /*
         *  Support for COW (CopyOnWrite):
         *
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
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         *
         *  \pre not i.AtEnd ()
         */
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);

    public:
        nonvirtual void Invariant () const noexcept;

    private:
        Link_* fHead_{};
        Link_* fTail_{};

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        virtual void Invariant_ () const noexcept;
#endif

    private:
        friend class ForwardIterator;
        friend class BidirectionalIterator;
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
     * 
     *  \note Satisfies Concepts:
     *      o   forward_iterator<typename DoublyLinkedList<T>::ForwardIterator>>
     *      o   regular<typename DoublyLinkedList<T>::ForwardIterator>>        // implies copyable/movable/equality_comparable
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
         *  return true if iterator not AtEnd
         */
        explicit operator bool () const;

    public:
        nonvirtual bool AtEnd () const noexcept;

    public:
        nonvirtual const T& operator* () const;

    public:
        nonvirtual const T* operator->() const;

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;
        nonvirtual ForwardIterator  operator++ (int) noexcept;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Average/WorseCase:  O(N)        - super slow cuz have to traverse on average half the list
         * 
         *  \pre data == fData_ argument constructed with (or as adjusted by Move...); api takes extra param so release builds need not store fData_
         */
        nonvirtual size_t CurrentIndex (const DoublyLinkedList* data) const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (UnderlyingIteratorRep l);

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const DoublyLinkedList* data) const;

    public:
        nonvirtual bool operator== (const ForwardIterator& rhs) const;

    public:
        nonvirtual void Invariant () const noexcept;

    protected:
        const Link_* _fCurrent{nullptr};

    private:
#if qStroika_Foundation_Debug_AssertionsChecked
        const DoublyLinkedList* fData_{nullptr};
#endif

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        friend class DoublyLinkedList;
    };

    // see Satisfies Concepts
    static_assert (forward_iterator<typename DoublyLinkedList<int>::ForwardIterator>);
    static_assert (regular<typename DoublyLinkedList<int>::ForwardIterator>);

    /**
     * @brief Same as ForwardIterator, but adding ability to reverse direction
     * 
     *  \note Satisfies Concepts:
     *      o   regular<typename DoublyLinkedList<T>::ForwardIterator>>        // implies copyable/movable/equality_comparable
     *      o   bidirectional_iterator<typename DoublyLinkedList<T>::ForwardIterator>>
     */
    template <typename T>
    class DoublyLinkedList<T>::BidirectionalIterator : public ForwardIterator {
    private:
        using inherited = ForwardIterator;

    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = bidirectional_iterator_tag;
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
        constexpr BidirectionalIterator () noexcept = default;
        explicit constexpr BidirectionalIterator (const DoublyLinkedList* data) noexcept;
        explicit constexpr BidirectionalIterator (const DoublyLinkedList* data, UnderlyingIteratorRep startAt) noexcept;
        constexpr BidirectionalIterator (const BidirectionalIterator&) noexcept = default;
        constexpr BidirectionalIterator (BidirectionalIterator&&) noexcept      = default;

    public:
        nonvirtual BidirectionalIterator& operator= (const BidirectionalIterator&)     = default;
        nonvirtual BidirectionalIterator& operator= (BidirectionalIterator&&) noexcept = default;

    public:
        nonvirtual bool AtStart () const noexcept;

    public:
        /**
         * @brief  increment iterator (same as inherited version, but returning BidirectionalIterator)
         */
        nonvirtual BidirectionalIterator& operator++ () noexcept;
        nonvirtual BidirectionalIterator  operator++ (int) noexcept;

    public:
        /**
         * \pre not AtStart ()
         */
        nonvirtual BidirectionalIterator& operator-- () noexcept;
        nonvirtual BidirectionalIterator  operator-- (int) noexcept;

    public:
        /**
         *  \brief Move the iterator back by the specified number of positions (note i maybe positive or negative, but must result in a valid position).
         */
        nonvirtual BidirectionalIterator operator- (ptrdiff_t i) const;

    private:
        const DoublyLinkedList* fData_{nullptr}; // needed to always know length - even if null/at end
    };

    // see Satisfies Concepts
    static_assert (bidirectional_iterator<typename DoublyLinkedList<int>::BidirectionalIterator>);
    static_assert (regular<typename DoublyLinkedList<int>::BidirectionalIterator>);

    // see Satisfies Concepts
    static_assert (ranges::input_range<DoublyLinkedList<int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "DoublyLinkedList.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_DoublyLinkedList_h_ */
