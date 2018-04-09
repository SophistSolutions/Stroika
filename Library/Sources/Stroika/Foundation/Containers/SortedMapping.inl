/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_inl_
#define _Stroika_Foundation_Containers_SortedMapping_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedMapping_Factory.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {

            /*
             ********************************************************************************
             ************* SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE> ***********************
             ********************************************************************************
             */
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping ()
                : SortedMapping (less<KEY_TYPE>{})
            {
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename KEY_INORDER_COMPARER, typename ENABLE_IF_IS_COMPARER>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const KEY_INORDER_COMPARER& inorderComparer, ENABLE_IF_IS_COMPARER*)
                : inherited (move (Factory::SortedMapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER> (inorderComparer) ()))
            {
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
                : SortedMapping ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
                : SortedMapping ()
            {
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const CONTAINER_OF_PAIR_KEY_T& src)
                : SortedMapping ()
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename KEY_INORDER_COMPARER, typename CONTAINER_OF_PAIR_KEY_T, typename ENABLE_IF>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const KEY_INORDER_COMPARER& inorderComparer, const CONTAINER_OF_PAIR_KEY_T& src)
                : SortedMapping (inorderComparer)
            {
                _AssertRepValidType ();
                this->AddAll (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            template <typename COPY_FROM_ITERATOR_KEYVALUE>
            SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
                : SortedMapping ()
            {
                this->AddAll (start, end);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (const _SortedMappingRepSharedPtr& src) noexcept
                : inherited (src)
            {
                RequireNotNull (src);
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (_SortedMappingRepSharedPtr&& src) noexcept
                : inherited (move (src))
            {
                //RequireNotNull (src); -- logically required, but we cannot test here, must test before mem-initializers
                _AssertRepValidType ();
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline void SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
            {
#if qDebug
                _SafeReadRepAccessor<_IRep>{this};
#endif
            }
            template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline auto SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::GetInOrderKeyComparer () const -> KeyInOrderKeyComparerType
            {
                return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetInOrderKeyComparer ();
            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
