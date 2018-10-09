/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_SortedMapping_inl_
#define _Stroika_Foundation_Containers_SortedMapping_inl_

#include "../Debug/Assertions.h"
#include "Factory/SortedMapping_Factory.h"

namespace Stroika::Foundation::Containers {

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
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited (move (Factory::SortedMapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_INORDER_COMPARER> (forward<KEY_INORDER_COMPARER> (inorderComparer)) ()))
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
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedMapping (forward<KEY_INORDER_COMPARER> (inorderComparer))
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
    template <typename KEY_INORDER_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> ()>*>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedMapping (forward<KEY_INORDER_COMPARER> (inorderComparer))
    {
        this->AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<(Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> or Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>)and not is_base_of_v<SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (CONTAINER_OF_ADDABLE&& src)
        : SortedMapping ()
    {
        _AssertRepValidType ();
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and (Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> or Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>) and not is_base_of_v<SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, CONTAINER_OF_ADDABLE&& src)
        : SortedMapping (forward<KEY_INORDER_COMPARER> (inorderComparer))
    {
        _AssertRepValidType ();
        this->AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedMapping ()
    {
        this->AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_INORDER_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_INORDER_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping (KEY_INORDER_COMPARER&& inorderComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : SortedMapping (forward<KEY_INORDER_COMPARER> (inorderComparer))
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

#endif /* _Stroika_Foundation_Containers_SortedMapping_inl_ */
