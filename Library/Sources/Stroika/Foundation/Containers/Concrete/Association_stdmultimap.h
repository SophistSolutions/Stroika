/*
* Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
*/
#include "../../StroikaPreComp.h"

#include "../Association.h"

#ifndef _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_
#define _Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_

/**
*  \file
*
*  \version    <a href="Code-Status.md#Alpha-Late">Alpha-Early</a>
***VERY ROUGH UNUSABLE DRAFT*
*
*  TODO:
*/

namespace Stroika::Foundation::Containers::Concrete {

    /**
     *  \brief   Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   \em Implementation Details
     *          This module is essentially identical to SortedAssociation_stdmultimap, but making it dependent on SortedAssociation<> creates
     *          problems with circular dependencies - especially give how the default Association CTOR calls the factory class
     *          which maps back to the _stdmultimap<> variant.
     *
     *          There maybe another (better) way, but this works.
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class Association_stdmultimap : public Association<KEY_TYPE, MAPPED_VALUE_TYPE> {
    private:
        using inherited = Association<KEY_TYPE, MAPPED_VALUE_TYPE>;

    public:
        /**
        */
        //using KeyWellOrderCompareFunctionType = typename TraitsType::KeyWellOrderCompareFunctionType;

    public:
        /**
         *  @todo - https://stroika.atlassian.net/browse/STK-652 - add COMPARER constructor overloads like the archtype base class
         */
        Association_stdmultimap ();
        Association_stdmultimap (const Association_stdmultimap& src) = default;
        template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>*>>* = nullptr>
        explicit Association_stdmultimap (const CONTAINER_OF_ADDABLE& src);
        template <typename COPY_FROM_ITERATOR_KEY_T>
        explicit Association_stdmultimap (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end);

    public:
        /**
        */
        nonvirtual Association_stdmultimap& operator= (const Association_stdmultimap& rhs) = default;

    private:
        class IImplRep_;
        class Rep_;

    private:
        nonvirtual void AssertRepValidType_ () const;
    };
}

/*
********************************************************************************
******************************* Implementation Details *************************
********************************************************************************
*/
#include "Association_stdmultimap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_ */
