/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Set_inl_
#define _Stroika_Foundation_Containers_Set_inl_

#include "../Common/Compare.h"
#include "../Configuration/Concepts.h"
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
        : Set{equal_to<value_type>{}}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer)
        : inherited{Factory::Set_Factory<T, decay_t<EQUALS_COMPARER>>::Default () (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const initializer_list<value_type>& src)
        : Set{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Set<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Set<T>::Set (ITERABLE_OF_ADDABLE&& src)
        : Set{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (const shared_ptr<_IRep>& src) noexcept
        : inherited{src}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    inline Set<T>::Set (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename T>
    template <input_iterator ITERATOR_OF_ADDABLE>
    inline Set<T>::Set (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER, input_iterator ITERATOR_OF_ADDABLE>
    inline Set<T>::Set (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T>
    inline auto Set<T>::GetElementEqualsComparer () const -> ElementEqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetElementEqualsComparer ();
    }
    template <typename T>
    inline bool Set<T>::Contains (ArgByValueType<value_type> item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (item, nullptr, nullptr);
    }
    template <typename T>
    bool Set<T>::ContainsAll (const Iterable<value_type>& items) const
    {
        for (const auto& i : items) {
            if (not Contains (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    bool Set<T>::ContainsAny (const Iterable<value_type>& items) const
    {
        for (const auto& i : items) {
            if (Contains (i)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    bool Set<T>::IsSubsetOf (const Set& superset) const
    {
        for (const auto& i : *this) {
            if (not superset.Contains (i)) {
                return false;
            }
        }
        return true;
    }
    template <typename T>
    inline auto Set<T>::Lookup (ArgByValueType<value_type> item) const -> optional<value_type>
    {
        optional<value_type> r;
        _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (item, &r, nullptr);
        return r; // don't move due to return value optimization
    }
    template <typename T>
    inline void Set<T>::Add (ArgByValueType<value_type> item)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (item);
    }
    template <typename T>
    inline bool Set<T>::AddIf (ArgByValueType<value_type> item)
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
    template <input_iterator ITERATOR_OF_ADDABLE>
    void Set<T>::AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE> (end); ++i) {
            tmp._GetWriteableRep ().Add (*i);
        }
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline void Set<T>::AddAll (ITERABLE_OF_ADDABLE&& items)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        if constexpr (std::is_convertible_v<decay_t<ITERABLE_OF_ADDABLE>*, const Iterable<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[unlikely]] {
                vector<value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                AddAll (std::begin (copy), std::end (copy));
                return;
            }
        }
        AddAll (std::begin (items), std::end (items));
    }
    template <typename T>
    inline void Set<T>::Remove (ArgByValueType<value_type> item)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item));
    }
    template <typename T>
    inline void Set<T>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T>
    inline bool Set<T>::RemoveIf (ArgByValueType<value_type> item)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (item);
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline size_t Set<T>::RemoveAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        size_t cnt{};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            ++cnt;
            Remove (*i);
        }
        return cnt;
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE>
    inline size_t Set<T>::RemoveAll (const ITERABLE_OF_ADDABLE& s)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        return RemoveAll (std::begin (s), std::end (s));
    }
    template <typename T>
    inline void Set<T>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<T, PREDICATE> ()>*>
    size_t Set<T>::RemoveAll (PREDICATE&& p)
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
    inline Set<T> Set<T>::Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const
    {
        return Iterable<T>::Where (includeIfTrue, Set<T>{});
    }
    template <typename T>
    bool Set<T>::Intersects (const Iterable<value_type>& rhs) const
    {
        for (const auto& i : rhs) {
            if (Contains (i)) {
                return true;
            }
        }
        return false;
    }
    template <typename T>
    inline bool Set<T>::Intersects (const Set& lhs, const Iterable<value_type>& rhs)
    {
        return lhs.Intersects (rhs);
    }
    template <typename T>
    inline bool Set<T>::Intersects (const Iterable<value_type>& lhs, const Set& rhs)
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
    Set<T> Set<T>::Intersection (const Iterable<value_type>& rhs) const
    {
        using namespace Stroika::Foundation::Common;
        Set<T> result{this->GetElementEqualsComparer ()};
        /*
         * note: TRIED using Apply() - and didn't seem to help performance --LGP 2021-11-08
         */
        for (const auto& i : rhs) {
            if (Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    inline Set<T> Set<T>::Intersection (const Set& lhs, const Iterable<value_type>& rhs)
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
    inline Set<T> Set<T>::Union (const Iterable<value_type>& rhs) const
    {
        Set r = *this;
        r.AddAll (rhs);
        return r;
    }
    template <typename T>
    inline Set<T> Set<T>::Union (ArgByValueType<value_type> rhs) const
    {
        Set r = *this;
        r.Add (rhs);
        return r;
    }
    template <typename T>
    inline Set<T> Set<T>::Union (const Set& lhs, const Iterable<value_type>& rhs)
    {
        return lhs.Union (rhs);
    }
    template <typename T>
    inline Set<T> Set<T>::Union (const Iterable<value_type>& lhs, const Set& rhs)
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
         * 
         * note: TRIED using Apply() - and didn't seem to help performance --LGP 2021-11-08
         */
        for (const auto& i : *this) {
            if (not rhs.Contains (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename T>
    Set<T> Set<T>::Difference (const Iterable<value_type>& rhs) const
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
         * 
         * note: TRIED using Apply() - and didn't seem to help performance --LGP 2021-11-08
         */
        Set<T> result = *this;
        for (const auto& i : rhs) {
            result.RemoveIf (i);
        }
        return result;
    }
    template <typename T>
    Set<T> Set<T>::Difference (ArgByValueType<value_type> rhs) const
    {
        /*
         *  Could implement as return Difference (Set{rhs});
         *  But due to COW in SharedByValue, this is better if 'rhs' not found in this.
         */
        Set<T> result = *this;
        result.RemoveIf (rhs);
        return result;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator+= (ArgByValueType<value_type> item)
    {
        Add (item);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator+= (const Iterable<value_type>& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator-= (ArgByValueType<value_type> item)
    {
        RemoveIf (item);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator-= (const Iterable<value_type>& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename T>
    inline Set<T>& Set<T>::operator^= (const Iterable<value_type>& items)
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
    inline auto Set<T>::find (ArgByValueType<value_type> item) const -> Iterator<value_type>
    {
        Iterator<value_type> r{nullptr};
        _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (item, nullptr, &r);
        return move (r);
    }
    template <typename T>
    inline void Set<T>::insert (ArgByValueType<value_type> item)
    {
        Add (item);
    }
    template <typename T>
    inline void Set<T>::erase (ArgByValueType<value_type> item)
    {
        Remove (item);
    }
    template <typename T>
    inline auto Set<T>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<T> nextI{nullptr};
        this->Remove (i, &nextI);
        return nextI;
    }
    template <typename T>
    auto Set<T>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        Iterator<value_type> patchedIterator = i;
        element_type* writableRep = this->_fRep.rwget ([&] (const element_type& prevRepPtr) -> typename inherited::_SharedByValueRepType::shared_ptr_type {
            return Debug::UncheckedDynamicCast<const _IRep&> (prevRepPtr).CloneAndPatchIterator (&patchedIterator);
        });
        AssertNotNull (writableRep);
        return make_tuple (Debug::UncheckedDynamicCast<_IRep*> (writableRep), move (patchedIterator));
    }
    template <typename T>
    inline void Set<T>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename T>
    inline bool Set<T>::operator== (const Set& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    template <typename T>
    inline bool Set<T>::operator== (const Iterable<value_type>& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ******************************** Set<T>::_IRep *********************************
     ********************************************************************************
     */
    template <typename T>
    bool Set<T>::_IRep::_Equals_Reference_Implementation (const typename Iterable<value_type>::_IRep& rhs) const
    {
        // for some reps, this is fast (vector, std::map), but others its quite slow, and not that great a test.
        // so for now, just iterate to see
        constexpr bool kCanUseGetLengthAsOptimizationCheck_ = false;
        if (this == &rhs) {
            return true;
        }
        if (kCanUseGetLengthAsOptimizationCheck_) {
            if (this->size () != rhs.size ()) {
                return false;
            }
        }
        // Note - no need to iterate over rhs because we checked sizes the same
        [[maybe_unused]] size_t rhsSize{};
        for (auto i = rhs.MakeIterator (nullptr); not i.Done (); ++i) {
            if (not Lookup (*i, nullptr, nullptr)) {
                return false;
            }
            ++rhsSize;
        }
        if (not kCanUseGetLengthAsOptimizationCheck_) {
            return rhsSize == this->size ();
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
    inline bool Set<T>::EqualsComparer::operator() (const Set& lhs, const Iterable<value_type>& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{&lhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<typename Iterable<T>::_IRep>{&rhs}._ConstGetRep ());
    }
    template <typename T>
    inline bool Set<T>::EqualsComparer::operator() (const Iterable<value_type>& lhs, const Set& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<typename Iterable<T>::_IRep>{&lhs}._ConstGetRep ());
    }

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
