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
        // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
        : inherited (Factory::Collection_Factory<T>{}())
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline Collection<T>::Collection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Collection{}
    {
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
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Collection<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Collection<T>::Collection (CONTAINER_OF_ADDABLE&& src)
        : Collection{}
    {
        AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    bool Collection<T>::Contains (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer) const
    {
        for (const auto i : *this) {
            if (equalsComparer (i, item)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Collection<T>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = start; i != end; ++i) {
            tmp._GetWriteableRep ().Add (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline void Collection<T>::AddAll (CONTAINER_OF_ADDABLE&& items)
    {
        if constexpr (std::is_convertible_v<decay_t<CONTAINER_OF_ADDABLE>*, Collection<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) {
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
    inline void Collection<T>::Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Update (i, newValue);
    }
    template <typename T>
    template <typename EQUALS_COMPARER>
    inline bool Collection<T>::Remove (ArgByValueType<value_type> item, const EQUALS_COMPARER& equalsComparer)
    {
        for (Iterator<value_type> i = this->begin (); i != this->end (); ++i) {
            if (equalsComparer (*i, item)) {
                _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i, nullptr);
                return true;
            }
        }
        return false;
    }
    template <typename T>
    inline void Collection<T>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty ());
        }
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, typename EQUALS_COMPARER>
    void Collection<T>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, const EQUALS_COMPARER& equalsComparer)
    {
        for (auto i = start; i != end; ++i) {
            Remove (*i, equalsComparer);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, typename EQUALS_COMPARER, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline void Collection<T>::RemoveAll (const CONTAINER_OF_ADDABLE& c, const EQUALS_COMPARER& equalsComparer)
    {
        if (static_cast<const void*> (this) == static_cast<const void*> (addressof (c))) {
            RemoveAll (equalsComparer);
        }
        else {
            RemoveAll (std::begin (c), std::end (c), equalsComparer);
        }
    }
    template <typename T>
    template <typename PREDICATE>
    nonvirtual size_t Collection<T>::RemoveAll (const PREDICATE& p)
    {
        size_t nRemoved{};
        for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
            if (p (*i)) {
                Remove (i);
                nRemoved++;
            }
        }
        return nRemoved;
    }
    template <typename T>
    inline void Collection<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWriterRepAndPatchAssociatedIterator (i);
        Debug::UncheckedDynamicCast<_IRep*> (writerRep.get ())->Remove (patchedIterator, nextI);
    }
    template <typename T>
    template <typename PREDICATE>
    nonvirtual bool Collection<T>::Remove (const PREDICATE& p)
    {
        for (Iterator<T> i = this->begin (); i != this->end (); ++i) {
            if (p (*i)) {
                Remove (i);
                return true;
            }
        }
        return false;
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
        Remove (item, equalsComparer);
    }
    template <typename T>
    inline auto Collection<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<T> nextI{nullptr};
        Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    inline Collection<T> Collection<T>::Where (const function<bool (ArgByValueType<value_type>)>& doToElement) const
    {
        return Iterable<T>::Where (doToElement, Collection<T>{});
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
    auto Collection<T>::_GetWriterRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<typename inherited::_SharedByValueRepType::shared_ptr_type, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        using shared_ptr_type                = typename inherited::_SharedByValueRepType::shared_ptr_type;
        Iterator<value_type> patchedIterator = i;
        shared_ptr_type      writerRep       = this->_fRep.rwget_ptr (
            [&] (const element_type& prevRepPtr) -> shared_ptr_type {
                return Debug::UncheckedDynamicCast<const _IRep*> (&prevRepPtr)->CloneAndPatchIterator (&patchedIterator);
            });
        return make_tuple (writerRep, patchedIterator);
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
