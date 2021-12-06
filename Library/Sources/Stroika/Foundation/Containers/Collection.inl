/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Collection_inl_
#define _Stroika_Foundation_Containers_Collection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */

#include "../Debug/Assertions.h"

#include "Factory/Collection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************************** Collection<T> *********************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection<T>::Collection ()
        : inherited{Factory::Collection_Factory<value_type>{}()}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline Collection<T>::Collection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Collection{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<COPY_FROM_ITERATOR_OF_ADDABLE>>);
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Collection<T>::Collection (const initializer_list<value_type>& src)
        : Collection{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<
                                                 Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>
#if qCompilerAndStdLib_template_enableIf_Addable_UseBroken_Buggy
                                                 and is_convertible_v<ExtractValueType_t<CONTAINER_OF_ADDABLE>, T>
#else
                                                 and Collection<T>::template IsAddable_v<ExtractValueType_t<CONTAINER_OF_ADDABLE>>
#endif
                                                 and not is_base_of_v<Collection<T>, decay_t<CONTAINER_OF_ADDABLE>>>*>
    inline Collection<T>::Collection (CONTAINER_OF_ADDABLE&& src)
        : Collection{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<CONTAINER_OF_ADDABLE>>); // redundant (enable_if_t) but for doc purpose
        AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline bool Collection<T>::Contains (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer) const
    {
        return this->Find (item, equalsComparer) != nullptr;
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Collection<T>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<COPY_FROM_ITERATOR_OF_ADDABLE>>);
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = start; i != end; ++i) {
            tmp._GetWriteableRep ().Add (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline void Collection<T>::AddAll (CONTAINER_OF_ADDABLE&& items)
    {
        static_assert (IsAddable_v<ExtractValueType_t<CONTAINER_OF_ADDABLE>>);
        if constexpr (std::is_convertible_v<decay_t<CONTAINER_OF_ADDABLE>*, Collection<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[UNLIKELY_ATTR]] {
                vector<value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                AddAll (std::begin (copy), std::end (copy));
                return;
            }
        }
        AddAll (std::begin (items), std::end (items));
    }
    template <typename T>
    inline void Collection<T>::Add (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item);
        Ensure (not this->IsEmpty ());
    }
    template <typename T>
    inline void Collection<T>::Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename T>
    inline void Collection<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void Collection<T>::Remove (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer)
    {
        auto i = this->Find (item, equalsComparer);
        Require (i != this->end ()); // use remove-if if the item might not exist
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, nullptr);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline bool Collection<T>::RemoveIf (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer)
    {
        if (auto i = this->Find (item, equalsComparer)) {
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, nullptr);
            return true;
        }
        return false;
    }
    template <typename T>
    template <typename PREDICATE>
    bool Collection<T>::RemoveIf (const PREDICATE& p)
    {
        if (auto i = this->Find (p)) {
            Remove (i);
            return true;
        }
        return false;
    }
    template <typename T>
    inline void Collection<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    size_t Collection<T>::RemoveAll (const Iterator<value_type>& start, const Iterator<value_type>& end, const EQUALS_COMPARER& equalsComparer)
    {
        size_t cnt{};
        for (auto i = start; i != end;) {
            if (RemoveIf (*i, equalsComparer, &i)) {
                ++cnt;
            }
        }
        return cnt;
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>*>
    inline size_t Collection<T>::RemoveAll (const CONTAINER_OF_ADDABLE& c, const EQUALS_COMPARER& equalsComparer)
    {
        static_assert (IsAddable_v<ExtractValueType_t<CONTAINER_OF_ADDABLE>>);
        if (static_cast<const void*> (this) == static_cast<const void*> (addressof (c))) {
            return RemoveAll (equalsComparer);
        }
        else {
            return RemoveAll (std::begin (c), std::end (c), equalsComparer);
        }
    }
    template <typename T>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<T, PREDICATE> ()>*>
    size_t Collection<T>::RemoveAll (const PREDICATE& p)
    {
        size_t nRemoved{};
        for (Iterator<T> i = this->begin (); i != this->end ();) {
            if (p (*i)) {
                Remove (i, &i);
                ++nRemoved;
            }
            else {
                ++i;
            }
        }
        return nRemoved;
    }
    template <typename T>
    inline void Collection<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline void Collection<T>::erase (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer)
    {
        Remove (item, forward<EQUALS_COMPARER> (equalsComparer));
    }
    template <typename T>
    inline auto Collection<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> nextI{nullptr};
        Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    inline Collection<T> Collection<T>::Where (const function<bool (ArgByValueType<value_type>)>& doToElement) const
    {
        return Iterable<value_type>::Where (doToElement, Collection<value_type>{});
    }
    template <typename T>
    inline auto Collection<T>::operator+= (ArgByValueType<value_type> item) -> Collection&
    {
        Add (item);
        return *this;
    }
    template <typename T>
    inline auto Collection<T>::operator+= (const Iterable<value_type>& items) -> Collection&
    {
        AddAll (items);
        return *this;
    }
    template <typename T>
    auto Collection<T>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type*        writableRep     = this->_fRep.rwget (
            [&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
                return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
            });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), patchedIterator);
    }
    template <typename T>
    inline void Collection<T>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

    /*
     ********************************************************************************
     ********************************* operator+ ************************************
     ********************************************************************************
     */
    template <typename T>
    Collection<T> operator+ (const Iterable<T>& lhs, const Collection<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Iterable<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }
    template <typename T>
    Collection<T> operator+ (const Collection<T>& lhs, const Collection<T>& rhs)
    {
        Collection<T> result{lhs};
        result += rhs;
        return result;
    }

}

#endif /* _Stroika_Foundation_Containers_Collection_inl_ */
