/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_DataStructures_Array_h_
#define _Stroika_Foundation_Containers_DataStructures_Array_h_

#include "../../StroikaPreComp.h"

#include <optional>

#include "../../Common/Compare.h"
#include "../../Configuration/Common.h"
#include "../../Debug/AssertExternallySynchronizedLock.h"

#include "../Common.h"

/**
 *
 *  Description:
 *
 *      Array<T> is a backend implementation. It is not intended to be directly
 *  used by programmers, except in implementing concrete container reps.
 *
 *      Array<T> is a template which provides a dynamic array class. Elements
 *  of type T can be assigned, and accessed much like a normal array, except
 *  that when debug is on, accesses are range-checked.
 *
 *      Array<T> also provides a dynamic sizing capability. It reallocs its
 *  underlying storgage is such a ways as to keep a buffer of n(currently 5)%
 *  extra, so that reallocs on resizes only occur logn times on n appends.
 *  To save even this space, you can call Compact().
 *
 *      Unlike other dynamic array implementations, when an item is removed,
 *  it is destructed then. So the effects of buffering have no effects on the
 *  semantics of the Array.
 *
 *
 *  Notes:
 *
 *  C++/StandardC arrays and segmented architectures:
 *
 *      Our iterators use address arithmatic since that is faster than
 *  array indexing, but that requires care in the presence of patching,
 *  and in iterating backwards.
 *
 *      The natural thing to do in iteration would be to have fCurrent
 *  point to the current item, but that would pose difficulties in the
 *  final test at the end of the iteration when iterating backwards. The
 *  final test would be fCurrent < _fStart. This would be illegal in ANSI C.
 *
 *      The next possible trick is for backwards iteration always point one
 *  past the one you mean, and have it.Current () subtract one before
 *  dereferncing. This works pretty well, but makes source code sharing between
 *  the forwards and backwards cases difficult.
 *
 *      The next possible trick, and the one we use for now, is to have
 *  fCurrent point to the current item, and in the Next() code, when
 *  going backwards, reset fCurrent to _fEnd - bizzare as this may seem
 *  then the test code can be shared among the forwards and backwards
 *  implementations, all the patching code can be shared, with only this
 *  one minor check. Other potential choices are presented in the TODO
 *  below.
 *
 *
 *  TODO:
 *      @todo   See if there is some way to get the performance of realloc for the cases where we could have
 *              realloced without moving.
 *
 *      @todo   Improve perofrmance/cleanup memory allocation. ALREADY got rid of  realloc().
 *              but celanup safety/use uninitalized_copy and stl destroy functions.
 *
 *              NOTE - CAN USE realloc() if is_trivially_copyable<T>::value, so maybe do SFINAE
 *              different impls...
 *
 *      @todo   Replace Contains() with Lookup () - as we did for LinkedList<T>
 *
 *      @todo   Add RVALUE-REF (MOVE) stuff.
 *
 *      @todo   (close to rvalue thing above) - fix construction/destruction stuff. More mdoern C++.
 *              That crap was written around 1990!!!
 */

namespace Stroika::Foundation::Containers::DataStructures {

    using Configuration::ArgByValueType;

    /**
     *      This class is the main core of the implementation. It provides
     *  an array abstraction, where the size can be set dynamically, and
     *  extra sluff is maintained off the end to reduce copying from reallocs.
     *  Only items 0..GetLength ()-1 are kept constructed. The rest (GetLength()+1
     *  ..fSlotsAlloced) are uninitialized memory. This is important because
     *  it means you can count on DTORs of your T being called when you
     *  remove them from contains, not when the caches happen to empty.
     */
    template <typename T>
    class Array : public Debug::AssertExternallySynchronizedLock {
    public:
        using value_type = T;

    public:
        Array () = default;
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
        nonvirtual T* PeekAt (size_t i);

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
        nonvirtual size_t GetLength () const;

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical Case: ?? for small changes often constant, but if enuf change of size O(N) growing. Less shrinking.
         */
        nonvirtual void SetLength (size_t newLength, ArgByValueType<T> fillValue);

    public:
        /**
         *  \note Complexity:
         *      Worst Case: O(N)
         *      Typical: depends on i, and Capacity - if need to change capacity O(N), and if near start of array O(N), and if near end of the array (append) can be cheap
         */
        nonvirtual void InsertAt (size_t index, ArgByValueType<T> item);

    public:
        nonvirtual void RemoveAt (size_t index);

    public:
        nonvirtual void RemoveAll ();

    public:
        template <typename EQUALS_COMPARER = equal_to<T>>
        nonvirtual bool Contains (ArgByValueType<T> item, const EQUALS_COMPARER& equalsComparer) const;

    public:
        template <typename FUNCTION>
        nonvirtual void Apply (FUNCTION doToElement) const;

    public:
        template <typename FUNCTION>
        nonvirtual size_t FindFirstThat (FUNCTION doToElement) const;

    public:
        /*
         * Memory savings/optimization methods.  Use this to tune useage
         * of arrays so that they don't waste time in Realloc's.
         */
        nonvirtual size_t GetCapacity () const;

    public:
        nonvirtual void SetCapacity (size_t slotsAlloced);

    public:
        nonvirtual void Compact ();

    public:
        class IteratorBase;

    public:
        class ForwardIterator;
        class BackwardIterator;

    public:
        /*
         *  Take iterator 'pi' which is originally a valid iterator from 'movedFrom' - and replace *pi with a valid
         *  iterator from 'this' - which points at the same logical position. This requires that this container
         *  was just 'copied' from 'movedFrom' - and is used to produce an eqivilennt iterator (since iterators are tied to
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
        nonvirtual void Invariant () const;

#if qDebug
    private:
        nonvirtual void Invariant_ () const;
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
    private:
        IteratorBase () = delete;

    public:
        IteratorBase (const Array* data);
        IteratorBase (const IteratorBase& src) = default;

#if qDebug
        ~IteratorBase ();
#endif

    public:
        nonvirtual T Current () const; //  Error to call if Done (), otherwise OK

    public:
        nonvirtual size_t CurrentIndex () const; //  NB: This can be called if we are done - if so, it returns GetLength() + 1.

    public:
        nonvirtual bool More (T* current, bool advance);

    public:
        nonvirtual bool Done () const;

    public:
        nonvirtual void SetIndex (size_t i);

    public:
        nonvirtual bool Equals (const IteratorBase& rhs) const;

    public:
        nonvirtual void PatchBeforeAdd (const IteratorBase& adjustmentAt);

    public:
        nonvirtual void PatchBeforeRemove (const IteratorBase* adjustmentAt);

    public:
        nonvirtual void Invariant () const;

#if qDebug
    private:
        nonvirtual void Invariant_ () const;
#endif

    private:
        const Array* fData_{nullptr};
        size_t       fCurrentIdx_{0};

    private:
        friend class Array;
    };

    /**
     *      Use this iterator to iterate forwards over the array. Be careful
     *  not to add or remove things from the array while using this iterator,
     *  since it is not safe. Use ForwardIterator_Patch for those cases.
     */
    template <typename T>
    class Array<T>::ForwardIterator : public Array<T>::IteratorBase {
    private:
        using inherited = IteratorBase;

    public:
        ForwardIterator (const Array* data);
        ForwardIterator (const ForwardIterator& src) = default;

    public:
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);

    public:
        nonvirtual ForwardIterator& operator++ () noexcept;
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
        BackwardIterator (const Array<T>* data);

    public:
        nonvirtual bool More (T* current, bool advance);
        nonvirtual void More (optional<T>* result, bool advance);
        nonvirtual bool More (nullptr_t, bool advance);
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Array.inl"

#endif /*_Stroika_Foundation_Containers_DataStructures_Array_h_ */
