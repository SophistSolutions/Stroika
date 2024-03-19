/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Association_inl_
#define _Stroika_Foundation_Containers_Association_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <set> //tmphack for sloppy RetainAll impl

#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Traversal/Generator.h"

#include "Factory/Association_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     **************** Association<KEY_TYPE, MAPPED_VALUE_TYPE> **********************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association ()
        requires (IEqualsComparer<equal_to<KEY_TYPE>, KEY_TYPE>)
        : Association{equal_to<KEY_TYPE>{}}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (KEY_EQUALS_COMPARER&& keyEqualsComparer)
        : inherited{Factory::Association_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, remove_cvref_t<KEY_EQUALS_COMPARER>>::Default () (
              forward<KEY_EQUALS_COMPARER> (keyEqualsComparer))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (IEqualsComparer<equal_to<KEY_TYPE>, KEY_TYPE>)
        : Association{}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (KEY_EQUALS_COMPARER&& keyEqualsComparer,
                                                                  const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Association{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (ITERABLE_OF_ADDABLE&& src)
        requires (IEqualsComparer<equal_to<KEY_TYPE>, KEY_TYPE> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Association<KEY_TYPE, MAPPED_VALUE_TYPE>>)
        : Association{}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERABLE_OF_ADDABLE&& src)
        : Association{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IEqualsComparer<equal_to<KEY_TYPE>, KEY_TYPE>)
        : Association{}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (KEY_EQUALS_COMPARER&& keyEqualsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Association{forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)}
    {
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (const shared_ptr<_IRep>& rep) noexcept
        : inherited{rep}
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Association (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireExpression (rep != nullptr), move (rep))}
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::GetKeyEqualsComparer () const -> KeyEqualsCompareFunctionType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyEqualsComparer ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Iterable<KEY_TYPE> Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Keys () const
    {
        return this->template Map<Iterable<KEY_TYPE>> ([] (const auto& kvp) { return kvp.fKey; });
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Iterable<MAPPED_VALUE_TYPE> Association<KEY_TYPE, MAPPED_VALUE_TYPE>::MappedValues () const
    {
        return this->template Map<Iterable<MAPPED_VALUE_TYPE>> ([] (const auto& kvp) { return kvp.fValue; });
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Lookup (ArgByValueType<key_type> key) const -> Iterable<mapped_type>
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename THROW_IF_MISSING>
    inline MAPPED_VALUE_TYPE Association<KEY_TYPE, MAPPED_VALUE_TYPE>::LookupOneChecked (ArgByValueType<key_type> key,
                                                                                         const THROW_IF_MISSING&  throwIfMissing) const
    {
        auto tmp = Lookup (key);
        if (auto i = tmp.begin ()) [[likely]] {
            return *i;
        }
        Execution::Throw (throwIfMissing);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline MAPPED_VALUE_TYPE Association<KEY_TYPE, MAPPED_VALUE_TYPE>::LookupOneValue (ArgByValueType<key_type>    key,
                                                                                       ArgByValueType<mapped_type> defaultValue) const
    {
        auto tmp = Lookup (key);
        if (auto i = tmp.begin ()) [[likely]] {
            return *i;
        }
        return defaultValue;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::operator[] (ArgByValueType<key_type> key) const -> const Iterable<mapped_type>
    {
        return Lookup (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Association<KEY_TYPE, MAPPED_VALUE_TYPE>::ContainsKey (ArgByValueType<key_type> key) const
    {
        return not _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key).empty ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline size_t Association<KEY_TYPE, MAPPED_VALUE_TYPE>::OccurrencesOf (ArgByValueType<key_type> key) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key).size ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <Common::IEqualsComparer<MAPPED_VALUE_TYPE> VALUE_EQUALS_COMPARER>
    inline bool Association<KEY_TYPE, MAPPED_VALUE_TYPE>::ContainsMappedValue (ArgByValueType<mapped_type>  v,
                                                                               const VALUE_EQUALS_COMPARER& valueEqualsComparer) const
    {
        return this->Find ([&valueEqualsComparer, &v] (const auto& t) { return valueEqualsComparer (t.fValue, v); }) != nullptr;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<value_type> p)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE> (end); ++i) {
            Add (*i);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (ITERABLE_OF_ADDABLE&& items)
    {
        if constexpr (std::is_convertible_v<remove_cvref_t<ITERABLE_OF_ADDABLE>*, Iterable<value_type>*>) {
            // very rare corner case
            if (static_cast<const Iterable<value_type>*> (this) == static_cast<const Iterable<value_type>*> (&items)) [[unlikely]] {
                vector<value_type> copy{std::begin (items), std::end (items)}; // because you can not iterate over a container while modifying it
                AddAll (std::begin (copy), std::end (copy));
                return;
            }
        }
        AddAll (std::begin (items), std::end (items));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (ArgByValueType<key_type> key)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key)); // use RemoveIf () if key may not exist
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveIf (ArgByValueType<key_type> key)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_KEY_OR_ADDABLE>
    size_t Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (const ITERABLE_OF_KEY_OR_ADDABLE& items)
    {
        using ITEM_T = ranges::range_value_t<ITERABLE_OF_KEY_OR_ADDABLE>;
        static_assert (is_convertible_v<ITEM_T, key_type> or is_convertible_v<ITEM_T, pair<key_type, mapped_type>> or
                       is_convertible_v<ITEM_T, Common::KeyValuePair<key_type, mapped_type>>);
        if (this == &items) { // avoid modifying container while iterating over it
            size_t result = this->size ();
            RemoveAll ();
            return result;
        }
        else {
            return RemoveAll (begin (items), end (items));
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERATOR_OF_KEY_OR_ADDABLE>
    inline size_t Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (ITERATOR_OF_KEY_OR_ADDABLE start, ITERATOR_OF_KEY_OR_ADDABLE end)
    {
        using ITEM_T = ranges::range_value_t<ITERATOR_OF_KEY_OR_ADDABLE>;
        static_assert (is_convertible_v<ITEM_T, key_type> or is_convertible_v<ITEM_T, pair<key_type, mapped_type>> or
                       is_convertible_v<ITEM_T, Common::KeyValuePair<key_type, mapped_type>>);
        size_t cnt{};
        for (auto i = start; i != end; ++i) {
            if constexpr (is_convertible_v<ITEM_T, key_type>) {
                if (RemoveIf (*i)) {
                    ++cnt;
                }
            }
            else if constexpr (is_convertible_v<ITEM_T, pair<key_type, mapped_type>>) {
                if (RemoveIf (i->first)) {
                    ++cnt;
                }
            }
            else if constexpr (is_convertible_v<ITEM_T, Common::KeyValuePair<key_type, mapped_type>>) {
                if (RemoveIf (i->fKey)) {
                    ++cnt;
                }
            }
            else {
                AssertNotReached ();
            }
        }
        return cnt;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <predicate<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> PREDICATE>
    size_t Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (PREDICATE&& p)
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
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue,
                                                                  Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Update (patchedIterator, newValue, nextI);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_KEY_TYPE>
    void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::RetainAll (const ITERABLE_OF_KEY_TYPE& items)
    {
        static_assert (is_convertible_v<ranges::range_value_t<ITERABLE_OF_KEY_TYPE>, key_type>);
        // @see https://stroika.atlassian.net/browse/STK-539
#if 0
                Association<KEY_TYPE, MAPPED_VALUE_TYPE>   result = Association<KEY_TYPE, MAPPED_VALUE_TYPE> { _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().CloneEmpty () };
                for (auto key2Keep : items) {
                    if (auto l = this->Lookup (key2Keep)) {
                        result.Add (key2Keep, *l);
                    }
                }
                *this = result;
#else
        // cannot easily use STL::less because our Association class only requires KeyEqualsCompareFunctionType - SO - should use Stroika Set<> But don't want cross-dependencies if not needed
        set<KEY_TYPE> tmp{items.begin (), items.end ()}; // @todo - weak implementation because of 'comparison' function, and performance (if items already a set)
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
    template <typename RESULT_CONTAINER, invocable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ELEMENT_MAPPER>
    nonvirtual RESULT_CONTAINER Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Map (ELEMENT_MAPPER&& elementMapper) const
        requires (convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, typename RESULT_CONTAINER::value_type> or
                  convertible_to<invoke_result_t<ELEMENT_MAPPER, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>, optional<typename RESULT_CONTAINER::value_type>>)
    {
        if constexpr (same_as<RESULT_CONTAINER, Association>) {
            // clone the rep so we retain any ordering function/etc, rep type
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper),
                                                              RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
        }
        else {
            return inherited::template Map<RESULT_CONTAINER> (forward<ELEMENT_MAPPER> (elementMapper)); // default Iterable<> implementation then...
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <derived_from<Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> RESULT_CONTAINER, typename INCLUDE_PREDICATE>
    inline auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (INCLUDE_PREDICATE&& includeIfTrue) const -> RESULT_CONTAINER
        requires (predicate<INCLUDE_PREDICATE, KEY_TYPE> or predicate<INCLUDE_PREDICATE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    {
        if constexpr (predicate<INCLUDE_PREDICATE, KEY_TYPE>) {
            // recurse once with a KVP predicate
            return Where<RESULT_CONTAINER> (
                [=] (const ArgByValueType<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& kvp) { return includeIfTrue (kvp.fKey); });
        }
        else {
            if constexpr (same_as<RESULT_CONTAINER, Association>) {
                // clone the rep so we retain any ordering function/etc, rep type
                return inherited::template Where<RESULT_CONTAINER> (
                    forward<INCLUDE_PREDICATE> (includeIfTrue), RESULT_CONTAINER{_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().CloneEmpty ()});
            }
            else {
                return inherited::template Where<RESULT_CONTAINER> (forward<INCLUDE_PREDICATE> (includeIfTrue)); // default Iterable<> implementation then...
            }
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEYS>
    auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const CONTAINER_OF_KEYS& includeKeys) const -> ArchetypeContainerType
    {
        return Where ([=] (const key_type& key) -> bool { return includeKeys.Contains (key); });
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const initializer_list<key_type>& includeKeys) const -> ArchetypeContainerType
    {
        Iterable<key_type> ik{includeKeys};
        return inherited::Where ([=] (const ArgByValueType<value_type>& kvp) { return ik.Contains (kvp.fKey); }, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_Key_T>
    inline CONTAINER_OF_Key_T Association<KEY_TYPE, MAPPED_VALUE_TYPE>::As () const
    {
        CONTAINER_OF_Key_T result;
        for (const auto& i : *this) {
            if constexpr (is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>) {
                // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
                result.insert (result.end (), pair<KEY_TYPE, MAPPED_VALUE_TYPE>{i.fKey, i.fValue});
            }
            else {
                result.insert (result.end (), value_type{i.fKey, i.fValue});
            }
        }
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void
    Association<KEY_TYPE, MAPPED_VALUE_TYPE>::Accumulate (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue,
                                                          const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f,
                                                          mapped_type initialValue)
    {
        Add (key, f (LookupValue (key, initialValue), newValue));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::erase (ArgByValueType<key_type> key)
    {
        Remove (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<value_type> nextI{nullptr};
        Remove (i, &nextI);
        return nextI;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::clear ()
    {
        RemoveAll ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE> Association<KEY_TYPE, MAPPED_VALUE_TYPE>::operator+ (const ITERABLE_OF_ADDABLE& items) const
    {
        Association<KEY_TYPE, MAPPED_VALUE_TYPE> result = *this;
        result.AddAll (items);
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>& Association<KEY_TYPE, MAPPED_VALUE_TYPE>::operator+= (const ITERABLE_OF_ADDABLE& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename ITERABLE_OF_KEY_OR_ADDABLE>
    inline Association<KEY_TYPE, MAPPED_VALUE_TYPE>& Association<KEY_TYPE, MAPPED_VALUE_TYPE>::operator-= (const ITERABLE_OF_KEY_OR_ADDABLE& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i)
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
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool Association<KEY_TYPE, MAPPED_VALUE_TYPE>::operator== (const Association& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }

    /*
     ********************************************************************************
     ********** Association<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer ************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <
#if qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy
        typename
#else
        Common::IEqualsComparer<MAPPED_VALUE_TYPE>
#endif
        VALUE_EQUALS_COMPARER>
    constexpr Association<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer<VALUE_EQUALS_COMPARER>::EqualsComparer (const VALUE_EQUALS_COMPARER& valueEqualsComparer)
        : fValueEqualsComparer{valueEqualsComparer}
    {
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <
#if qCompilerAndStdLib_RequiresIEqualsCrashesAssociation_Buggy
        typename
#else
        Common::IEqualsComparer<MAPPED_VALUE_TYPE>
#endif
        VALUE_EQUALS_COMPARER>
    bool Association<KEY_TYPE, MAPPED_VALUE_TYPE>::EqualsComparer<VALUE_EQUALS_COMPARER>::operator() (const Association& lhs, const Association& rhs) const
    {
        /*
         *    @todo   This code is not very efficient, except in the 'quickEqualsTest' case
         */
        _SafeReadRepAccessor<_IRep> lhsR{&lhs};
        _SafeReadRepAccessor<_IRep> rhsR{&rhs};
        if (&lhsR._ConstGetRep () == &rhsR._ConstGetRep ()) {
            // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
            return true;
        }
        // Check length, so we don't need to check both iterators for end/done; length is often a quick computation and always quick compared to this comparison algorithm
        if (lhsR._ConstGetRep ().size () != rhsR._ConstGetRep ().size ()) {
            return false;
        }
        /*
         *  Two Associations compare equal, if they have the same domain, and map each element of that domain to the same range.
         *  They need not be in the same order to compare equals. Still - they often are, and if they are, this algorithm is faster.
         *  If they miss, we need to fall back to a slower strategy.
         */
        auto quickEqualsTest = [&] () -> bool {
            auto li                = lhsR._ConstGetRep ().MakeIterator ();
            auto ri                = rhs.MakeIterator ();
            auto keyEqualsComparer = lhs.GetKeyEqualsComparer (); // arbitrarily select left side key equals comparer
            while (not li.Done ()) {
                Assert (not ri.Done ()); // cuz move at same time and same size
                bool keysEqual = keyEqualsComparer (li->fKey, ri->fKey);
                Require (keysEqual == rhs.GetKeyEqualsComparer () (li->fKey, ri->fKey)); // if fails, cuz rhs/lhs keys equals comparers disagree
                if (not keysEqual or not fValueEqualsComparer (li->fValue, ri->fValue)) {
                    return false;
                }
                // if we got this far, all compared OK so far, so keep going
                ++li;
                ++ri;
            }
            Assert (ri.Done ()); // cuz LHS done and both sides iterate at same pace, and we checked both same size
            return true;
        };
        if (quickEqualsTest ()) {
            return true;
        }
        // OK, we failed the quick test, but the associations might still be 'equal' - just in a funny order
        // note this is extremely expensive. We should find a better algorithm...
        auto keyEqualsComparer = lhs.GetKeyEqualsComparer (); // arbitrarily select left side key equals comparer
        return lhs.MultiSetEquals (rhs, Common::DeclareEqualsComparer ([&] (const value_type& lhs, const value_type& rhs) {
                                       return keyEqualsComparer (lhs.fKey, rhs.fKey) and fValueEqualsComparer (lhs.fKey, rhs.fKey);
                                   }));
    }

}

#endif /* _Stroika_Foundation_Containers_Association_inl_ */
