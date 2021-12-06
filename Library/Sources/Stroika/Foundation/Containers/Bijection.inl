/*
* Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
        : inherited{L"Injectivity violation"sv}
    {
    }

    /*
     ********************************************************************************
     ******************** Bijection<DOMAIN_TYPE, RANGE_TYPE> ************************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection ()
        : Bijection{equal_to<DOMAIN_TYPE>{}, equal_to<RANGE_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsEqualsComparer<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : Bijection{InjectivityViolationPolicy::eDEFAULT, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsEqualsComparer<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (InjectivityViolationPolicy injectivityCheckPolicy, DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : inherited{Factory::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER> (injectivityCheckPolicy, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)) ()}
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const initializer_list<value_type>& src)
        : Bijection{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsEqualsComparer<RANGE_EQUALS_COMPARER, RANGE_TYPE> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const initializer_list<value_type>& src)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> and not is_base_of_v<Bijection<DOMAIN_TYPE, RANGE_TYPE>, decay_t<CONTAINER_OF_SINGLEVALUE_ADD_ARGS>>>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src)
        : Bijection{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsEqualsComparer<RANGE_EQUALS_COMPARER, RANGE_TYPE> () and Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>>>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>*>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end)
        : Bijection{}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Common::IsEqualsComparer<DOMAIN_EQUALS_COMPARER, DOMAIN_TYPE> () and Common::IsEqualsComparer<RANGE_EQUALS_COMPARER, RANGE_TYPE> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>*>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end)
        : Bijection{forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const _IRepSharedPtr& src) noexcept
        : inherited{src}
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (_IRepSharedPtr&& src) noexcept
        : inherited{(RequireNotNull (src), move (src))}
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
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Preimage ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::Image () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Image ();
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
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::Map (const Iterable<DomainType>& values) const -> Iterable<RangeType>
    {
        vector<RANGE_TYPE> tmp;
        tmp.reserve (values.size ());
        for (auto i : values) {
            tmp.push_back (*Lookup (i));
        }
        return Iterable<RANGE_TYPE>{tmp};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseMap (const Iterable<RangeType>& values) const -> Iterable<DomainType>
    {
        vector<DOMAIN_TYPE> tmp;
        tmp.reserve (values.size ());
        for (auto i : values) {
            tmp.push_back (*InverseLookup (i));
        }
        return Iterable<DOMAIN_TYPE>{move (tmp)};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::Where (const function<bool (pair<DomainType, RangeType>)>& includeIfTrue) const -> Bijection
    {
        // @todo - fix very primitive implementation - could use Generator instead?, generator avoids copy of data, and just copies includeIfTrue filter function
        _SafeReadRepAccessor<_IRep> lhs{this};
        Bijection                   result = dynamic_pointer_cast<_IRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty ());
        for (auto&& i : *this) {
            if (includeIfTrue (i)) {
                result.Add (i);
            }
        }
        return result;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::WhereDomainIntersects (const Iterable<DomainType>& domainValues) const -> Bijection
    {
        // @todo - fix very primitive implementation - could use Generator instead?, generator avoids copy of data, and just copies includeIfTrue filter function
        _SafeReadRepAccessor<_IRep> lhs{this};
        Bijection                   result = dynamic_pointer_cast<_IRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty ());
        for (auto&& i : *this) {
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
        Bijection                   result = dynamic_pointer_cast<_IRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty ());
        for (auto&& i : *this) {
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
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Add (const pair<DomainType, RangeType>& p)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.first, p.second);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename KEYVALUEPAIR, enable_if_t<not is_convertible_v<KEYVALUEPAIR, pair<DOMAIN_TYPE, RANGE_TYPE>>>*>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Add (KEYVALUEPAIR p)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_KEYVALUE>
    void Bijection<DOMAIN_TYPE, RANGE_TYPE>::AddAll (COPY_FROM_ITERATOR_KEYVALUE start, COPY_FROM_ITERATOR_KEYVALUE end)
    {
        // TODO GET THIS WORKING - static_assert (IsAddable_v<ExtractValueType_t<COPY_FROM_ITERATOR_KEYVALUE>>);
        for (auto i = start; i != end; ++i) {
            Add (*i);
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>*>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::AddAll (const CONTAINER_OF_KEYVALUE& items)
    {
        // todo get this working - static_assert (IsAddable_v<ExtractValueType_t<CONTAINER_OF_KEYVALUE>>);
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
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<pair<DOMAIN_TYPE, RANGE_TYPE>, PREDICATE> ()>*>
    size_t Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveAll (const PREDICATE& p)
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
        return CONTAINER_PAIR_RANGE_DOMAIN (this->begin (), this->end ());
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
    template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>& Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator+= (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>& Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator-= (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
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
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::operator== (const Bijection& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }
#endif

    /*
     ********************************************************************************
     ************** Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep ***********************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    Iterable<DOMAIN_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep::_PreImage_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<DOMAIN_TYPE> {
            using MyBijection_ = Bijection<DOMAIN_TYPE, RANGE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<DOMAIN_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<DOMAIN_TYPE>::_IterableRepSharedPtr;
                Bijection fBijection_;
                MyIterableRep_ (const Bijection& b)
                    : fBijection_{b}
                {
                }
                virtual Iterator<DOMAIN_TYPE> MakeIterator () const override
                {
                    // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same container)
                    auto sharedContext = make_shared<MyBijection_> (fBijection_);
                    // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
                    // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
                    auto getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator ()] () mutable -> optional<DOMAIN_TYPE> {
                        if (perIteratorContextBaseIterator.Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = perIteratorContextBaseIterator->first;
                            ++perIteratorContextBaseIterator;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<DOMAIN_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<DOMAIN_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyBijection_& b)
                : Iterable<DOMAIN_TYPE>{Iterable<DOMAIN_TYPE>::template MakeSmartPtr<MyIterableRep_> (b)}
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<_IRep> (const_cast<_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{Bijection<DOMAIN_TYPE, RANGE_TYPE>{rep}};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep::_Image_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<RANGE_TYPE> {
            using MyBijection_ = Bijection<DOMAIN_TYPE, RANGE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<RANGE_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<RANGE_TYPE>::_IterableRepSharedPtr;
                MyBijection_ fBijection_;
                MyIterableRep_ (const MyBijection_& b)
                    : fBijection_{b}
                {
                }
                virtual Iterator<RANGE_TYPE> MakeIterator () const override
                {
                    // If we have many iterator copies, we need ONE copy of this sharedContext (they all share a reference to the same container)
                    auto sharedContext = make_shared<MyBijection_> (fBijection_);
                    // If we have many iterator copies, each needs to copy their 'base iterator' (this is their 'index' into the container)
                    // Both the 'sharedContext' and the perIteratorContextBaseIterator' get stored into the lambda closure so they get appropriately copied as you copy iterators
                    auto getNext = [sharedContext, perIteratorContextBaseIterator = sharedContext->MakeIterator ()] () mutable -> optional<RANGE_TYPE> {
                        if (perIteratorContextBaseIterator.Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = perIteratorContextBaseIterator->second;
                            ++perIteratorContextBaseIterator;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<RANGE_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<RANGE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyBijection_& b)
                : Iterable<RANGE_TYPE>{Iterable<RANGE_TYPE>::template MakeSmartPtr<MyIterableRep_> (b)}
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<_IRep> (const_cast<_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{Bijection<DOMAIN_TYPE, RANGE_TYPE>{rep}};
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep::_Equals_Reference_Implementation (const _IRep& rhs) const
    {
        if (this == &rhs) {
            return true;
        }
        // If sizes differ, the Bijections differ
        if (this->GetLength () != rhs.GetLength ()) {
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

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ******************** Bijection comparison operators ****************************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs)
    {
        using B = Bijection<DOMAIN_TYPE, RANGE_TYPE>;
        return typename B::template _SafeReadRepAccessor<typename B::_IRep>{&lhs}._ConstGetRep ().Equals (typename B::template _SafeReadRepAccessor<typename B::_IRep>{&rhs}._ConstGetRep ());
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
