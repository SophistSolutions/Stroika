/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
    template <Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer)
        requires (KeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited{Factory::KeyedCollection_Factory<T, KEY_TYPE, TRAITS, decay_t<KEY_EQUALS_COMPARER>>::Default () (
              keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
        _AssertRepValidType ();
    }
#if !qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (ITERABLE_OF_ADDABLE&& src)
        requires (KeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and
                  not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>)
        : KeyedCollection{KeyExtractorType{}, equal_to<KEY_TYPE>{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        _AssertRepValidType ();
    }
#endif
#if !qCompilerAndStdLib_template_Requires_templateDeclarationMatchesOutOfLine2_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (KeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and
                  not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>>)
        : KeyedCollection{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAll (src);
        _AssertRepValidType ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer,
                                                                  ITERABLE_OF_ADDABLE&& src)
        requires (Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>)
        : KeyedCollection{keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <forward_iterator ITERATOR_OF_ADDABLE, Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (KeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection{KeyExtractorType{}, KEY_EQUALS_COMPARER{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <forward_iterator ITERATOR_OF_ADDABLE, Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (KeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <Common::EqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, forward_iterator ITERATOR_OF_ADDABLE>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer,
                                                           ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection{keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const shared_ptr<_IRep>& rep) noexcept
        : inherited{rep}
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (shared_ptr<_IRep>&& rep) noexcept
        : inherited{(RequireNotNull (rep), move (rep))}
    {
        _AssertRepValidType ();
    }
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
        return this->template Map<KEY_TYPE> ([keyExtractor = this->GetKeyExtractor ()] (const T& t) { return keyExtractor (t); });
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Contains (ArgByValueType<KEY_TYPE> key) const
    {
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Lookup (key, nullptr);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Contains (ArgByValueType<value_type> elt) const
    {
        _SafeReadRepAccessor<_IRep> r{this};
        return r._ConstGetRep ().Lookup (r._ConstGetRep ().GetKeyExtractor () (elt), nullptr);
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
    template <typename THROW_IF_MISSING>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::LookupChecked (ArgByValueType<KeyType> key, THROW_IF_MISSING&& throwIfMissing) const -> value_type
    {
        if (optional<value_type> r{Lookup (key)}) [[likely]] {
            return *r;
        }
        Execution::Throw (forward<THROW_IF_MISSING> (throwIfMissing));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::LookupValue (ArgByValueType<KeyType> key, ArgByValueType<value_type> defaultValue) const -> value_type
    {
        optional<value_type> r{Lookup (key)};
        return r.has_value () ? *r : defaultValue;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::Add (ArgByValueType<value_type> t)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Add (t);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE>
    unsigned int KeyedCollection<T, KEY_TYPE, TRAITS>::AddAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        unsigned int                     cntAdded{};
        _SafeReadWriteRepAccessor<_IRep> r{this};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != forward<ITERATOR_OF_ADDABLE> (end); ++i) {
            if (r._GetWriteableRep ().Add (*i)) {
                ++cntAdded;
            }
        }
        return cntAdded;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline unsigned int KeyedCollection<T, KEY_TYPE, TRAITS>::AddAll (ITERABLE_OF_ADDABLE&& items)
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        if constexpr (is_same_v<decay_t<ITERABLE_OF_ADDABLE>, KeyedCollection>) {
            // avoid trouble with a.AddAll(a);
            if (this != &items) {
                return AddAll (std::begin (items), std::end (items));
            }
            return 0;
        }
        else {
            return AddAll (std::begin (items), std::end (items));
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (ArgByValueType<KeyType> key)
    {
        Verify (_SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (ArgByValueType<value_type> elt)
    {
        _SafeReadWriteRepAccessor<_IRep> writeAccessor{this};
        Verify (writeAccessor._GetWriteableRep ().RemoveIf (writeAccessor._ConstGetRep ().GetKeyExtractor () (elt)));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (const Iterator<value_type>& i, const Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWritableRepAndPatchAssociatedIterator (i);
        writerRep->Remove (patchedIterator, nextI);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveIf (ArgByValueType<KeyType> key)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().RemoveIf (key);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveIf (ArgByValueType<value_type> elt)
    {
        _SafeReadWriteRepAccessor<_IRep> r{this};
        return r._GetWriteableRep ().Remove (r._ConstGetRep ().GetKeyExtractor () (elt));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll ()
    {
        _SafeReadRepAccessor<_IRep> tmp{this}; // important to use READ not WRITE accessor, because write accessor would have already cloned the data
        if (not tmp._ConstGetRep ().empty ()) {
            this->_fRep = tmp._ConstGetRep ().CloneEmpty ();
        }
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename PREDICATE, enable_if_t<Configuration::IsTPredicate<T, PREDICATE> ()>*>
    size_t KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll (PREDICATE&& p)
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
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE>
    inline size_t KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll (const ITERABLE_OF_ADDABLE& items)
    {
        size_t cnt{};
        for (const auto& i : items) {
            Remove (i.first);
            ++cnt;
        }
        return cnt;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE>
    inline size_t KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveAll (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
    {
        size_t cnt{};
        for (auto i = forward<ITERATOR_OF_ADDABLE> (start); i != end; ++i) {
            Remove (i->first);
            ++cnt;
        }
        return cnt;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::operator== (const KeyedCollection& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::operator== (const Iterable<value_type>& rhs) const
    {
        return EqualsComparer{}(*this, rhs);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator+= (ArgByValueType<value_type> item)
    {
        Add (item);
        return *this;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator+= (const Iterable<value_type>& items)
    {
        AddAll (items);
        return *this;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator-= (ArgByValueType<value_type> item)
    {
        Remove (item);
        return *this;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>& KeyedCollection<T, KEY_TYPE, TRAITS>::operator-= (const Iterable<value_type>& items)
    {
        RemoveAll (items);
        return *this;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::clear ()
    {
        RemoveAll ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::erase (ArgByValueType<value_type> item)
    {
        this->Remove (item);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::erase (const Iterator<value_type>& i) -> Iterator<value_type>
    {
        Iterator<T> nextI{nullptr};
        this->Remove (i, &nextI);
        return nextI;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::_GetWritableRepAndPatchAssociatedIterator (const Iterator<value_type>& i)
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
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
        if constexpr (qDebug) {
            [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
        }
    }

    /*
     ********************************************************************************
     ********** KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer ****************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer::operator() (const KeyedCollection& lhs, const KeyedCollection& rhs) const
    {
        return operator() (lhs, static_cast<const Iterable<T>&> (rhs)); // use common-code
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    bool KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer::operator() (const KeyedCollection& lhs, const Iterable<value_type>& rhs) const
    {
        _SafeReadRepAccessor<_IRep> lhsR{&lhs};
        _SafeReadRepAccessor<_IRep> rhsR{&rhs};
        if (&lhsR._ConstGetRep () == &rhsR._ConstGetRep ()) {
            // not such an unlikely test result since we use lazy copy, but this test is only an optimization and not logically required
            return true;
        }
        // Check length, so we don't need to check both iterators for end/done
        if (lhsR._ConstGetRep ().size () != rhsR._ConstGetRep ().size ()) {
            return false;
        }

        /*
         *  Order is meaningless for KeyedCollection<>, so treat like set. Iterate over Iterable<T> on rhs, and check for contains
         *  on LHS (known to have relatively fast lookup).
         * 
         *  Leverage fact we know by this point the two iterables have the same length.
         */
        for (const auto& ri : rhs) {
            if (not lhs.Contains (ri)) {
                return false;
            }
        }
        return true;
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer::operator() (const Iterable<value_type>& lhs, const KeyedCollection& rhs) const
    {
        return operator() (rhs, lhs); // use commutativity of ==
    }

}

#endif /* _Stroika_Foundation_Containers_KeyedCollection_inl_ */
