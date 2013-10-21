/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SortedSet_stdset.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                ********************************************************************************
                **************************** Set_setset<T, TRAITS> *****************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Set_stdset<T, TRAITS>::Set_stdset ()
                    : inherited (static_cast<const inherited&> (SortedSet_stdset<T, TRAITS> ()))
                {
                }
                template    <typename T, typename TRAITS>
                Set_stdset<T, TRAITS>::Set_stdset (const Set_stdset<T, TRAITS>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                }
#if      qCompilerAndStdLib_Supports_initializer_lists
                template    <typename T, typename TRAITS>
                inline  Set_stdset<T, TRAITS>::Set_stdset (const std::initializer_list<T>& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
#endif
                template    <typename T, typename TRAITS>
                template    <typename CONTAINER_OF_T>
                inline  Set_stdset<T, TRAITS>::Set_stdset (const CONTAINER_OF_T& src)
                    : inherited (static_cast<const inherited&> (SortedSet_stdset<T, TRAITS> ()))
                {
                    this->AddAll (src);
                }
                template    <typename T, typename TRAITS>
                inline  Set_stdset<T, TRAITS>&   Set_stdset<T, TRAITS>::operator= (const Set_stdset<T, TRAITS>& rhs)
                {
                    inherited::operator= (rhs);
                    return *this;
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_ */
