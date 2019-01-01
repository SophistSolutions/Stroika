/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_ChunkedArray_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_ChunkedArray_h_

/**
 *  \file
 *
 *  \version    NOT IMPLEMENTED
 *
 *  TODO:
 *      @todo   Idea is basically the same as Led::ChunkedArrayTextStore. Except that uses a vector of chunk
 *              pointers. It may be better to use a doubly linked list of Chunks? Unsure. Or maybe a tree (stl map)
 *              of chunks, so you can insert/remove them more easily.
 *
 *              The Led (vector Chunk*) doesn't work well as you append lots of data to the end of the sequence (realloc).
 *
 *              DoublyLinkedList of chunks with cache for some 'central' point can allow you to take advantage of typical
 *              locality of reference (searching/udpating in an area), and still have the performance benefits of no
 *              grand slowdown (except if you index outside that region, you get bad lookup behavior. BUt a tree
 *              structure might eliminate that problem too.. SkipList?
 *
 *              One other thing HARDER about this comapred with the old LED code - was tha twe have to handle correclty
 *              manual item construction/destruciton (in Led we used chars and so could leave uninitiualized and not be
 *              careful about move etc).
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_SparseSortedMapping<T> is a sparse-Array-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T, size_t CHUNK_SIZE = 4096>
    class Sequence_ChunckedArray : public Sequence<T> {
    private:
        using inherited = Sequence<T>;

    public:
        /**
         *  The only non-obvious parameter is 'sparseValue'. This is the special value used internally to
         *  make for the 'sparse' array. These particular values don't require storage.
         *
         *  Note - this has no externally visible semantics: it just affects the storage usage, and perhaps
         *  performance.
         */
        Sequence_ChunckedArray ();
        Sequence_ChunckedArray (const Sequence_ChunckedArray<T>& s);
        template <typename CONTAINER_OF_T>
        explicit Sequence_ChunckedArray (const CONTAINER_OF_T& s);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Sequence_ChunckedArray (COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        nonvirtual Sequence_ChunckedArray<T>& operator= (const Sequence_ChunckedArray<T>& rhs);

    private:
        class Rep_;

    private:
        nonvirtual const Rep_& GetRep_ () const;
        nonvirtual Rep_& GetRep_ ();
    };
}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */

//#include    "Sequence_ChunckedArray.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_ChunkedArray_h_ */
