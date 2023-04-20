/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/Array.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class KeyedCollection_Array<T, KEY_TYPE, TRAITS>::IImplRep_ : public KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {};

    /*
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER>
    class KeyedCollection_Array<T, KEY_TYPE, TRAITS>::Rep_
        : public IImplRep_,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EXTRACTOR, KEY_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRep_;
        [[no_unique_address]] const KEY_EXTRACTOR       fKeyExtractor_;
        [[no_unique_address]] const KEY_EQUALS_COMPARER fKeyComparer_;

    public:
        Rep_ (const KEY_EXTRACTOR& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{keyComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<T>::_IRep> Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find ([[maybe_unused]] const shared_ptr<typename Iterable<T>::_IRep>& thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>&         that,
                                           [[maybe_unused]] Execution::SequencePolicy                      seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            size_t                                                i = fData_.Find (that);
            if (i == fData_.size ()) {
                return nullptr;
            }
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, i)};
        }

        // KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep overrides
    public:
        virtual KeyExtractorType GetKeyExtractor () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fKeyExtractor_;
        }
        virtual KeyEqualityComparerType GetKeyEqualityComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return KeyEqualityComparerType{fKeyComparer_};
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_); // lose data but keep compare functions
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Lookup (ArgByValueType<KeyType> key, optional<value_type>* item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (auto i = this->Find (
                    nullptr, [this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
                    Execution::SequencePolicy::eDefault)) {
                if (item != nullptr) {
                    *item = *i;
                }
                return true;
            }
            return false;
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            KEY_TYPE                                               key{fKeyExtractor_ (item)};
            if (auto i = this->Find (
                    nullptr, [this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
                    Execution::SequencePolicy::eDefault)) {
                auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
                fData_.SetAt (mir.fIterator, item);
                fChangeCounts_.PerformedChange ();
                return false;
            }
            else {
                fData_.push_back (item); // order meaningless for collection, and append cheaper on array
                fChangeCounts_.PerformedChange ();
                return true;
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (nextI != nullptr) {
                *nextI = i;
                ++(*nextI);
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                Debug::UncheckedDynamicCast<IteratorRep_&> (nextI->GetRep ()).UpdateChangeCount ();
                nextI->Refresh (); // update to reflect changes made to rep
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            if (auto i = this->Find (
                    nullptr, [this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
                    Execution::SequencePolicy::eDefault)) {
                Remove (i, nullptr);
                return true;
            }
            return false;
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************** KeyedCollection_Array<T> ***********************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EQUALS_COMPARER, typename KEY_EXTRACTOR,
              enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer)
        : KeyedCollection_Array{KEY_EXTRACTOR{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER,
              enable_if_t<Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<KEY_EXTRACTOR, KEY_EQUALS_COMPARER>> (forward<KEY_EXTRACTOR> (keyExtractor),
                                                                                     forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER,
              enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_Array{typename TRAITS::DefaultKeyExtractor{}, equal_to<KEY_TYPE>{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERABLE_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER,
              enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<KeyedCollection_Array<T, KEY_TYPE, TRAITS>, decay_t<ITERABLE_OF_ADDABLE>> and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> ()>*>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_Array{typename TRAITS::DefaultKeyExtractor{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERABLE_OF_ADDABLE,
              enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterable_v<ITERABLE_OF_ADDABLE>>*>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EXTRACTOR&&       keyExtractor,
                                                                              KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_Array{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER,
              enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>*>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_Array{KEY_EXTRACTOR{}, KEY_EQUALS_COMPARER{}}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename ITERATOR_OF_ADDABLE, typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER,
              enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_ADDABLE> and KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> ()>*>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                                                       ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_Array{KEY_EXTRACTOR{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <typename KEY_EXTRACTOR, typename KEY_EQUALS_COMPARER, typename ITERATOR_OF_ADDABLE,
              enable_if_t<KeyedCollection_IsKeyExctractor<T, KEY_TYPE, KEY_EXTRACTOR> () and
                          Common::IsEqualsComparer<KEY_EQUALS_COMPARER, KEY_TYPE> () and Configuration::IsIterator_v<ITERATOR_OF_ADDABLE>>*>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EXTRACTOR&& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer,
                                                                       ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_Array{forward<KEY_EXTRACTOR> (keyExtractor), forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void KeyedCollection_Array<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_KeyedCollection_Array_inl_ */
