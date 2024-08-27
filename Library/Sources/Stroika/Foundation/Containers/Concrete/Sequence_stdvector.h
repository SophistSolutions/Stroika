/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_
#define _Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Private/StdVectorSupport.h"
#include "Stroika/Foundation/Containers/Sequence.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 *  TODO:
 *      @todo   Be sure can move-semantics into and out of Sequence_stdvector() - with vector<T> -
 *              so can go back and forth between reps efficiently. This COULD use used to avoid
 *              any performance overhead with Stroika Sequences.
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Sequence_stdvector<T> is an std::vector-based concrete implementation of the Sequence<T> container pattern.
     *
     *  \note Performance notes
     * 
     *      A good default implementation, except that empirically, this appears slower than Sequence_Array<>.
     *
     *      o   push_back should perform well (typically constant time, but occasionally O(N))
     *      o   it is FAST to array index a Sequence_stdvector.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     */
    template <typename T>
    class Sequence_stdvector : public Private::StdVectorBasedContainer<Sequence_stdvector<T>, Sequence<T>> {
    private:
        using inherited = Private::StdVectorBasedContainer<Sequence_stdvector<T>, Sequence<T>>;

    public:
        using value_type = typename inherited::value_type;

    public:
        /**
         *  \see docs on Sequence<> constructor
         *       (plus a move constructor for vector<T>)
         */
        Sequence_stdvector ();
        Sequence_stdvector (Sequence_stdvector&&) noexcept      = default;
        Sequence_stdvector (const Sequence_stdvector&) noexcept = default;
        Sequence_stdvector (std::vector<T>&& src);
        Sequence_stdvector (const initializer_list<value_type>& src);
        template <IIterableOf<T> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Sequence_stdvector<T>>)
        explicit Sequence_stdvector (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Sequence_stdvector{}
        {
            if constexpr (Configuration::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                this->reserve (src.size ());
            }
            this->AppendAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IInputIterator<T> ITERATOR_OF_ADDABLE>
        Sequence_stdvector (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        /**
         */
        nonvirtual Sequence_stdvector& operator= (Sequence_stdvector&&) noexcept = default;
        nonvirtual Sequence_stdvector& operator= (const Sequence_stdvector&)     = default;

    private:
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;

    private:
        friend inherited;
    };

}

/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include "Sequence_stdvector.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Sequence_stdvector_h_ */
