/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_Array_h_
#define _Stroika_Foundation_Containers_DataStructures_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Configuration/Common.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Common.h"

/**
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /**
     *      This class provides an array abstraction, where the size can be set dynamically, and
     *  extra sluff is maintained off the end to reduce copying from reallocs.
     *  Only items 0..size ()-1 are kept constructed. The rest (size()+1
     *  ..fSlotsAlloced) are uninitialized memory. This is important because
     *  it means you can count on DTORs of your T being called when you
     *  remove them from contains, not when the caches happen to empty.
     *
     *      Array<T> is simple data structure implementation. It is not intended to be directly
     *  used by programmers, except in implementing concrete container reps.
     *
     *      Array<T> is a template which provides a dynamic array class (very similar to std::vector). Elements
     *  of type T can be assigned, and accessed much like a normal array, except
     *  that when debug is on, accesses are range-checked.
     *
     *      Array<T> also provides a dynamic sizing capability. It reallocs its
     *  underlying storage is such a ways as to keep a buffer of n(currently 5)%
     *  extra, so that reallocs on resizes only occur logn times on n appends.
     *  To save even this space, you can call shrink_to_fit().
     *
     *      Unlike other dynamic array implementations, when an item is removed,
     *  it is destructed then. So the effects of buffering have no effects on the
     *  semantics of the Array.
     *
     *
     *  Notes:
     *
     *  C++/StandardC arrays and segmented architectures: ((@todo DOCS OBSOLETE))
     *
     *      Our iterators use address arithmetic since that is faster than
     *  array indexing, but that requires care in the presence of patching,
     *  and in iterating backwards.
     *
     *      The natural thing to do in iteration would be to have fCurrent
     *  point to the current item, but that would pose difficulties in the
     *  final test at the end of the iteration when iterating backwards. The
     *  final test would be fCurrent < _fStart. This would be illegal in ANSI C.
     *
     *      The next possible trick is for backwards iteration always point one
     *  past the one you mean, and have *it subtract one before
     *  dereferencing. This works pretty well, but makes source code sharing between
     *  the forwards and backwards cases difficult.
     *
     *      The next possible trick, and the one we use for now, is to have
     *  fCurrent point to the current item, and in the Next() code, when
     *  going backwards, reset fCurrent to _fEnd - bizarre as this may seem
     *  then the test code can be shared among the forwards and backwards
     *  implementations, all the patching code can be shared, with only this
     *  one minor check. Other potential choices are presented in the TODO
     *  below.
     *
     *  TODO:
     *      @todo   See if there is some way to get the performance of realloc for the cases where we could have
     *              realloced without moving.
     *
     *      @todo   Improve performance/cleanup memory allocation. ALREADY got rid of  realloc().
     *              but celanup safety/use uninitialized_copy and stl destroy functions.
     *
     *              NOTE - CAN USE realloc() if is_trivially_copyable<T>::value, so maybe do SFINAE
     *              different impls...
     *
     *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
     *              (IN FACT, Find (function) overload basically same thing - maybe have two find overloads - with function and comparer
     *              but easily confusable, and redundant, but maybe useful for performance?)
     *
     *      @todo   Add RVALUE-REF (MOVE) stuff.
     *
     *      @todo   (close to rvalue thing above) - fix construction/destruction stuff. More modern C++.
     *              That crap was written around 1990!!!
     */
    template <typename T>
    class Array : public Debug::AssertExternallySynchronizedMutex {
    public:
        using value_type = T;

    public:
        /**
         *  Basic (mostly internal) element used by ForwardIterator. Abstract name so can be referenced generically across 'DataStructure' objects
         */
        using UnderlyingIteratorRep = size_t;

    public:
        /**
         */
        Array () = default;
        Array (Array&& from);
        Array (const Array& from);

    public:
        ~Array ();

    public:
        nonvirtual Array& operator= (const Array& rhs);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  Not a great API, since cannot check it very well. However, its more efficient when storing a larger object and you need
         *  to update just part of it.
         *
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual T*       PeekAt (size_t i);
        nonvirtual const T* PeekAt (size_t i) const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual T& operator[] (size_t i);
        nonvirtual T  operator[] (size_t i) const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  \note Complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical Case: ?? for small changes often constant, but if enuf change of size O(N) growing. Less shrinking.
         */
        nonvirtual void SetLength (size_t newLength, ArgByValueType<T> fillValue);

    public:
        /**
         *  \note index may == size() - in which case, we are appending.
         *
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: depends on i, and Capacity - if need to change capacity O(N), and if near start of array O(N), and if near end of the array (append) can be cheap
         */
        nonvirtual void InsertAt (size_t index, ArgByValueType<T> item);

    public:
        /**
         *  \brief STL-ish alias for Insert (size(), item)
         * 
         *  \alias Append
         * 
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(1)
         */
        nonvirtual void push_back (ArgByValueType<T> item);

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: depends on index but typically O(N) (can be less if removing from end of Array)
         */
        nonvirtual void RemoveAt (size_t index);

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N) - if !trivial_type
         *      Typical: O(1)
         */
        nonvirtual void RemoveAll ();

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of array
         */
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer) const;

    public:
        /**
         *  \note Complexity:
         *      Always: O(N)
         */
        template <invocable<T> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement, Execution::SequencePolicy seq = Execution::SequencePolicy::eDEFAULT) const;

    public:
        class IteratorBase;

    public:
        class ForwardIterator;
        class BackwardIterator;

    public:
        /**
         */
        nonvirtual ForwardIterator begin () const;

    public:
        /**
         */
        constexpr ForwardIterator end () const;

    public:
        /**
         *  Return index of first place in the array matching, or nullopt if not found
         * 
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of array
         * 
         *  \note in Stroika v2.1, this returned value == size() means not found, but now uses optional to make clearer
         *        and more similar to LinkedList find ...
         * 
         *  \note before Stroika v3.0d10, this returned optional<size_t>
         */
        template <predicate<T> FUNCTION>
        nonvirtual ForwardIterator Find (FUNCTION&& firstThat) const;

    public:
        /*
         * Memory savings/optimization methods.  Use this to tune usage
         * of arrays so that they don't waste time in Realloc's.
         */
        nonvirtual size_t capacity () const;

    public:
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        nonvirtual void shrink_to_fit ();

    public:
        /*
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (IteratorBase* pi, const Array* movedFrom) const;

    public:
        nonvirtual void RemoveAt (const ForwardIterator& i);
        nonvirtual void RemoveAt (const BackwardIterator& i);

    public:
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);
        nonvirtual void SetAt (const BackwardIterator& i, ArgByValueType<T> newValue);

    public:
        //  NB: Can be called if i done
        nonvirtual void AddBefore (const ForwardIterator& i, ArgByValueType<T> item);
        nonvirtual void AddBefore (const BackwardIterator& i, ArgByValueType<T> item);

    public:
        nonvirtual void AddAfter (const ForwardIterator& i, ArgByValueType<T> item);
        nonvirtual void AddAfter (const BackwardIterator& i, ArgByValueType<T> item);

    public:
        nonvirtual void Invariant () const noexcept;

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    private:
        size_t fLength_{0};         // #items advertised/constructed
        size_t fSlotsAllocated_{0}; // #items allocated (though not necessarily initialized)
        T*     fItems_{nullptr};
    };

    /**
     *      IteratorBase<T> is an un-advertised implementation
     *  detail designed to help in source-code sharing among various
     *  iterator implementations.
     * 
     *  \note Design note:
     *      Use index instead of cursored pointer, since performance appears same either way, and
     *      cursored pointer requires patching considerations on 'realloc'.
     */
    template <typename T>
    class Array<T>::IteratorBase {
    public:
        // stuff STL requires you to set to look like an iterator
        using iterator_category = forward_iterator_tag;
        using value_type        = Array::value_type;
        using difference_type   = ptrdiff_t;
        using pointer           = const value_type*;
        using reference         = const value_type&;

    public:
        constexpr IteratorBase () noexcept = default;
        IteratorBase (const Array* data);
        IteratorBase (const IteratorBase&) noexcept = default;

#if qDebug
        ~IteratorBase ();
#endif

    public:
        nonvirtual const T& operator* () const; //  Error to call if Done (), otherwise OK

    public:
        nonvirtual const T* operator->() const; //  Error to call if Done (), otherwise OK

    public:
        nonvirtual size_t CurrentIndex () const; //  NB: This can be called if we are done - if so, it returns size() + 1.

    public:
        nonvirtual void SetIndex (size_t i);

    public:
        nonvirtual bool operator== (const IteratorBase& rhs) const;

    public:
        nonvirtual UnderlyingIteratorRep GetUnderlyingIteratorRep () const;

    public:
        nonvirtual void SetUnderlyingIteratorRep (const UnderlyingIteratorRep l);

    public:
        /**
         *  For debugging, assert the iterator data matches argument data
         */
        constexpr void AssertDataMatches (const Array* data) const;

    public:
        nonvirtual void Invariant () const noexcept;

#if qDebug
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    protected:
        const Array* _fData{nullptr};
        size_t       _fCurrentIdx{0};

    private:
        friend class Array;
    };

    /**
     *      Use this iterator to iterate forwards over the array. Be careful
     *  not to add or remove things from the array while using this iterator,
     *  since it is not safe.
     */
    template <typename T>
    class Array<T>::ForwardIterator : public Array<T>::IteratorBase {
    private:
        using inherited = IteratorBase;

    public:
        /**
         *  overload taking only 'data' starts at beginning.
         *  note startAt = 0 for begin(), and startAt = data->size () for end
         */
        constexpr ForwardIterator () noexcept = default;
        explicit ForwardIterator (const Array* data, UnderlyingIteratorRep startAt = static_cast<UnderlyingIteratorRep> (0));
        ForwardIterator (const ForwardIterator&) noexcept = default;
        constexpr ForwardIterator (ForwardIterator&&) noexcept;

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
    };

    /**
     *      Use this iterator to iterate backwards over the array. Be careful
     *  not to add or remove things from the array while using this iterator,
     *  since it is not safe. Use BackwardIterator_Patch for those cases.
     * 
     *      // NOTE - I THINK NYI (fully) and not used
     */
    template <typename T>
    class Array<T>::BackwardIterator : public Array<T>::IteratorBase {
    private:
        using inherited = IteratorBase;

    public:
        BackwardIterator (const Array* data);
        BackwardIterator (const Array* data, UnderlyingIteratorRep startAt);

    public:
        nonvirtual bool Done () const noexcept;

    public:
        nonvirtual BackwardIterator& operator++ () noexcept;
    };

    static_assert (ranges::input_range<Array<int>>); // smoke test - make sure basic iteration etc should work (allows formattable to work)

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Array.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_Array_h_ */
