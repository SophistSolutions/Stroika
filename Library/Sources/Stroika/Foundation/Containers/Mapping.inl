/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
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
        : inherited (move (Factory::Mapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE> () ()))
    {
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer)
        : inherited (move (Factory::Mapping_Factory<KEY_TYPE, MAPPED_VALUE_TYPE, KEY_EQUALS_COMPARER> (forward<KEY_EQUALS_COMPARER> (keyEqualsComparer)) ()))
    {
        _AssertRepValidType ();
    }
#if 0
            //  https://stroika.atlassian.net/browse/STK-541
            template    <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
            inline  Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>&& src) noexcept
                : inherited (move (src))
            {
                _AssertRepValidType ();
            }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping (forward<KEY_EQUALS_COMPARER> (keyEqualsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const initializer_list<pair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : Mapping (forward<KEY_EQUALS_COMPARER> (keyEqualsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const CONTAINER_OF_ADDABLE& src)
        : Mapping ()
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> and not is_convertible_v<const CONTAINER_OF_ADDABLE*, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>*>>*>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, const CONTAINER_OF_ADDABLE& src)
        : Mapping (forward<KEY_EQUALS_COMPARER> (keyEqualsComparer))
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Mapping ()
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (KEY_EQUALS_COMPARER&& keyEqualsComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : Mapping (forward<KEY_EQUALS_COMPARER> (keyEqualsComparer))
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (const _MappingRepSharedPtr& rep) noexcept
        : inherited (rep)
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Mapping (_MappingRepSharedPtr&& rep) noexcept
        : inherited ((RequireNotNull (rep), move (rep)))
    {
        _AssertRepValidType ();
    }
#if qDebug
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::~Mapping ()
    {
        if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
            // SharingState can be NULL because of MOVE semantics
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
        }
    }
#endif
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
        if (optional<MAPPED_VALUE_TYPE> r{Lookup (key)}) {
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
    inline MAPPED_VALUE_TYPE Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::operator[] (ArgByValueType<key_type> key) const
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
        for (MAPPED_VALUE_TYPE t : *this) {
            if (valueEqualsComparer (t.fValue, v)) {
                return true;
            }
        }
        return false;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newElt)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (key, newElt);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Add (ArgByValueType<KeyValuePair<key_type, mapped_type>> p)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (p.fKey, p.fValue);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Add (*i);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEYVALUE, enable_if_t<Configuration::IsIterable_v<CONTAINER_OF_KEYVALUE>>*>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::AddAll (const CONTAINER_OF_KEYVALUE& items)
    {
        /*
         *  Note - unlike other containers - we dont need to check for this != &s because if we
         *  attempt to add items that already exist, it would do nothing to our iteration
         *  and therefore not lead to an infinite loop.
         */
        AddAll (std::begin (items), std::end (items));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (ArgByValueType<key_type> key)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (key);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Remove (const Iterator<value_type>& i)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_ADDABLE>
    void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (const CONTAINER_OF_ADDABLE& items)
    {
        for (auto i : items) {
            Remove (i.first);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Remove (i->first);
        }
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEY_TYPE>
    void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::RetainAll (const CONTAINER_OF_KEY_TYPE& items)
    {
        // @see https://stroika.atlassian.net/browse/STK-539
#if 0
                Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>   result = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> { _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().CloneEmpty (this) };
                for (auto key2Keep : items) {
                    if (auto l = this->Lookup (key2Keep)) {
                        result.Add (key2Keep, *l);
                    }
                }
                *this = result;
#else
        // cannot easily use STL::less because our Mapping class only requires KeyEqualsCompareFunctionType - SO - should use Stroika Set<> But dont want cross-dependencies if not needed
        set<KEY_TYPE> tmp (items.begin (), items.end ()); // @todo - weak implementation because of 'comparison' function, and performance (if items already a set)
        for (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> i = this->begin (); i != this->end (); ++i) {
            if (tmp.find (i->fKey) == tmp.end ()) {
                Remove (i);
            }
        }
#endif
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (const function<bool(ArgByValueType<key_type>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where ([=](const ArgByValueType<KeyValuePair<key_type, mapped_type>>& kvp) { return includeIfTrue (kvp.fKey); }, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Where (const function<bool(ArgByValueType<KeyValuePair<key_type, mapped_type>>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_KEYS>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const CONTAINER_OF_KEYS& includeKeys) const -> ArchetypeContainerType
    {
        return Where ([=](const key_type& key) -> bool { return includeKeys.Contains (key); });
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    auto Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::WithKeys (const initializer_list<key_type>& includeKeys) const -> ArchetypeContainerType
    {
        Iterable<key_type> ik{includeKeys};
        return inherited::Where ([=](const ArgByValueType<KeyValuePair<key_type, mapped_type>>& kvp) { return ik.Contains (kvp.fKey); }, ArchetypeContainerType{});
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
        for (auto i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), pair<KEY_TYPE, MAPPED_VALUE_TYPE> (i.fKey, i.fValue));
        }
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename CONTAINER_OF_Key_T>
    CONTAINER_OF_Key_T Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::As_ (enable_if_t<!is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int>) const
    {
        CONTAINER_OF_Key_T result;
        for (auto i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (i.fKey, i.fValue));
        }
        return result;
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <typename VALUE_EQUALS_COMPARER>
    bool Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::Equals (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs, const VALUE_EQUALS_COMPARER& valueEqualsComparer) const
    {
        /*
         *    @todo   THIS CODE IS TOO COMPLICATED, and COULD USE CLEANUP/CODE REVIEW - LGP 2014-06-11
         */
        _SafeReadRepAccessor<_IRep> lhs{this};
        _SafeReadRepAccessor<_IRep> rhsR{&rhs};
        if (&lhs._ConstGetRep () == &rhsR._ConstGetRep ()) {
            // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
            return true;
        }
        // Check length, so we dont need to check both iterators for end/done
        if (lhs._ConstGetRep ().GetLength () != rhsR._ConstGetRep ().GetLength ()) {
            return false;
        }
        /*
         *  Two Mappings compare equal, if they have the same domain, and map each element of that domain to the same range.
         *  They need not be in the same order to compare equals. Still - they often are, and if they are, this algorithm is faster.
         *  If they miss, we need to fall back to a slower strategy.
         */
        auto li = lhs._ConstGetRep ().MakeIterator (this);
        auto ri = rhs.MakeIterator ();
        while (not li.Done ()) {
            bool keysEqual = GetKeyEqualsComparer () (li->fKey, ri->fKey);
            if (keysEqual and valueEqualsComparer (li->fValue, ri->fValue)) {
                // then this element matches
            }
            else {
                // check if li maps to right value in rhs
                auto o = rhs.Lookup (li->fKey);
                if (not o.has_value () or not valueEqualsComparer (*o, li->fValue)) {
                    return false;
                }
                // if the keys were differnt, we must check the reverse direction too
                if (not keysEqual) {
                    if (not lhs._ConstGetRep ().Lookup (ri->fKey, &o) or not valueEqualsComparer (*o, ri->fValue)) {
                        return false;
                    }
                }
            }
            // if we got this far, all compared OK so far, so keep going
            ++li;
            ++ri;
        }
        return li.Done ();
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
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::erase (const Iterator<value_type>& i)
    {
        Remove (i);
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
    inline void Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_AssertRepValidType () const
    {
#if qDebug
        _SafeReadRepAccessor<_IRep>{this};
#endif
    }

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
                using inherited             = typename Traversal::IterableFromIterator<KEY_TYPE>::_Rep;
                using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;

                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : inherited ()
                    , fMapping_ (map)
                {
                }
                virtual Iterator<KEY_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext]() -> optional<KEY_TYPE> {
                        if (myContext->Done ()) {
                            return optional<KEY_TYPE> ();
                        }
                        else {
                            auto result = (*myContext)->fKey;
                            (*myContext)++;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<KEY_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - ignore forIterableEnvelope
                    return Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyMapping_& m)
                : Iterable<KEY_TYPE> (Iterable<KEY_TYPE>::template MakeSharedPtr<MyIterableRep_> (m))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> (const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_ (Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> (rep));
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    Iterable<MAPPED_VALUE_TYPE> Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep::_Values_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<MAPPED_VALUE_TYPE> {
            using MyMapping_ = Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<MAPPED_VALUE_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using inherited             = typename Traversal::IterableFromIterator<MAPPED_VALUE_TYPE>::_Rep;
                using _IterableRepSharedPtr = typename Iterable<MAPPED_VALUE_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : inherited ()
                    , fMapping_ (map)
                {
                }
                virtual Iterator<MAPPED_VALUE_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext]() -> optional<MAPPED_VALUE_TYPE> {
                        if (myContext->Done ()) {
                            return optional<MAPPED_VALUE_TYPE> ();
                        }
                        else {
                            auto result = (*myContext)->fValue;
                            (*myContext)++;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<MAPPED_VALUE_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - ignore forIterableEnvelope
                    return Iterable<MAPPED_VALUE_TYPE>::template MakeSharedPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyMapping_& m)
                : Iterable<MAPPED_VALUE_TYPE> (Iterable<MAPPED_VALUE_TYPE>::template MakeSharedPtr<MyIterableRep_> (m))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> (const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_ (Mapping<KEY_TYPE, MAPPED_VALUE_TYPE> (rep));
    }

    /*
     ********************************************************************************
     ************************** Mapping operators ***********************************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool operator== (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs)
    {
        return lhs.Equals (rhs);
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline bool operator!= (const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& lhs, const Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>& rhs)
    {
        return not lhs.Equals (rhs);
    }
}
#endif /* _Stroika_Foundation_Containers_Mapping_inl_ */
