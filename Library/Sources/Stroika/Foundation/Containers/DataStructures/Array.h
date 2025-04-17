/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2025.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_Array_h_
#define _Stroika_Foundation_Containers_DataStructures_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include <optional>

#include "Stroika/Foundation/Common/Common.h"
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Containers/Common.h"
#include "Stroika/Foundation/Debug/AssertExternallySynchronizedMutex.h"
#include "Stroika/Foundation/Execution/Common.h"
#include "Stroika/Foundation/Memory/Common.h"

/**
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Common::ArgByValueType;

    /**
     *  \brief very similar to std::vector<T>
     * 
     *      This class provides an array abstraction, where the size can be set dynamically, and
     *  extra sluff is maintained off the end to reduce copying from reallocs.
     *  Only items 0..size ()-1 are kept constructed. The rest (size()+1
     *  ..fSlotsAlloced) are uninitialized memory. This is important because
     *  it means you can count on DTORs of your T being called when you
     *  remove them from contains, not when the caches happen to empty.
     *
     *      Array<T> is simple data structure implementation. It is not intended to be directly
     *  used by programmers, except in implementing concrete container reps (and occasionally in
     *  performance sensitive situations, though std::vector<> maybe a better choice then).
     *
     *      Array<T> is a template which provides a dynamic array class (very similar to std::vector). Elements
     *  of type T can be assigned, and accessed much like a normal array, except
     *  that when debug is on, accesses are range-checked.
     *
     *      Array<T> also provides a dynamic sizing capability. It reallocs its
     *  underlying storage is such a ways as to keep a buffer of roughly 20%
     *  extra (see Support::ReserveTweaks::GetScaledUpCapacity), so that reallocs on resizes
     *  only occur log(n) times on n appends.
     *  To save even this space, you can call shrink_to_fit().
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
        class ForwardIterator;
        class BackwardIterator;

    public:
        /**
         *  \brief returns internal pointer to data - which is unsynchronized, and only guaranteed valid until the next non-const array method.
         */
        nonvirtual T*       data () noexcept;
        nonvirtual const T* data () const noexcept;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual T GetAt (size_t i) const;

    public:
        /**
         *  Not a great API, since cannot check it very well. However, its more efficient when storing a larger object and you need
         *  to update just part of it.
         *
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual T*       PeekAt (size_t i);
        nonvirtual const T* PeekAt (size_t i) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual void SetAt (size_t i, ArgByValueType<T> item);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual T& operator[] (size_t i);
        nonvirtual T  operator[] (size_t i) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: constant
         */
        nonvirtual bool empty () const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical Case: ?? for small changes often constant, but if enuf change of size O(N) growing. Less shrinking.
         */
        nonvirtual void SetLength (size_t newLength, ArgByValueType<T> fillValue);

    public:
        /**
         *  \note index may == size() - in which case, we are appending.
         *
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: depends on i, and Capacity - if need to change capacity O(N), and if near start of array O(N), and if near end of the array (append) can be cheap
         */
        nonvirtual void Insert (size_t index, ArgByValueType<T> item);
        template <Memory::ISpanOfT<T> SPAN_T>
        nonvirtual void Insert (size_t at, const SPAN_T& copyFrom);
        nonvirtual void Insert (const ForwardIterator& i, ArgByValueType<T> item);
        nonvirtual void Insert (const BackwardIterator& i, ArgByValueType<T> item);

    public:
#if qCompilerAndStdLib_MemoryInsertAt_Buggy
        nonvirtual void Insert_BWA (size_t index, ArgByValueType<T> item);
#endif

    public:
        /**
         *  \brief STL-ish alias for Insert (size(), item)
         * 
         *  @aliases Append
         * 
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: constant
         */
        nonvirtual void push_back (ArgByValueType<T> item);

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N) - if !trivial_type
         *      Typical: constant
         */
        nonvirtual void clear ();

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Always: O(N)
         */
        template <invocable<T> FUNCTION>
        nonvirtual void Apply (FUNCTION&& doToElement, Execution::SequencePolicy seq = Execution::SequencePolicy::eDEFAULT) const;

    public:
        class IteratorBase;

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
         *  Return ForwardIterator of first place in the array matching, or nullptr if not found
         * 
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: O(N), but can be less if systematically finding entries near start of array
         * 
         *  \note in Stroika v2.1, this returned value == size() means not found, but now uses optional to make clearer
         *        and more similar to LinkedList find ...
         * 
         *  \note before Stroika v3.0d10, this returned optional<size_t>
         *        
         *   EQUALS_COMPARER OVERLOAD : Returns pointer to T (or nullptr if not found). Lifetime of T* only til next call on this.
         * 
         *  \alias Lookup, First, Contains (sort of)
         */
        template <predicate<T> FUNCTION>
        nonvirtual ForwardIterator Find (FUNCTION&& firstThat) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual const T* Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {}) const;
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual T* Find (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer = {});

    public:
        /*
         * Memory savings/optimization methods.  Use this to tune usage
         * of arrays so that they don't waste time in Realloc's.
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *  \brief sets the reserved capacity to slotsAlloced
         * 
         *  \pre size () <= slotsAlloced
         * 
         *  \see also more likely to use ReserveAtLeast
         */
        nonvirtual void reserve (size_t slotsAlloced);

    public:
        /**
         *  \note slotsAllocated maybe < size() - but then it would be ignored, since this only grows the capacity
         */
        nonvirtual void ReserveAtLeast (size_t slotsAlloced);

    public:
        /**
         */
        nonvirtual void shrink_to_fit ();

    public:
        /*
         *  Support for COW (CopyOnWrite):
         *
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an equivalent iterator (since iterators are tied to
         *  the container they were iterating over).
         */
        nonvirtual void MoveIteratorHereAfterClone (IteratorBase* pi, const Array* movedFrom) const;

    public:
        /**
         *  \note Runtime performance/complexity:
         *      Worst Case: O(N)
         *      Typical: depends on index but typically O(N) (can be less if removing from end of Array)
         *
         *  \see erase () - same as Remove(it) but returns iterator of 'next'
         */
        nonvirtual void Remove (const ForwardIterator& i);
        nonvirtual void Remove (const BackwardIterator& i);
        nonvirtual void Remove (size_t index) noexcept;
        nonvirtual void Remove (size_t from, size_t to) noexcept;

    public:
        /**
         *  \brief remove the element at i, and return valid iterator to the element that was following it (which can be empty iterator)
         * 
         *  \pre i != end ()
         * 
         *  \brief see https://en.cppreference.com/w/cpp/container/vector/erase
         */
        nonvirtual ForwardIterator erase (const ForwardIterator& i);

    public:
        /**
         */
        nonvirtual void SetAt (const ForwardIterator& i, ArgByValueType<T> newValue);
        nonvirtual void SetAt (const BackwardIterator& i, ArgByValueType<T> newValue);

    public:
        nonvirtual void Invariant () const noexcept;

#if qStroika_Foundation_Debug_AssertionsChecked
    private:
        nonvirtual void Invariant_ () const noexcept;
#endif

    public:
        template <typename EQUALS_COMPARER = equal_to<T>>
        [[deprecated ("Since Stroika v3.0d18")]] bool Contains (ArgByValueType<T> item, EQUALS_COMPARER&& equalsComparer) const
        {
            return this->Find (item, equalsComparer) != nullptr;
        }
        /**
         *  \brief insert the 
         *  NB: Can be called if i done, and just means add before the last item (so if i==end() - same as append)
         */
        [[deprecated ("Since v3.0d18 - use Insert()")]] void AddBefore (const ForwardIterator& i, ArgByValueType<T> item)
        {
            InsertAt (i, item);
        }
        [[deprecated ("Since v3.0d18 - use Insert()")]] void AddBefore (const BackwardIterator& i, ArgByValueType<T> item)
        {
            InsertAt (i, item);
        }
        [[deprecated ("Since v3.0d18 - use Insert()")]] void AddAfter (const ForwardIterator& i, ArgByValueType<T> item)
        {
            Insert (i.CurrentIndex () + 1, item);
        }
        [[deprecated ("Since v3.0d18 - use Insert()")]] void AddAfter (const BackwardIterator& i, ArgByValueType<T> newValue)
        {
            Insert (i.CurrentIndex () + 1, newValue);
        }

    private:
        // mostly useful cuz allows for use of realloc, which might imply fewer copies,
        // but is only legal for trivially_copyable types (cuz realloc sometimes resizes sometimes moves data)
        static constexpr bool kUseMalloc_{is_trivially_copyable_v<T>};

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

#if qStroika_Foundation_Debug_AssertionsChecked
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

#if qStroika_Foundation_Debug_AssertionsChecked
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

    public:
        nonvirtual bool operator== (const ForwardIterator& rhs) const;
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

    public:
        nonvirtual bool operator== (const BackwardIterator& rhs) const;
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
