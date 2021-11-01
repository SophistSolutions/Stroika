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
    template <typename KEY_EQUALS_COMPARER, typename KEY_EXTRACTOR, enable_if_t<Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer)
        : KeyedCollection{KEY_EXTRACTOR{}, keyComparer}
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited (Factory::KeyedCollection_Factory<T, KEY_TYPE, TRAITS, KEY_EXTRACTOR, KEY_EQUALS_COMPARER>{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}())
    {
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection{typename TRAITS::DefaultKeyExtractor{}, equal_to<KEY_TYPE>{}}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename CONTAINER_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T> and not is_base_of_v<KeyedCollection<T, KEY_TYPE, TRAITS>, Configuration::remove_cvref_t<CONTAINER_OF_ADDABLE>> and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection{typename TRAITS::DefaultKeyExtractor{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename CONTAINER_OF_ADDABLE, enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::IsIterableOfT_v<CONTAINER_OF_ADDABLE, T>>*>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, CONTAINER_OF_ADDABLE&& src)
        : KeyedCollection{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (src);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection{KEY_EXTRACTOR{}, KEY_EQUALS_COMPARER{}}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename COPY_FROM_ITERATOR_OF_ADDABLE,
              typename KEY_EXTRACTOR,
              typename KEY_EQUALS_COMPARER,
              enable_if_t<
                  Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> ()>*>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection{KEY_EXTRACTOR{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename COPY_FROM_ITERATOR_OF_ADDABLE,
              enable_if_t<
                  KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and Common::IsPotentiallyComparerRelation<KEY_TYPE, KEY_EQUALS_COMPARER> () and Configuration::is_iterator_v<COPY_FROM_ITERATOR_OF_ADDABLE>>*>
    KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer, COPY_FROM_ITERATOR_OF_ADDABLE start, COPY_FROM_ITERATOR_OF_ADDABLE end)
        : KeyedCollection{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AddAll (start, end);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (const _IRepSharedPtr& rep) noexcept
        : inherited{rep}
    {
        RequireNotNull (rep);
        _AssertRepValidType ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline KeyedCollection<T, KEY_TYPE, TRAITS>::KeyedCollection (_IRepSharedPtr&& rep) noexcept
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
        return _SafeReadRepAccessor<_IRep>{this}._ConstGetRep ().Keys ();
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
    inline auto KeyedCollection<T, KEY_TYPE, TRAITS>::LookupChecked (ArgByValueType<KeyType> key, const THROW_IF_MISSING& throwIfMissing) const -> value_type
    {
        if (optional<value_type> r{Lookup (key)}) [[LIKELY_ATTR]] {
            return *r;
        }
        Execution::Throw (throwIfMissing);
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
        return AddAll (std::begin (items), std::end (items));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline unsigned int KeyedCollection<T, KEY_TYPE, TRAITS>::AddAll (const KeyedCollection& items)
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
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (ArgByValueType<value_type> elt)
    {
        _SafeReadWriteRepAccessor<_IRep> r{this};
        r._GetWriteableRep ().Remove (r._ConstGetRep ().GetKeyExtractor () (elt));
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::Remove (const Iterator<value_type>& i, const Iterator<value_type>* nextI)
    {
        Require (not i.Done ());
        auto [writerRep, patchedIterator] = _GetWriterRepAndPatchAssociatedIterator (i);
        Debug::UncheckedDynamicCast<_IRep*> (writerRep.get ())->Remove (patchedIterator, nextI);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool KeyedCollection<T, KEY_TYPE, TRAITS>::RemoveIf (ArgByValueType<KeyType> key)
    {
        return _SafeReadWriteRepAccessor<_IRep>{this}._GetWriteableRep ().Remove (key);
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
        _SafeReadWriteRepAccessor<_IRep> tmp{this};
        if (not tmp._ConstGetRep ().IsEmpty ()) {
            tmp._UpdateRep (tmp._ConstGetRep ().CloneEmpty ());
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
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::Where (const function<bool (ArgByValueType<value_type>)>& includeIfTrue) const -> ArchetypeContainerType
    {
        return inherited::Where (includeIfTrue, ArchetypeContainerType{});
    }
#if __cpp_impl_three_way_comparison >= 201907
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
#endif
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
    auto KeyedCollection<T, KEY_TYPE, TRAITS>::_GetWriterRepAndPatchAssociatedIterator (const Iterator<value_type>& i) -> tuple<typename inherited::_SharedByValueRepType::shared_ptr_type, Iterator<value_type>>
    {
        Require (not i.Done ());
        using element_type                   = typename inherited::_SharedByValueRepType::element_type;
        using shared_ptr_type                = typename inherited::_SharedByValueRepType::shared_ptr_type;
        Iterator<value_type> patchedIterator = i;
        shared_ptr_type      writerRep       = this->_fRep.rwget_ptr (
            [&] (const element_type& prevRepPtr) -> shared_ptr_type {
                return Debug::UncheckedDynamicCast<_IRep*> (prevRepPtr.get ())->CloneAndPatchIterator (&patchedIterator);
            });
        return make_tuple (writerRep, patchedIterator);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection<T, KEY_TYPE, TRAITS>::_AssertRepValidType () const
    {
#if qDebug
        [[maybe_unused]] _SafeReadRepAccessor<_IRep> ignored{this};
#endif
    }

    /*
     ********************************************************************************
     ****************** KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep *****************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    Iterable<KEY_TYPE> KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep::_Keys_Reference_Implementation () const
    {
        struct MyIterable_ : Iterable<KEY_TYPE> {
            using BaseCollectionType_ = KeyedCollection<T, KEY_TYPE, TRAITS>;
            struct MyIterableRep_ : Traversal::IterableFromIterator<KEY_TYPE>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using _IterableRepSharedPtr = typename Iterable<KEY_TYPE>::_IterableRepSharedPtr;
                BaseCollectionType_ fBaseCollection_;
                MyIterableRep_ (const BaseCollectionType_& map)
                    : fBaseCollection_{map}
                {
                }
                virtual Iterator<KEY_TYPE> MakeIterator () const override
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
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<KEY_TYPE>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const BaseCollectionType_& m)
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
        if (lhsR._ConstGetRep ().GetLength () != rhsR._ConstGetRep ().GetLength ()) {
            return false;
        }

        /*
         *  Order is meaningless for KeyedCollection<>, so treat like set. Iterate over Iterable<T> on rhs, and check for contains
         *  on LHS (known to have relatively fast lookup).
         * 
         *  Leverage fact we know by this point the two iterables have the same length.
         */
        for (auto ri : rhs) {
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

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     ******************** Mapping comparison operators ******************************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool operator== (const KeyedCollection<T, KEY_TYPE, TRAITS>& lhs, const KeyedCollection<T, KEY_TYPE, TRAITS>& rhs)
    {
        return typename KeyedCollection<T, KEY_TYPE, TRAITS>::EqualsComparer{}(lhs, rhs);
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline bool operator!= (const KeyedCollection<T, KEY_TYPE, TRAITS>& lhs, const KeyedCollection<T, KEY_TYPE, TRAITS>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

}

#endif /* _Stroika_Foundation_Containers_KeyedCollection_inl_ */
