/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Association_Array_h_
#define _Stroika_Foundation_Containers_Concrete_Association_Array_h_

#include "Stroika/Foundation/StroikaPreComp.h"

#include "Stroika/Foundation/Containers/Association.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"

/**
 *  \file
 *
 *  \note Code-Status:  <a href="Code-Status.md#Beta">Beta</a>
 *
 */

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an Array-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     * \note Runtime performance/complexity:
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_Array
        : public Private::ArrayBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true> {
    private:
        using inherited =
            Private::ArrayBasedContainer<Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>, true>;

    public:
        using KeyEqualsCompareFunctionType = typename inherited::KeyEqualsCompareFunctionType;
        using value_type                   = typename inherited::value_type;
        using mapped_type                  = typename inherited::mapped_type;

    public:
        /**
         *  \see docs on Association<> constructor
         */
        Association_Array ();
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        explicit Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer);
        Association_Array (Association_Array&&) noexcept      = default;
        Association_Array (const Association_Array&) noexcept = default;
        Association_Array (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src);
        template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
            requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Association_Array<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        explicit Association_Array (ITERABLE_OF_ADDABLE&& src)
#if qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
            : Association_Array{}
        {
            if constexpr (Configuration::IHasSizeMethod<ITERABLE_OF_ADDABLE>) {
                this->reserve (src.size ());
            }
            this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
            AssertRepValidType_ ();
        }
#endif
        ;
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src);
        template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);
        template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
        Association_Array (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end);

    public:
        nonvirtual Association_Array& operator= (Association_Array&&) noexcept = default;
        nonvirtual Association_Array& operator= (const Association_Array&)     = default;

    private:
        using IImplRepBase_ = Containers::Private::ArrayBasedContainerIRep<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep>;
        template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IEqualsComparer<KEY_TYPE>) KEY_EQUALS_COMPARER>
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

#include "Association_Array.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_Array_h_ */
