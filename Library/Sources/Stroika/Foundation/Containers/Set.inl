/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    template <typename ID>
    auto Set<T>::mkComparerByIDExtract (const function<ID (T)>& idExtractor) -> ElementEqualityComparerType
    {
        return [idExtractor] (const T& lhs, const T& rhs) { return idExtractor (lhs) == idExtractor (rhs); };
    }
    template <typename T>
    inline Set<T>::Set ()
        : Set{equal_to<T>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer)
        // Use inherited() to avoid matching inherited(initializer_list<>... - see docs in inherited::CTORs...
        : inherited (Factory::Set_Factory<T, EQUALS_COMPARER> (forward<EQUALS_COMPARER> (equalsComparer)) ())
    {
        static_assert (Common::IsEqualsComparer<EQUALS_COMPARER> (), "Set constructor with EQUALS_COMPARER - comparer not valid EqualsComparer- see ComparisonRelationDeclaration<Common::ComparisonRelationType::eEquals, function<bool(T, T)>");
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const initializer_list<T>& src)
        : Set{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> ()>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, const initializer_list<T>& src)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<Set<T>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Set<T>::Set (CONTAINER_OF_ADDABLE&& src)
        : Set{}
    {
        AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, const CONTAINER_OF_ADDABLE& src)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const _SetRepSharedPtr& src) noexcept
        : inherited{src}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (_SetRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    inline Set<T>::Set (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Set{}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<T, EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
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
    inline auto Set<T>::GetElementEqualsComparer () const -> ElementEqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementEqualsComparer ();
    }
    template <typename T>
    inline bool Set<T>::Contains (ArgByValueType<T> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Contains (item);
    }
    template <typename T>
    bool Set<T>::Contains (const Iterable<T>& items) const
    {
        for (auto i : items) {
            if (not Contains (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    bool Set<T>::ContainsAll (const Iterable<T>& items) const
    {
        for (auto i : items) {
            if (not Contains (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    bool Set<T>::ContainsAny (const Iterable<T>& items) const
    {
        for (auto i : items) {
            if (Contains (i)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    bool Set<T>::IsSubsetOf (const Set& superset) const
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
         *  if someone calls AddIf() - they don't know/care if this call or another at the same time is doing the
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
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline void Set<T>::AddAll (CONTAINER_OF_ADDABLE&& s)
    {
        // Note - unlike Collection<T> - we don't need to check for this != &s because if we
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
         *  if someone calls RemoveIf() - they don't know/care if this call or another at the same time is doing the
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
    inline Set<T> Set<T>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const
    {
        return Iterable<T>::Where (includeIfTrue, Set<T>{});
    }
    template <typename T>
    bool Set<T>::Intersects (const Iterable<T>& rhs) const
    {
        for (T i : rhs) {
            if (Contains (i)) [[UNLIKELY_ATTR]] {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    inline bool Set<T>::Intersects (const Set& lhs, const Iterable<T>& rhs)
    {
        return lhs.Intersects (rhs);
    }
    template <typename T>
    inline bool Set<T>::Intersects (const Iterable<T>& lhs, const Set& rhs)
    {
        return rhs.Intersects (lhs);
    }
    template <typename T>
    inline bool Set<T>::Intersects (const Set& lhs, const Set& rhs)
    {
        // intersection operation is commutitive; and O(N) in side we iterate over, and < O(N) in side we do lookups
        // on, so do iteration on shorter side
        if (lhs.size () < rhs.size ()) {
            return rhs.Intersects (static_cast<const Iterable<T>&> (lhs));
        }
        else {
            return lhs.Intersects (static_cast<const Iterable<T>&> (rhs));
        }
    }
    template <typename T>
    Set<T> Set<T>::Intersection (const Iterable<T>& rhs) const
    {
        using namespace Stroika::Foundation::Common;
        Set<T> result{this->GetElementEqualsComparer ()};
        for (T i : rhs) {
            if (Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    inline Set<T> Set<T>::Intersection (const Set& lhs, const Iterable<T>& rhs)
    {
        return lhs.Intersection (rhs);
    }
    template <typename T>
    inline Set<T> Set<T>::Intersection (const Iterable<T>& lhs, const Set& rhs)
    {
        return rhs.Intersection (lhs);
    }
    template <typename T>
    inline Set<T> Set<T>::Intersection (const Set& lhs, const Set& rhs)
    {
        // Require (lhs.GetElementEqualsComparer () == rhs.GetElementEqualsComparer ());  Not LITERALLY required, but results undefined if these comparers produce diffrent answers
        // union operation is commutitive; and O(N) in side we iterate over, and < O(N) in side we do lookups
        // on, so do iteration on shorter side
        if (lhs.size () < rhs.size ()) {
            return rhs.Intersection (static_cast<const Iterable<T>&> (lhs));
        }
        else {
            return lhs.Intersection (static_cast<const Iterable<T>&> (rhs));
        }
    }
    template <typename T>
    inline Set<T> Set<T>::Union (const Iterable<T>& rhs) const
    {
        Set r = *this;
        r.AddAll (rhs);
        return r;
    }
    template <typename T>
    inline Set<T> Set<T>::Union (ArgByValueType<T> rhs) const
    {
        Set r = *this;
        r.Add (rhs);
        return r;
    }
    template <typename T>
    Set<T> Set<T>::Union (const Set& lhs, const Iterable<T>& rhs)
    {
        return lhs.Union (rhs);
    }
    template <typename T>
    Set<T> Set<T>::Union (const Iterable<T>& lhs, const Set& rhs)
    {
        // union operation is commutitive
        return rhs.Union (lhs);
    }
    template <typename T>
    inline Set<T> Set<T>::Union (const Set& lhs, const Set& rhs)
    {
        // Require (lhs.GetElementEqualsComparer () == rhs.GetElementEqualsComparer ());  Not LITERALLY required, but results undefined if these comparers produce diffrent answers
        // union operation is commutitive; and O(N) in side we iterate over, and < O(N) in side we do lookups
        // on, so do iteration on shorter side
        if (lhs.size () < rhs.size ()) {
            return rhs.Union (static_cast<const Iterable<T>&> (lhs));
        }
        else {
            return lhs.Union (static_cast<const Iterable<T>&> (rhs));
        }
    }
    template <typename T>
    Set<T> Set<T>::Difference (const Set& rhs) const
    {
        using namespace Stroika::Foundation::Common;
        Set result{this->GetElementEqualsComparer ()};
        /*
         *  whether its better to incrementally create the result set, or better to incrementally remove
         * from the result set (performance wise) (probably) depends on how close *this and rhs are to one another.
         */
        for (T i : *this) {
            if (not rhs.Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    Set<T> Set<T>::Difference (const Iterable<T>& rhs) const
    {
        /*
         * We could iterate (doubly nested loop) over both *this and rhs, and that would avoid constructing
         * an intermediate object. However, in the INNER loop, we would do an O(N) operation, and this way we
         * probably have an (O(Log(N)) operation in the inner loop. At least for larger containers, that
         * makes sense (except for requiring more memory temporarily). For smaller ones, it probably doesn't matter.
         *
         * OR - maybe better, could CLONE this, and iterate over rhs, and just remove els as needed. for special case where nothing removed
         * that would be faster (because of COW), and maybe in general faster.
         *
         *  OR return Difference (Set{this->GetElementEqualsComparer (), rhs});
         */
        Set<T> result = *this;
        for (auto i : rhs) {
            result.Remove (i);
        }
        return result;
    }
    template <typename T>
    Set<T> Set<T>::Difference (ArgByValueType<T> rhs) const
    {
        /*
         *  Could implement as return Difference (Set{rhs});
         *  But due to COW in SharedByValue, this is better if 'rhs' not found in this.
         */
        Set<T> result = *this;
        result.Remove (rhs);
        return result;
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
        [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
#endif
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T>
    inline bool Set<T>::operator== (const Set& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    template <typename T>
    inline bool Set<T>::operator== (const Iterable<T>& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
#endif

    /*
     ********************************************************************************
     ******************************** Set<T>::_IRep *********************************
     ********************************************************************************
     */
    template <typename T>
    bool Set<T>::_IRep::_Equals_Reference_Implementation (const typename Iterable<T>::_IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        if (this->GetLength () != rhs.GetLength ()) {
            return false;
        }
        // Note - no need to iterate over rhs because we checked sizes the same
        for (auto i = rhs.MakeIterator (&rhs); not i.Done (); ++i) {
            if (not Contains (*i)) {
                return false;
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     ************************* Set<T>::EqualsComparer *******************************
     ********************************************************************************
     */
    template <typename T>
    inline bool Set<T>::EqualsComparer::operator() (const Set& lhs, const Set& rhs) const
    {
        bool result = _SafeReadRepAccessor<_IRep>{&lhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
        Ensure (result == _SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&lhs}._ConstGetRep ())); // if this fails caller probably provided incompatible LHS/RHS comparers
        return result;
    }
    template <typename T>
    inline bool Set<T>::EqualsComparer::operator() (const Set& lhs, const Iterable<T>& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{&lhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<typename Iterable<T>::_IRep>{&rhs}._ConstGetRep ());
    }
    template <typename T>
    inline bool Set<T>::EqualsComparer::operator() (const Iterable<T>& lhs, const Set& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<typename Iterable<T>::_IRep>{&lhs}._ConstGetRep ());
    }

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ************************* Set<T> comparison operators **************************
     ********************************************************************************
     */
    template <typename T>
    inline bool operator== (const Set<T>& lhs, const Set<T>& rhs)
    {
        return typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator== (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator== (const Iterable<T>& lhs, const Set<T>& rhs)
    {
        return typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const Set<T>& lhs, const Set<T>& rhs)
    {
        return not typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const Set<T>& lhs, const Iterable<T>& rhs)
    {
        return not typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T>
    inline bool operator!= (const Iterable<T>& lhs, const Set<T>& rhs)
    {
        return not typename Set<T>::EqualsComparer{}(lhs, rhs);
    }
#endif

    /*
     ********************************************************************************
     ******************************* Set<T> operators *******************************
     ********************************************************************************
     */
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
    inline Set<T> operator- (const Set<T>& lhs, const Iterable<T>& rhs)
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
    template <typename T>
    inline Set<T> operator^ (const Iterable<T>& lhs, const Set<T>& rhs)
    {
        return rhs.Intersection (lhs); // intersection is commutative
    }
    template <typename T>
    inline Set<T> operator^ (const Set<T>& lhs, const Set<T>& rhs)
    {
        return lhs.Intersection (rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_Set_inl_ */
