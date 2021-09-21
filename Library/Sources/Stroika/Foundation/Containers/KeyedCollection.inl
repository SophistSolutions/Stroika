/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_KeyedCollection_inl_
#define _Stroika_Foundation_Containers_KeyedCollection_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../Debug/Assertions.h"
#include "../Traversal/Generator.h"
#include "Factory/KeyedCollection_Factory.h"

namespace Stroika::Foundation::Containers {

    /*
     ********************************************************************************
     ******************** KeyedCollection<T, KEY_TYPE, TRAITS> **********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KE, enable_if_t<Configuration::is_callable_v<KE>>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KeyEqualityComparerType keyComparer)
        : KeyedCollection{TRAITS::kDefaultKeyExtractor, keyComparer}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
        // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
        : inherited (Factory::KeyedCollection_Factory<T, KEY_TYPE, TRAITS>{keyExtractor, keyComparer}())
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KE, enable_if_t<Configuration::is_callable_v<KE>>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const initializer_list<T>& src, KeyEqualityComparerType keyComparer)
        : KeyedCollection{TRAITS::kDefaultKeyExtractor, keyComparer}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const initializer_list<T>& src, KeyExtractorType keyExtractor, KeyEqualityComparerType keyComparer)
        : KeyedCollection{keyExtractor, keyComparer}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const _KeyedCollectionRepSharedPtr& rep) noexcept
        : inherited{rep}
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (_KeyedCollectionRepSharedPtr&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
#if qDebug
    template <typename T, typename KEY_TYPE, typename TRAITS>
    KeyedCollection<T, KEY_TYPE, TRAITS>::~KeyedCollection ()
    {
        if (this->_GetSharingState () != Memory::SharedByValue_State::eNull) {
            // SharingState can be NULL because of MOVE semantics
            _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().AssertNoIteratorsReferenceOwner (this);
        }
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::GetKeyExtractor () const -> KeyExtractorType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyExtractor ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::GetKeyEqualityComparer () const -> KeyEqualityComparerType
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().GetKeyEqualityComparer ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline Iterable<KEY_TYPE> KeyedCollection<T, KEY_TYPE, TRAITS>::Keys () const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Keys ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Contains (ArgByValueType<KEY_TYPE> key) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Contains (ArgByValueType<T> elt) const
    {
        _SafeReadRepAccessor<_IRep> r{this};
        return r._ConstGetRep ().Lookup (r._ConstGetRep ().GetKeyExtractor (elt), nullptr);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::Lookup (ArgByValueType<KeyType> key) const -> optional<value_type>
    {
        optional<T>           r;
        [[maybe_unused]] bool result = _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &r);
        Ensure (result == r.has_value ());
        return r;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const
    {
        if (item == nullptr) {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
        }
        else {
            optional<value_type> tmp;
            if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                *item = *tmp;
                return true;
            }
            return false;
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Lookup (ArgByValueType<KeyType> key, value_type* item) const
    {
        if (item == nullptr) {
            return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
        }
        else {
            optional<value_type> tmp;
            if (_SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, &tmp)) {
                *item = *tmp;
                return true;
            }
            return false;
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Lookup (ArgByValueType<KeyType> key, nullptr_t) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Add (ArgByValueType<T> t)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (t);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    unsigned int KeyedCollection<T, KEY_TYPE, TRAITS>::AddAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        unsigned int                     cntAdded{};
        _SafeReadWriteRepAccessor<_IRep> r{this};
        for (auto i = start; i != end; ++i) {
            if (r._GetWriteableRep ().Add (*i)) {
                cntAdded++;
            }
        }
        return cntAdded;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE, enable_if_t<Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline unsigned int KeyedCollection<T, KEY_TYPE, TRAITS>::AddAll (CONTAINER_OF_ADDABLE&& items)
    {
        // avoid trouble with a.AddAll(a);
        if (this != &items) {
            return AddAll (std::begin (items), std::end (items));
        }
        return 0;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (ArgByValueType<KeyType> key)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (key);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (ArgByValueType<T> elt)
    {
        _SafeReadWriteRepAccessor<_IRep> r{this};
        r._GetWriteableRep ().Remove (r._ConstGetRep ().GetKeyExtractor () (elt));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (const Iterator<T>& i)
    {
        _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (i);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveIf (ArgByValueType<KeyType> key)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (key);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveIf (ArgByValueType<T> elt)
    {
        _SafeReadWriteRepAccessor<_IRep> r{this};
        return r._GetWriteableRep ().Remove (r._ConstGetRep ().GetKeyExtractor () (elt));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll ()
    {
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty (this));
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE>
    void KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll (const CONTAINER_OF_ADDABLE& items)
    {
        for (auto i : items) {
            Remove (i.first);
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE>
    void KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
    {
        for (auto i = start; i != end; ++i) {
            Remove (i->first);
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::Where (const function<bool (ArgByValueType<T>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }

    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
#if qDebug
        [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
#endif
    }

#if 0
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_KEY_TYPE>
    void KeyedCollection<T, KEY_TYPE, TRAITS>::RetainAll (const CONTAINER_OF_KEY_TYPE& items)
    {
        // @see https://stroika.atlassian.net/browse/STK-539
#if 0
                KeyedCollection<T, KEY_TYPE, TRAITS>   result = KeyedCollection<T, KEY_TYPE, TRAITS> { _SafeReadRepAccessor<_IRep> { this } ._ConstGetRep ().CloneEmpty (this) };
                for (auto key2Keep : items) {
                    if (auto l = this->Lookup (key2Keep)) {
                        result.Add (key2Keep, *l);
                    }
                }
                *this = result;
#else
        // cannot easily use STL::less because our Mapping class only requires KeyEqualsCompareFunctionType - SO - should use Stroika Set<> But don't want cross-dependencies if not needed
        set<KEY_TYPE> tmp (items.begin (), items.end ()); // @todo - weak implementation because of 'comparison' function, and performance (if items already a set)
        for (Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> i = this->begin (); i != this->end (); ++i) {
            if (tmp.find (i->fKey) == tmp.end ()) {
                Remove (i);
            }
        }
#endif
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::Where (const function<bool (ArgByValueType<KeyValuePair<key_type, mapped_type>>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_KEYS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::WithKeys (const CONTAINER_OF_KEYS& includeKeys) const -> ArchetypeContainerType
    {
        return Where ([=] (const key_type& key) -> bool { return includeKeys.Contains (key); });
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::WithKeys (const initializer_list<key_type>& includeKeys) const -> ArchetypeContainerType
    {
        Iterable<key_type> ik{includeKeys};
        return inherited::Where ([=] (const ArgByValueType<KeyValuePair<key_type, mapped_type>>& kvp) { return ik.Contains (kvp.fKey); }, ArchetypeContainerType{});
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_Key_T>
    inline CONTAINER_OF_Key_T KeyedCollection<T, KEY_TYPE, TRAITS>::As () const
    {
        return As_<CONTAINER_OF_Key_T> ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_Key_T>
    CONTAINER_OF_Key_T KeyedCollection<T, KEY_TYPE, TRAITS>::As_ ([[maybe_unused]] enable_if_t<is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int> usesInsertPair) const
    {
        CONTAINER_OF_Key_T result;
        for (auto i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), pair<KEY_TYPE, MAPPED_VALUE_TYPE> (i.fKey, i.fValue));
        }
        return result;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_Key_T>
    CONTAINER_OF_Key_T KeyedCollection<T, KEY_TYPE, TRAITS>::As_ (enable_if_t<!is_convertible_v<typename CONTAINER_OF_Key_T::value_type, pair<KEY_TYPE, MAPPED_VALUE_TYPE>>, int>) const
    {
        CONTAINER_OF_Key_T result;
        for (auto i : *this) {
            // the reason we use the overload with an extra result.end () here is so it will work with std::map<> or std::vector<>
            result.insert (result.end (), KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE> (i.fKey, i.fValue));
        }
        return result;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Accumulate (ArgByValueType<key_type> key, ArgByValueType<mapped_type> newValue, const function<mapped_type (ArgByValueType<mapped_type>, ArgByValueType<mapped_type>)>& f, mapped_type initialValue)
    {
        Add (key, f (LookupValue (key, initialValue), newValue));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::erase (ArgByValueType<key_type> key)
    {
        Remove (key);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::erase (const Iterator<value_type>& i)
    {
        Remove (i);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::clear ()
    {
        RemoveAll ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE>
    inline KeyedCollection<T, KEY_TYPE, TRAITS> KeyedCollection<T, KEY_TYPE, TRAITS>::operator+ (const CONTAINER_OF_ADDABLE& items) const
    {
        KeyedCollection<T, KEY_TYPE, TRAITS> result = *this;
        result.AddAll (items);
        return result;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator+= (const CONTAINER_OF_ADDABLE& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator-= (const CONTAINER_OF_ADDABLE& items)
    {
        RemoveAll (items);
        return *this;
    }
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::operator== (const Mapping& rhs) const
    {
        return EqualsComparer<>{}(*this, rhs);
    }
#endif
#endif

    /*
     ********************************************************************************
     ****************** KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep *****************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    Iterable<KEY_TYPE> KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<KEY_TYPE> {
            using MyMapping_ = KeyedCollection<T, KEY_TYPE, TRAITS>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fBaseCollection_;
                MyIterableRep_ (const MyMapping_& map)
                    : fBaseCollection_{map}
                {
                }
                virtual Iterator<KEY_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext    = make_shared<Iterator<T>> (fBaseCollection_.MakeIterator ());
                    auto keyExtractor = fBaseCollection_.GetKeyExtractor ();
                    auto getNext      = [myContext, keyExtractor] () -> optional<KEY_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
                        }
                        else {
                            auto result = keyExtractor (**myContext);
                            (*myContext)++;
                            return result;
                        }
                    };
                    return Traversal::CreateGeneratorIterator<KEY_TYPE> (getNext);
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - ignore forIterableEnvelope
                    return Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyMapping_& m)
                // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
                : Iterable<KEY_TYPE>{Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (m)}
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> (const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{KeyedCollection<T, KEY_TYPE, TRAITS>{rep}};
    }
#if 0
    template <typename T, typename KEY_TYPE, typename TRAITS>
    Iterable<KEY_TYPE> KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<KEY_TYPE> {
            using MyMapping_ = KeyedCollection<T, KEY_TYPE, TRAITS>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : fMapping_{map}
                {
                }
                virtual Iterator<KEY_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<KEY_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
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
                    return Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const MyMapping_& m)
                // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
                : Iterable<KEY_TYPE> (Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (m))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> (const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{KeyedCollection<T, KEY_TYPE, TRAITS> (rep)};
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    Iterable<MAPPED_VALUE_TYPE> KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep::_Values_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<MAPPED_VALUE_TYPE> {
            using MyMapping_ = KeyedCollection<T, KEY_TYPE, TRAITS>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<MAPPED_VALUE_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<MAPPED_VALUE_TYPE>::_IterableRepSharedPtr;
                MyMapping_ fMapping_;
                MyIterableRep_ (const MyMapping_& map)
                    : fMapping_{map}
                {
                }
                virtual Iterator<MAPPED_VALUE_TYPE> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                {
                    auto myContext = make_shared<Iterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>> (fMapping_.MakeIterator ());
                    auto getNext   = [myContext] () -> optional<MAPPED_VALUE_TYPE> {
                        if (myContext->Done ()) {
                            return nullopt;
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
                    return Iterable<MAPPED_VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            // @todo debug if/why issue with using uninform initializaiton here - fails to compile on vs2k17 and gcc ASAN giving erorrs that maybe related???
            MyIterable_ (const MyMapping_& m)
                // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
                : Iterable<MAPPED_VALUE_TYPE> (Iterable<MAPPED_VALUE_TYPE>::template MakeSmartPtr<MyIterableRep_> (m))
            {
            }
        };
#if qStroika_Foundation_Traveral_IterableUsesSharedFromThis_
        auto rep = dynamic_pointer_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> (const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ());
#else
        auto rep = const_cast<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep*> (this)->shared_from_this ();
#endif
        return MyIterable_{KeyedCollection<T, KEY_TYPE, TRAITS>{rep}};
    }
#endif

#if 0
    /*
     ********************************************************************************
     ********** KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer ****************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename VALUE_EQUALS_COMPARER>
    constexpr KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer<VALUE_EQUALS_COMPARER>::EqualsComparer (const VALUE_EQUALS_COMPARER& valueEqualsComparer)
        : fValueEqualsComparer{valueEqualsComparer}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename VALUE_EQUALS_COMPARER>
    bool KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer<VALUE_EQUALS_COMPARER>::operator() (const Mapping& lhs, const Mapping& rhs) const
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
        auto li                = lhsR._ConstGetRep ().MakeIterator (this);
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
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool operator== (const KeyedCollection<T, KEY_TYPE, TRAITS>& lhs, const KeyedCollection<T, KEY_TYPE, TRAITS>& rhs)
    {
        return typename KeyedCollection<T, KEY_TYPE, TRAITS>::template EqualsComparer<>{}(lhs, rhs);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool operator!= (const KeyedCollection<T, KEY_TYPE, TRAITS>& lhs, const KeyedCollection<T, KEY_TYPE, TRAITS>& rhs)
    {
        return not(lhs == rhs);
    }
#endif
#endif

}

#endif /* _Stroika_Foundation_Containers_KeyedCollection_inl_ */
