/*
* Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
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
        : inherited (L"Injectivity violation"sv)
    {
    }

    /*
     ********************************************************************************
     ******************** Bijection<DOMAIN_TYPE, RANGE_TYPE> ************************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection ()
        : Bijection (equal_to<DOMAIN_TYPE>{}, equal_to<RANGE_TYPE>{})
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : Bijection (InjectivityViolationPolicy::eDEFAULT, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer))
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (InjectivityViolationPolicy injectivityCheckPolicy, DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer)
        : inherited (Factory::Bijection_Factory<DOMAIN_TYPE, RANGE_TYPE, DOMAIN_EQUALS_COMPARER, RANGE_EQUALS_COMPARER> (injectivityCheckPolicy, forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer)) ())
    {
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src)
        : Bijection ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> ()>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const initializer_list<pair<DOMAIN_TYPE, RANGE_TYPE>>& src)
        : Bijection (forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> and not is_base_of_v<Bijection<DOMAIN_TYPE, RANGE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_SINGLEVALUE_ADD_ARGS>>>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src)
        : Bijection ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename CONTAINER_OF_SINGLEVALUE_ADD_ARGS, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_SINGLEVALUE_ADD_ARGS, Common::KeyValuePair<DOMAIN_TYPE, RANGE_TYPE>> and not is_base_of_v<Bijection<DOMAIN_TYPE, RANGE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_SINGLEVALUE_ADD_ARGS>>>*>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, const CONTAINER_OF_SINGLEVALUE_ADD_ARGS& src)
        : Bijection (forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>*>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end)
        : Bijection ()
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename DOMAIN_EQUALS_COMPARER, typename RANGE_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG, enable_if_t<Common::IsPotentiallyComparerRelation<DOMAIN_TYPE, DOMAIN_EQUALS_COMPARER> () and Common::IsPotentiallyComparerRelation<RANGE_TYPE, RANGE_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG>>*>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (DOMAIN_EQUALS_COMPARER&& domainEqualsComparer, RANGE_EQUALS_COMPARER&& rangeEqualsComparer, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG start, COPY_FROM_ITERATOR_SINGLEVALUE_ADD_ARG end)
        : Bijection (forward<DOMAIN_EQUALS_COMPARER> (domainEqualsComparer), forward<RANGE_EQUALS_COMPARER> (rangeEqualsComparer))
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (const _BijectionRepSharedPtr& src) noexcept
        : inherited (src)
    {
        RequireNotNull (src);
        _AssertRepValidType ();
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline Bijection<DOMAIN_TYPE, RANGE_TYPE>::Bijection (_BijectionRepSharedPtr&& src) noexcept
        : inherited ((RequireNotNull (src), move (src)))
    {
        _AssertRepValidType ();
    }
#if qDebug
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    Bijection<DOMAIN_TYPE, RANGE_TYPE>::~Bijection ()
    {
        if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
            // SharingState can be NULL because of MOVE semantics
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
        }
    }
#endif
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
        return Iterable<RANGE_TYPE> (tmp);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::InverseMap (const Iterable<RangeType>& values) const -> Iterable<DomainType>
    {
        vector<DOMAIN_TYPE> tmp;
        tmp.reserve (values.size ());
        for (auto i : values) {
            tmp.push_back (*InverseLookup (i));
        }
        return Iterable<DOMAIN_TYPE> (move (tmp));
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    auto Bijection<DOMAIN_TYPE, RANGE_TYPE>::Where (const function<bool (pair<DomainType, RangeType>)>& includeIfTrue) const -> Bijection
    {
        // @todo - fix very primitive implementation
        _SafeReadRepAccessor<_IRep> lhs{this};
        //Bijection                   result = dynamic_pointer_cast<_BijectionRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty (this));
        Bijection result;
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
        // @todo - fix very primitive implementation
        _SafeReadRepAccessor<_IRep> lhs{this};
        //Bijection                   result = dynamic_pointer_cast<_BijectionRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty (this));
        Bijection result;
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
        // @todo - fix very primitive implementation
        _SafeReadRepAccessor<_IRep> lhs{this};
        //Bijection                   result = dynamic_pointer_cast<_BijectionRepSharedPtr> (lhs._ConstGetRep ().CloneEmpty (this));
        Bijection result;
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
        for (pair<DOMAIN_TYPE, RANGE_TYPE> t : *this) {
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
        for (auto i = start; i != end; ++i) {
            Add (*i);
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>*>
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
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::Remove (const Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>& i)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
        }
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    template <typename TARGET_CONTAINER>
    TARGET_CONTAINER Bijection<DOMAIN_TYPE, RANGE_TYPE>::Inverse () const
    {
        TARGET_CONTAINER r;
        for (pair<DOMAIN_TYPE, RANGE_TYPE> i : *this) {
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
    inline void Bijection<DOMAIN_TYPE, RANGE_TYPE>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

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
                using inherited             = typename Traversal::IterableFromIterator<DOMAIN_TYPE>::_Rep;
                using _IterableRepSharedPtr = typename Iterable<DOMAIN_TYPE>::_IterableRepSharedPtr;
                Bijection fBijection_;
                MyIterableRep_ (const Bijection& b)
                    : inherited ()
                    , fBijection_ (b)
                {
                }
                virtual Iterator<DOMAIN_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>> (fBijection_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<DOMAIN_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = (*myContext)->first;
                            (*myContext)++;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<DOMAIN_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - ignore forIterableEnvelope
                    return Iterable<DOMAIN_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyBijection_& b)
                : Iterable<DOMAIN_TYPE> (Iterable<DOMAIN_TYPE>::template MakeSmartPtr<MyIterableRep_> (b))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep> (const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_ (Bijection<DOMAIN_TYPE, RANGE_TYPE> (rep));
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    Iterable<RANGE_TYPE> Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep::_Image_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<RANGE_TYPE> {
            using MyBijection_ = Bijection<DOMAIN_TYPE, RANGE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<RANGE_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using inherited             = typename Traversal::IterableFromIterator<RANGE_TYPE>::_Rep;
                using _IterableRepSharedPtr = typename Iterable<RANGE_TYPE>::_IterableRepSharedPtr;
                MyBijection_ fBijection_;
                MyIterableRep_ (const MyBijection_& b)
                    : inherited ()
                    , fBijection_ (b)
                {
                }
                virtual Iterator<RANGE_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<pair<DOMAIN_TYPE, RANGE_TYPE>>> (fBijection_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<RANGE_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = (*myContext)->second;
                            (*myContext)++;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<RANGE_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - ignore forIterableEnvelope
                    return Iterable<RANGE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyBijection_& b)
                : Iterable<RANGE_TYPE> (Iterable<RANGE_TYPE>::template MakeSmartPtr<MyIterableRep_> (b))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep> (const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_ (Bijection<DOMAIN_TYPE, RANGE_TYPE> (rep));
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
        for (auto i = this->MakeIterator (this); not i.Done (); ++i) {
            optional<RangeType> tmp;
            if (not rhs.Lookup (i->first, &tmp) or not GetRangeEqualsComparer () (*tmp, i->second)) {
                return false;
            }
        }
        return true;
    }

    /*
     ********************************************************************************
     *********** Bijection<DOMAIN_TYPE, RANGE_TYPE>::EqualsComparer *****************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool Bijection<DOMAIN_TYPE, RANGE_TYPE>::EqualsComparer::operator() (const Bijection& lhs, const Bijection& rhs) const
    {
        return _SafeReadRepAccessor<_IRep>{&lhs}._ConstGetRep ().Equals (_SafeReadRepAccessor<_IRep>{&rhs}._ConstGetRep ());
    }

    /*
     ********************************************************************************
     ******************** Bijection comparison operators ****************************
     ********************************************************************************
     */
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool operator== (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs)
    {
        return typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::EqualsComparer{}(lhs, rhs);
    }
    template <typename DOMAIN_TYPE, typename RANGE_TYPE>
    inline bool operator!= (const Bijection<DOMAIN_TYPE, RANGE_TYPE>& lhs, const Bijection<DOMAIN_TYPE, RANGE_TYPE>& rhs)
    {
        return not typename Bijection<DOMAIN_TYPE, RANGE_TYPE>::EqualsComparer{}(lhs, rhs);
    }

}

#endif /* _Stroika_Foundation_Containers_Bijection_inl_ */
