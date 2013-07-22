/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SortedMapping_stdmap.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                **************** Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ******************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap ()
                    : inherited (static_cast<const inherited&> (SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ()))
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Mapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src)
                    : inherited (static_cast<const inherited&> (SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS> ()))
                {
                    this->AddAll (src);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>&   Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const Mapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Mapping_stdmap_inl_ */
