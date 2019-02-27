/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#include "../../StroikaPreComp.h"

#include "../Sequence.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_

/**
 *  \file
 *
 *  \version    NOT IMPLEMENTED
 *
 *  TODO:
 *      @todo   Most of the implementation is simple - use SortedMapping_stdmap as a backend. Map from idx
 *              to value. On any update, remove values if they are the special 'sparse' value. On iteration,
 *              count the gap between index values and return the 'sparse' value.
 *
 *              Lookup by index is a key lookup.
 *
 *              The only tricky part is that inserts involve renumbering keys past a certain point in the
 *              container.
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_SparseSortedMapping<T> is a sparse-Array-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Sequence_SparseSortedMapping : public Sequence<T> {
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
        Sequence_SparseSortedMapping (T sparseValue);
        Sequence_SparseSortedMapping (const Sequence_SparseSortedMapping<T>& src);
        template <typename CONTAINER_OF_T>
        explicit Sequence_SparseSortedMapping (T sparseValue, const CONTAINER_OF_T& src);
        template <typename COPY_FROM_ITERATOR_OF_T>
        explicit Sequence_SparseSortedMapping (T sparseValue, COPY_FROM_ITERATOR_OF_T start, COPY_FROM_ITERATOR_OF_T end);

    public:
        nonvirtual Sequence_SparseSortedMapping<T>& operator= (const Sequence_SparseSortedMapping<T>& s);

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

//#include    "Sequence_SparseSortedMapping.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_SparseSortedMapping_h_ */
