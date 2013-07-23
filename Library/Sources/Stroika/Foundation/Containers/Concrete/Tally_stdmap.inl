/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SortedTally_stdmap.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                ************************** Tally_stdmap<T, TRAITS> *****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap ()
                    : inherited (static_cast<const inherited&> (SortedTally_stdmap<T, TRAITS> ()))
                {
                }
                template    <typename T, typename TRAITS>
                Tally_stdmap<T, TRAITS>::Tally_stdmap (const Tally_stdmap<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Tally_stdmap<T, TRAITS>::Tally_stdmap (const CONTAINER_OF_T& src)
                    : inherited (static_cast<const inherited&> (SortedTally_stdmap<T, TRAITS> ()))
                {
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                inline  Tally_stdmap<T, TRAITS>&   Tally_stdmap<T, TRAITS>::operator= (const Tally_stdmap<T, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Tally_stdmap_inl_ */
