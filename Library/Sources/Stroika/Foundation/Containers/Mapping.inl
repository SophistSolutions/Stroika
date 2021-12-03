/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Mapping_inl_
#define _Stroika_Foundation_Containers_Mapping_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set>

#include "../Debug/Assertions.h"
#include "../Debug/Cast.h"
#include "../Traversal/Generator.h"
#include "Factory/Mapping_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> **********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping ()
        : inherited{Factory::Mapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE>{}()}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_EQUALS_COMPARER, KEY_TYPE> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer)
        : inherited{Factory::Mapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_EQUALS_COMPARER>{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}()}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_EQUALS_COMPARER, KEY_TYPE> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_EQUALS_COMPARER, KEY_TYPE> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> and not is_base_of_v<Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>>>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (CONTAINER_OF_ADDABLE&& src)
        : Mapping{}
    {
        AddAll (forward<CONTAINER_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const CONTAINER_OF_ADDABLE& src)
        : Mapping{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Mapping{}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Mapping{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const _IRepSharedPtr& rep) noexcept
        : inherited{rep}
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (_IRepSharedPtr&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::GetKeyEqualsComparer () const -> KeyEqualsCompareFunctionType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyEqualsComparer ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Iterable<KEY_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Keys () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Keys ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Iterable<MAPPED_VALUE_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::MappedValues () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().MappedValues ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Lookup (ArgByValueType<key_type> key, mapped_type* item) const
    {
        if (item == nullptr) {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
        }
        else {
            optional<mapped_type> tmp;
            if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                *item = *tmp;
                return true;
            }
            return false;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Lookup (ArgByValueType<key_type> key, optional<mapped_type>* item) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, item);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline optional<MAPPED_VALUE_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Lookup (ArgByValueType<key_type> key) const
    {
        optional<MAPPED_VALUE_TYPE> r;
        [[maybe_unused]] bool       result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &r);
        Ensure (result == r.has_value ());
        return r;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Lookup (ArgByValueType<key_type> key, nullptr_t) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename THROW_IF_MISSING>
    inline MAPPED_VALUE_TYPE Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::LookupChecked (ArgByValueType<key_type> key, const THROW_IF_MISSING& throwIfMissing) const
    {
        if (optional<MAPPED_VALUE_TYPE> r{Lookup (key)}) [[LIKELY_ATTR]] {
            return *r;
        }
        Execution::Throw (throwIfMissing);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline MAPPED_VALUE_TYPE Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::LookupValue (ArgByValueType<key_type> key, ArgByValueType<mapped_type> defaultValue) const
    {
        optional<MAPPED_VALUE_TYPE> r{Lookup (key)};
        return r.has_value () ? *r : defaultValue;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator[] (ArgByValueType<key_type> key) const -> add_const_t<mapped_type>
    {
        return *Lookup (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::ContainsKey (ArgByValueType<key_type> key) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::ContainsMappedValue (ArgByValueType<mapped_type> v, const VALUE_EQUALS_COMPARER& valueEqualsComparer) const
    {
        return this->Find ([&valueEqualsComparer, &v] (const auto& t) { return valueEqualsComparer (t.fValue, v); }) != nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt, AddReplaceMode addReplaceMode)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt, addReplaceMode);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<value_type> p, AddReplaceMode addReplaceMode)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue, addReplaceMode);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    unsigned int Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end, AddReplaceMode addReplaceMode)
    {
        unsigned int cntAdded{};
        for (auto i = start; i != end; ++i) {
            if (Add (*i, addReplaceMode)) {
                ++cntAdded;
            }
        }
        return cntAdded;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>*>
    inline unsigned int Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (CONTAINER_OF_KEYVALUE&& items, AddReplaceMode addReplaceMode)
    {
        if constexpr (std::is_convertible_v<decay_t<CONTAINER_OF_KEYVALUE>*, Iterable<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[UNLIKELY_ATTR]] {
                vector<value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                return AddAll (std::begin (copy), std::end (copy), addReplaceMode);
            }
        }
        return AddAll (std::begin (items), std::end (items), addReplaceMode);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (ArgByValueType<key_type> key)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key)); // use RemoveIf () if key may not exist
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveIf (ArgByValueType<key_type> key)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    size_t Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (const CONTAINER_OF_ADDABLE& items)
    {
        if (this == &items) { // avoid modifying container while iterating over it
            size_t result = this->size ();
            RemoveAll ();
            return result;
        }
        else {
            size_t cnt{};
            for (const auto& i : items) {
                ++cnt;
                Remove (i.first);
            }
            return cnt;
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    inline size_t Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        size_t cnt{};
        for (auto i = start; i != end; ++i) {
            ++cnt;
            Remove (i->first);
        }
        return cnt;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>, PREDICATE> ()>*>
    size_t Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (const PREDICATE& p)
    {
        size_t nRemoved{};
        for (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> i = this->begin (); i != this->end ();) {
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
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEY_TYPE>
    void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RetainAll (const CONTAINER_OF_KEY_TYPE& items)
    {
        // @see https://stroika.atlassian.net/browse/STK-539
#if 0
                Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>   result = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> { _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().CloneEmpty () };
                for (auto key2Keep : items) {
                    if (auto l = this->Lookup (key2Keep)) {
                        result.Add (key2Keep, *l);
                    }
                }
                *this = result;
#else
        // cannot easily use STL::less because our Mapping class only requires KeyEqualsCompareFunctionType - SO - should use Stroika Set<> But don't want cross-dependencies if not needed
        set<KEY_TYPE> tmp (items.begin (), items.end ()); // @todo - weak implementation because of 'comparison' function, and performance (if items already a set)
        for (Iterator<value_type> i = this->begin (); i != this->end ();) {
            if (tmp.find (i->fKey) == tmp.end ()) {
                [[maybe_unused]] size_t sz = this->size ();
                i                          = this->erase (i);
                Assert (this->size () == sz - 1u);
            }
            else {
                ++i;
            }
        }
#endif
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (const function<bool (ArgByValueType<key_type>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where ([=] (const ArgByValueType<KeyValuePair<key_type, mapped_type>>& kvp) { return includeIfTrue (kvp.fKey); }, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEYS>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const CONTAINER_OF_KEYS& includeKeys) const -> ArchetypeContainerType
    {
        return Where ([=] (const key_type& key) -> bool { return includeKeys.Contains (key); });
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const initializer_list<key_type>& includeKeys) const -> ArchetypeContainerType
    {
        Iterable<key_type> ik{includeKeys};
        return inherited::Where ([=] (const ArgByValueType<value_type>& kvp) { return ik.Contains (kvp.fKey); }, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_Key_T>
    inline CONTAINER_OF_Key_T Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::As () const
    {
        return As_<CONTAINER_OF_Key_T> ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_Key_T>
    CONTAINER_OF_Key_T Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::As_ ([[maybe_unused]] enable_if_t<is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesInsertPair) const
    {
        CONTAINER_OF_Key_T result;
        for (const auto& i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), pair<KEY_TYPE, MAPPED_VALUE_TYPE>{i.fKey, i.fValue});
        }
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_Key_T>
    CONTAINER_OF_Key_T Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::As_ (enable_if_t<!is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int>) const
    {
        CONTAINER_OF_Key_T result;
        for (const auto& i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), value_type{i.fKey, i.fValue});
        }
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Accumulate (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue, const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f, mapped_type initialValue)
    {
        Add (key, f (LookupValue (key, initialValue), newValue));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::erase (ArgByValueType<key_type> key)
    {
        Remove (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> nextI{nullptr};
        Remove (i, &nextI);
        return nextI;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::clear ()
    {
        RemoveAll ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator+ (const CONTAINER_OF_ADDABLE& items) const
    {
        Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> result = *this;
        result.AddAll (items);
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator+= (const CONTAINER_OF_ADDABLE& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator-= (const CONTAINER_OF_ADDABLE& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<_IRep*, Iterator<value_type>>
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
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
#if qDebug
        [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
#endif
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator== (const Mapping& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }
#endif

    /*
     ********************************************************************************
     ****************** Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep *****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    Iterable<KEY_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep::_Keys_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<KEY_TYPE> {
            using MyMapping_ = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : fMapping_{map}
                {
                }
                virtual Iterator<KEY_TYPE> MakeIterator () const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<KEY_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = (*myContext)->fKey;
                            ++(*myContext);
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<KEY_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyMapping_& m)
                : Iterable<KEY_TYPE>{Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (m)}
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> (const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> (rep)};
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    Iterable<MAPPED_VALUE_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep::_Values_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<MAPPED_VALUE_TYPE> {
            using MyMapping_ = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<MAPPED_VALUE_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<MAPPED_VALUE_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : fMapping_{map}
                {
                }
                virtual Iterator<MAPPED_VALUE_TYPE> MakeIterator () const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<MAPPED_VALUE_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = (*myContext)->fValue;
                            ++(*myContext);
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<MAPPED_VALUE_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<MAPPED_VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            // @todo debug if/why issue with using uninform initializaiton here - fails to compile on vs2k17 and gcc ASAN giving erorrs that maybe related???
            MyIterable_ (const MyMapping_& m)
                : Iterable<MAPPED_VALUE_TYPE>{Iterable<MAPPED_VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (m)}
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> (const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>{rep}};
    }

    /*
     ********************************************************************************
     ********** Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer ****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    constexpr Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer<VALUE_EQUALS_COMPARER>::EqualsComparer (const VALUE_EQUALS_COMPARER& valueEqualsComparer)
        : fValueEqualsComparer{valueEqualsComparer}
    {
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer<VALUE_EQUALS_COMPARER>::operator() (const Mapping& lhs, const Mapping& rhs) const
    {
        /*
         *    @todo   THIS CODE IS TOO COMPLICATED, and COULD USE CLEANUP/CODE REVIEW - LGP 2014-06-11
         */
        _SafeReadRepAccessor<_IRep> lhsR{&lhs};
        _SafeReadRepAccessor<_IRep> rhsR{&rhs};
        if (&lhsR._ConstGetRep () == &rhsR._ConstGetRep ()) {
            // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
            return true;
        }
        // Check length, so we don't need to check both iterators for end/done
        if (lhsR._ConstGetRep ().GetLength () != rhsR._ConstGetRep ().GetLength ()) {
            return false;
        }
        /*
         *  Two Mappings compare equal, if they have the same domain, and map each element of that domain to the same range.
         *  They need not be in the same order to compare equals. Still - they often are, and if they are, this algorithm is faster.
         *  If they miss, we need to fall back to a slower strategy.
         */
        auto li                = lhsR._ConstGetRep ().MakeIterator ();
        auto ri                = rhs.MakeIterator ();
        auto keyEqualsComparer = lhs.GetKeyEqualsComparer (); // arbitrarily select left side key equals comparer
        while (not li.Done ()) {
            Assert (not ri.Done ()); // cuz move at same time and same size
            bool keysEqual = keyEqualsComparer (li->fKey, ri->fKey);
            Require (keysEqual == rhs.GetKeyEqualsComparer () (li->fKey, ri->fKey)); // if fails, cuz rhs/lhs keys equals comparers disagree
            if (keysEqual) {
                // then we are doing in same order so can do quick impl
                if (not fValueEqualsComparer (li->fValue, ri->fValue)) {
                    return false;
                }
            }
            else {
                // check if li maps to right value in rhs
                auto o = rhs.Lookup (li->fKey);
                if (not o.has_value () or not fValueEqualsComparer (*o, li->fValue)) {
                    return false;
                }
                // if the keys were different, we must check the reverse direction too
                if (not lhsR._ConstGetRep ().Lookup (ri->fKey, &o) or not fValueEqualsComparer (*o, ri->fValue)) {
                    return false;
                }
            }
            // if we got this far, all compared OK so far, so keep going
            ++li;
            ++ri;
        }
        Assert (ri.Done ()); // cuz LHS done and both sides iterate at same pace, and we checked both same size
        return true;
    }

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ******************** Mapping comparison operators ******************************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool operator== (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs)
    {
        return typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::template EqualsComparer<>{}(lhs, rhs);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool operator!= (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
