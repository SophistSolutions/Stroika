/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                /**
                 *  \brief   Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE, TRAITS> is an std::map-based concrete implementation of the Association<KEY_TYPE, MAPPED_VALUE_TYPE, typename TRAITS::AssociationTraitsType> container pattern.
                 *
                 *  \note   \em Thread-Safety   <a href="thread_safety.html#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
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
                    */
                    Association_stdmultimap ();
                    Association_stdmultimap (const Association_stdmultimap& src) = default;
                    template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF = typename enable_if<Configuration::has_beginend<CONTAINER_OF_PAIR_KEY_T>::value && !std::is_convertible<const CONTAINER_OF_PAIR_KEY_T*, const Association_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>*>::value>::type>
                    explicit Association_stdmultimap (const CONTAINER_OF_PAIR_KEY_T& src);
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
        }
    }
}

/*
********************************************************************************
******************************* Implementation Details *************************
********************************************************************************
*/
#include "Association_stdmultimap.inl"

#endif /*_Stroika_Foundation_Containers_Concrete_Association_stdmultimap_h_ */
