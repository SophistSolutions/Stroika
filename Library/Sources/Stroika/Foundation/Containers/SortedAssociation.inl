/*
* Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
*/
#ifndef _Stroika_Foundation_Containers_SortedAssociation_inl_
#define _Stroika_Foundation_Containers_SortedAssociation_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedAssociation_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
            ********************************************************************************
            ************* SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE> ***********************
            ********************************************************************************
            */
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation ()
                : inherited (move (Factory::SortedAssociation_Factory<KEY_TYPE, MAPPED_VALUE_TYPE> () ()))
            {
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
                : SortedAssociation ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
                : SortedAssociation ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename CONTAINER_OF_ADDABLE, typename ENABLE_IF>
            inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const CONTAINER_OF_ADDABLE& src)
                : SortedAssociation ()
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename COPY_FROM_ITERATOR_KEY_T>
            SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (COPY_FROM_ITERATOR_KEY_T start, COPY_FROM_ITERATOR_KEY_T end)
                : SortedAssociation ()
            {
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (const _SortedAssociationRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation (_SortedAssociationRepSharedPtr&& src) noexcept
                : inherited (move (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline void SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedAssociation_inl_ */
