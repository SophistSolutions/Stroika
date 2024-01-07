/*
* Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
*/

/*
********************************************************************************
***************************** Implementation Details ***************************
********************************************************************************
*/
#ifndef _Stroika_Foundation_Containers_Bijection_inl_
#define _Stroika_Foundation_Containers_Bijection_inl_

#include "../Debug/Assertions.h"

#include "Factory/Bijection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** Bijection_Base::InjectivityViolation **********************
     ********************************************************************************
     */
    inline Bijection_Base::InjectivityViolation::InjectivityViolation ()
        : inherited{"Injectivity violation"sv}
    {
    }

    /*
     ********************************************************************************
     ******************** Bijection<DOMAIN_TYPE, RANGE_TYPE> ************************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection ()
        requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>)
        : Bijection{equal_to<DOMAIN_TYPE>{}, equal_to<RANGE_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : Bijection{DataExchange::ValidationStrategy::eAssertion, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer),
                    forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DataExchange::ValidationStrategy injectivityCheckPolicy,
                                                          DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : inherited{Factory::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, remove_cvref_t<DOMAIN_EQUALS_COMPARER>, remove_cvref_t<RANGE_EQUALS_COMPARER>>::Default () (
              injectivityCheckPolicy, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const initializer_list<value_type>& src)
        requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>)
        : Bijection{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                                                          RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const initializer_list<value_type>& src)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (ITERABLE_OF_ADDABLE&& src)
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Bijection<DOMAIN_TYPE, RANGE_TYPE>>)
        : Bijection{}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer,
                                                          RANGE_EQUALS_COMPARER&& rangeEqualsComparer, ITERABLE_OF_ADDABLE&& src)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IEqualsComparer<equal_to<DOMAIN_TYPE>, DOMAIN_TYPE> and IEqualsComparer<equal_to<RANGE_TYPE>, RANGE_TYPE>)
        : Bijection{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IEqualsComparer<DOMAIN_TYPE> DOMAIN_EQUALS_COMPARER, IEqualsComparer<RANGE_TYPE> RANGE_EQUALS_COMPARER, IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERATOR_OF_ADDABLE>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer,
                                                   ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const shared_ptr<_IRep>& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (shared_ptr<_IRep>&& src) noexcept
        : inherited{(RequireExpression (src != nullptr), move (src))}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::GetDomainEqualsComparer () const -> DomainEqualsCompareFunctionType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetDomainEqualsComparer ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::GetRangeEqualsComparer () const -> RangeEqualsCompareFunctionType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetRangeEqualsComparer ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Iterable<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::Preimage () const
    {
        // need inherited temporarily til we remove deprecated Map function
        //return this->template Map<DOMAIN_TYPE> ([] (const pair<DOMAIN_TYPE, RANGE_TYPE>& elt) { return elt.first; });
        return this->inherited::template Map<Iterable<DOMAIN_TYPE>> ([] (const pair<DOMAIN_TYPE, RANGE_TYPE>& elt) { return elt.first; });
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::Image () const
    {
        // need inherited temporarily til we remove deprecated Map function
        //return this->Map<RANGE_TYPE> ([] (const pair<DOMAIN_TYPE, RANGE_TYPE>& elt) { return elt.second; });
        return this->inherited::template Map<Iterable<RANGE_TYPE>> ([] (const pair<DOMAIN_TYPE, RANGE_TYPE>& elt) { return elt.second; });
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::Lookup (ArgByValueType<DomainType> key, RangeType* item) const
    {
        if (item == nullptr) {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
        }
        else {
            optional<RangeType> tmp;
            if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                *item = *tmp;
                return true;
            }
            return false;
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::Lookup (ArgByValueType<DomainType> key, optional<RangeType>* item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, item);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline optional<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::Lookup (ArgByValueType<DomainType> key) const
    {
        optional<RANGE_TYPE>  r;
        [[maybe_unused]] bool result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &r);
        Ensure (result == r.has_value ());
        return r;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::Lookup (ArgByValueType<DomainType> key, nullptr_t) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline RANGE_TYPE Bijection<DOMAIN_TYPE, RANGE_TYPE>::LookupValue (ArgByValueType<DomainType> key, ArgByValueType<RangeType> defaultValue) const
    {
        optional<RANGE_TYPE> r = Lookup (key);
        return r.has_value () ? *r : defaultValue;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseLookup (ArgByValueType<RangeType> key, DomainType* item) const
    {
        if (item == nullptr) {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, nullptr);
        }
        else {
            optional<DomainType> tmp;
            if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, &tmp)) {
                *item = *tmp;
                return true;
            }
            return false;
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseLookup (ArgByValueType<RangeType> key, optional<DomainType>* item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, item);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline optional<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseLookup (ArgByValueType<RangeType> key) const
    {
        optional<DOMAIN_TYPE> r;
        [[maybe_unused]] bool result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, &r);
        Ensure (result == r.has_value ());
        return r;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseLookup (ArgByValueType<RangeType> key, nullptr_t) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().InverseLookup (key, nullptr);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline DOMAIN_TYPE Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseLookupValue (ArgByValueType<RangeType> key, ArgByValueType<DomainType> defaultValue) const
    {
        optional<DOMAIN_TYPE> r = InverseLookup (key);
        return r.has_value () ? *r : defaultValue;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::MapToRange (const Iterable<DomainType>& values) const -> Iterable<RangeType>
    {
        vector<RANGE_TYPE> tmp;
        tmp.reserve (values.size ());
        for (const auto& i : values) {
            tmp.push_back (*Lookup (i));
        }
        return Iterable<RANGE_TYPE>{tmp};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::MapToDomain (const Iterable<RangeType>& values) const -> Iterable<DomainType>
    {
        vector<DOMAIN_TYPE> tmp;
        tmp.reserve (values.size ());
        for (const auto& i : values) {
            tmp.push_back (*InverseLookup (i));
        }
        return Iterable<DOMAIN_TYPE>{move (tmp)};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename RESULT_CONTAINER, invocable<pair<DOMAIN_TYPE, RANGE_TYPE>> ELEMENT_MAPPER>
    inline RESULT_CONTAINER Bijection<DOMAIN_TYPE, RANGE_TYPE>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, pair<DOMAIN_TYPE, RANGE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, pair<DOMAIN_TYPE, RANGE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, Bijection>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<ELEMENT_MAPPER> (elementMapper), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> implementation then...
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <derived_from<Iterable<pair<DOMAIN_TYPE, RANGE_TYPE>>> RESULT_CONTAINER, predicate<pair<DOMAIN_TYPE, RANGE_TYPE>> INCLUDE_PREDICATE>
    inline auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::Where (INCLUDE_PREDICATE&& includeIfTrue) const -> RESULT_CONTAINER
    {
        if constexpr (same_as<RESULT_CONTAINER, Bijection>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Where<RESULT_CONTAINER> (
                forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default Iterable<> implementation then...
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::WhereDomainIntersects (const Iterable<DomainType>& domainValues) const -> Bijection
    {
        // @todo - fix very primitive implementation - could use Generator instead?, generator avoids copy of data, and just copies includeIfTrue filter function
        _SafeReadRepAccessor<_IRep> lhs{this};
        Bijection                   result = lhs._ConstGetRep ().CloneEmpty ();
        for (const auto& i : *this) {
            if (domainValues.Contains (i.first, this->GetDomainEqualsComparer ())) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::WhereRangeIntersects (const Iterable<RangeType>& rangeValues) const -> Bijection
    {
        // @todo - fix very primitive implementation - could use Generator instead?, generator avoids copy of data, and just copies includeIfTrue filter function
        _SafeReadRepAccessor<_IRep> lhs{this};
        Bijection                   result = lhs._ConstGetRep ().CloneEmpty ();
        for (const auto& i : *this) {
            if (rangeValues.Contains (i.second, this->GetRangeEqualsComparer ())) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::ContainsDomainElement (ArgByValueType<DomainType> key) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::ContainsRangeElement (ArgByValueType<RangeType> v) const
    {
        // REIMPLEMENT USING InverseLookup()!!! @todo
        for (value_type t : *this) {
            if (GetRangeEqualsComparer () (t.second, v)) {
                return true;
            }
        }
        return false;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Add (ArgByValueType<DomainType> key, ArgByValueType<RangeType> newElt)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename ADDABLE_T>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Add (ADDABLE_T&& p)
        requires (convertible_to<ADDABLE_T, pair<DOMAIN_TYPE, RANGE_TYPE>> or convertible_to<ADDABLE_T, KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>>)
    {
        if constexpr (is_convertible_v<remove_cvref_t<ADDABLE_T>, value_type>) {
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.first, p.second);
        }
        else {
            static_assert (is_convertible_v<ADDABLE_T, Common::KeyValuePair<DomainType, RangeType>>);
            _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IInputIterator<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> COPY_FROM_ITERATOR_KEYVALUE>
    void Bijection<DOMAIN_TYPE, RANGE_TYPE>::AddAll (COPY_FROM_ITERATOR_KEYVALUE&& start, COPY_FROM_ITERATOR_KEYVALUE&& end)
    {
        for (auto i = forward<COPY_FROM_ITERATOR_KEYVALUE> (start); i != forward<COPY_FROM_ITERATOR_KEYVALUE> (end); ++i) {
            Add (*i);
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> CONTAINER_OF_KEYVALUE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::AddAll (const CONTAINER_OF_KEYVALUE& items)
    {
        // see https://stroika.atlassian.net/browse/STK-645
        /*
         *  Note - unlike some other containers - we don't need to check for this != &s because if we
         *  attempt to add items that already exist, it would do nothing to our iteration
         *  and therefore not lead to an infinite loop.
         */
        AddAll (std::begin (items), std::end (items));
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveDomainElement (ArgByValueType<DomainType> d)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveDomainElement (d);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveRangeElement (ArgByValueType<RangeType> r)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveRangeElement (r);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <predicate<pair<DOMAIN_TYPE, RANGE_TYPE>> PREDICATE>
    size_t Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveAll (PREDICATE&& p)
    {
        size_t nRemoved{};
        for (Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>> i = this->begin (); i != this->end ();) {
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
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename TARGET_CONTAINER>
    TARGET_CONTAINER Bijection<DOMAIN_TYPE, RANGE_TYPE>::Inverse () const
    {
        TARGET_CONTAINER r;
        for (value_type i : *this) {
            r.Add (i.second, i.first);
        }
        return r;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_PAIR_RANGE_DOMAIN>
    inline CONTAINER_PAIR_RANGE_DOMAIN Bijection<DOMAIN_TYPE, RANGE_TYPE>::As () const
    {
        return CONTAINER_PAIR_RANGE_DOMAIN{this->begin (), this->end ()};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::clear ()
    {
        RemoveAll ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> nextI{nullptr};
        this->Remove (i, &nextI);
        return nextI;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>& Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator+= (const ITERABLE_OF_ADDABLE& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <IIterableOf<KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>& Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator-= (const ITERABLE_OF_ADDABLE& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i)
        -> tuple<_IRep*, Iterator<value_type>>
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
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            _SafeReadRepAccessor<_IRep>{this};
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator== (const Bijection& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }

    /*
     ********************************************************************************
     ************** Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep ***********************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        // If sizes differ, the Bijections differ
        if (this->size () != rhs.size ()) {
            return false;
        }
        // Since both sides are the same size, we can iterate over one, and make sure the key/values in the first
        // are present, and with the same Bijection in the second.
        for (auto i = this->MakeIterator (); not i.Done (); ++i) {
            optional<RangeType> tmp;
            if (not rhs.Lookup (i->first, &tmp) or not GetRangeEqualsComparer () (*tmp, i->second)) {
                return false;
            }
        }
        return true;
    }

}

#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
