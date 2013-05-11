/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedBag_inl_
#define _Stroika_Foundation_Containers_SortedBag_inl_

#include    "../Debug/Assertions.h"

#include    "Concrete/SortedBag_Factory.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /*
             ********************************************************************************
             ************************************ SortedBag<T> ******************************
             ********************************************************************************
             */
            template    <typename typename T>
            inline  SortedBag<T>::SortedBag ()
                : inherited (Concrete::mkSortedBag_Default<T> ())
            {
            }
            template    <typename typename T>
            inline  SortedBag<T>::SortedBag (const SortedBag<T>& sb)
                : inherited (static_cast<const inherited&> (sb))
            {
            }
            template    <typename typename T>
            inline  SortedBag<T>::SortedBag (const _SharedPtrIRep& rep)
                : inherited (typename inherited::_SharedPtrIRep (rep))
            {
                RequireNotNull (rep);
                EnsureMember (&inherited::_GetRep (), _IRep);
            }
            template    <typename typename T>
            inline  SortedBag<T>&   SortedBag<T>::operator= (const SortedBag<T>& rhs)
            {
                inherited::operator= (static_cast<const inherited&> (rhs));
                EnsureMember (&inherited::_GetRep (), _IRep);
                return *this;
            }


        }
    }
}



#endif /* _Stroika_Foundation_Containers_SortedBag_inl_ */
