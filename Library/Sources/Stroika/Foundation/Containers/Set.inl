/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_inl_
#define _Stroika_Foundation_Containers_Set_inl_

#include "../Common/Compare.h"
#include "../Debug/Assertions.h"
#include "Factory/Set_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ************************************* Set<T> ***********************************
     ********************************************************************************
     */
    template <typename T>
    inline Set<T>::Set ()
        : Set (equal_to<T>{})
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer)
        : inherited (move (Factory::Set_Factory<T, EQUALS_COMPARER> (forward<EQUALS_COMPARER> (equalsComparer)) ()))
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Set constructor with EQUALS_COMPARER - comparer not valid EqualsComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(T, T)>");
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const initializer_list<T>& src)
        : Set ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : Set (forward<EQUALS_COMPARER> (equalsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Set<T>*>>*>
    inline Set<T>::Set (const CONTAINER_OF_ADDABLE& src)
        : Set ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Set<T>*>>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, const CONTAINER_OF_ADDABLE& src)
        : Set (forward<EQUALS_COMPARER> (equalsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const _SetRepSharedPtr& src) noexcept
        : inherited (src)
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (_SetRepSharedPtr&& src) noexcept
        : inherited ((RequireNotNull (src), move (src)))
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    inline Set<T>::Set (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Set ()
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Set (forward<EQUALS_COMPARER> (equalsComparer))
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
#if qDebug
    template <typename T>
    Set<T>::~Set ()
    {
        if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
            // SharingState can be NULL because of MOVE semantics
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
        }
    }
#endif
    template <typename T>
    inline auto Set<T>::GetEqualsComparer () const -> EqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetEqualsComparer ();
    }
    template <typename T>
    inline bool Set<T>::Contains (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
    }
    template <typename T>
    bool Set<T>::IsSubsetOf (const Set<T>& superset) const
    {
        for (auto i : *this) {
            if (not superset.Contains (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    inline optional<T> Set<T>::Lookup (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (item);
    }
    template <typename T>
    inline void Set<T>::Add (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item);
    }
    template <typename T>
    inline bool Set<T>::AddIf (ArgByValueType<T> item)
    {
        /*
         *  Note, this is an non-performance optimal implementation, but is not a race, because from the outside
         *  if someone calls AddIf() - they dont know/care if this call or another at the same time is doing the
         *  addition. Any 'race' would be in the logical of the calling code.
         */
        if (Contains (item)) {
            return false;
        }
        else {
            Add (item);
            return true;
        }
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Set<T>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = start; i != end; ++i) {
            tmp._GetWriteableRep ().Add (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_ADDABLE>>*>
    inline void Set<T>::AddAll (const CONTAINER_OF_ADDABLE& s)
    {
        // Note - unlike Collection<T, TRAITS> - we dont need to check for this != &s because if we
        // attempt to add items that already exist, it would do nothing, and not lead to
        // an infinite loop
        AddAll (std::begin (s), std::end (s));
    }
    template <typename T>
    inline void Set<T>::Remove (ArgByValueType<T> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (item);
    }
    template <typename T>
    inline void Set<T>::Remove (const Iterator<T>& i)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
    }
    template <typename T>
    inline bool Set<T>::RemoveIf (ArgByValueType<T> item)
    {
        /*
         *  Note, this is an non-performance optimal implementation, but is not a race, because from the outside
         *  if someone calls RemoveIf() - they dont know/care if this call or another at the same time is doing the
         *  addition. Any 'race' would be in the logical of the calling code.
         */
        if (Contains (item)) {
            Remove (item);
            return true;
        }
        else {
            return false;
        }
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Set<T>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Remove (*i);
        }
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE>
    inline void Set<T>::RemoveAll (const CONTAINER_OF_ADDABLE& s)
    {
        RemoveAll (std::begin (s), std::end (s));
    }
    template <typename T>
    inline void Set<T>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
        }
    }
    template <typename T>
    inline bool Set<T>::Equals (const Set<T>& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }
    template <typename T>
    inline bool Set<T>::Equals (const Iterable<T>& rhs) const
    {
        // KISS for now
        return Equals (Set<T>{rhs});
    }
    template <typename T>
    inline Set<T> Set<T>::Where (const function<bool(ArgByValueType<T>)>& includeIfTrue) const
    {
        return Iterable<T>::Where (includeIfTrue, Set<T>{});
    }
    template <typename T>
    bool Set<T>::Intersects (const Iterable<T>& rhs) const
    {
        for (T i : rhs) {
            if (Contains (i))
                [[UNLIKELY_ATTR]]
                {
                    return true;
                }
        }
        return false;
    }
    template <typename T>
    Set<T> Set<T>::Intersection (const Iterable<T>& rhs) const
    {
        using namespace Stroika::Foundation::Common;
        Set<T> result{this->GetEqualsComparer ()};
        for (T i : rhs) {
            if (Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    inline Set<T> Set<T>::Union (const Iterable<T>& rhs) const
    {
        Set<T> r = *this;
        r.AddAll (rhs);
        return r;
    }
    template <typename T>
    inline Set<T> Set<T>::Union (ArgByValueType<T> rhs) const
    {
        Set<T> r = *this;
        r.Add (rhs);
        return r;
    }
    template <typename T>
    Set<T> Set<T>::Difference (const Set<T>& rhs) const
    {
        using namespace Stroika::Foundation::Common;
        Set<T> result{this->GetEqualsComparer ()};
        for (T i : *this) {
            if (not rhs.Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    Set<T> Set<T>::Difference (ArgByValueType<T> rhs) const
    {
        return Difference (Set<T>{rhs});
    }
    template <typename T>
    inline Set<T>& Set<T>::operator+= (ArgByValueType<T> item)
    {
        Add (item);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator+= (const Iterable<T>& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator-= (ArgByValueType<T> item)
    {
        Remove (item);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator-= (const Iterable<T>& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator^= (const Iterable<T>& items)
    {
        *this = Intersection (items);
        return *this;
    }
    template <typename T>
    inline void Set<T>::clear ()
    {
        RemoveAll ();
    }
    template <typename T>
    inline void Set<T>::insert (ArgByValueType<T> item)
    {
        Add (item);
    }
    template <typename T>
    inline void Set<T>::erase (ArgByValueType<T> item)
    {
        Remove (item);
    }
    template <typename T>
    inline void Set<T>::erase (const Iterator<T>& i)
    {
        Remove (i);
    }
    template <typename T>
    inline void Set<T>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

    /*
     ********************************************************************************
     ******************************** Set<T>::_IRep *********************************
     ********************************************************************************
     */
    template <typename T>
    bool Set<T>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        if (this->GetLength () != rhs.GetLength ()) {
            return false;
        }
        // Note - no need to iterate over rhs because we checked sizes the same
        for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
            if (not rhs.Contains (*i)) {
                return false;
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     ******************************* Set<T> operators *******************************
     ********************************************************************************
     */
    template <typename T>
    inline bool operator== (const Set<T>& lhs, const Set<T>& rhs)
    {
        return lhs.Equals (rhs);
    }
    template <typename T>
    inline bool operator== (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return lhs.Equals (rhs);
    }
    template <typename T>
    inline bool operator== (const Iterable<T>& lhs, const Set<T>& rhs)
    {
        return rhs.Equals (lhs);
    }
    template <typename T>
    inline bool operator!= (const Set<T>& lhs, const Set<T>& rhs)
    {
        return not lhs.Equals (rhs);
    }
    template <typename T>
    inline bool operator!= (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return not lhs.Equals (rhs);
    }
    template <typename T>
    inline bool operator!= (const Iterable<T>& lhs, const Set<T>& rhs)
    {
        return not rhs.Equals (lhs);
    }
    template <typename T>
    inline Set<T> operator+ (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return lhs.Union (rhs);
    }
    template <typename T>
    inline Set<T> operator+ (const Set<T>& lhs, const T& rhs)
    {
        return lhs.Union (rhs);
    }
    template <typename T>
    inline Set<T> operator- (const Set<T>& lhs, const Set<T>& rhs)
    {
        return lhs.Difference (rhs);
    }
    template <typename T>
    inline Set<T> operator- (const Set<T>& lhs, const T& rhs)
    {
        return lhs.Difference (rhs);
    }
    template <typename T>
    inline Set<T> operator^ (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return lhs.Intersection (rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_Set_inl_ */
