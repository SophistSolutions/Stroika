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
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    class KeyedCollection_Array<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRep_,
                                                             public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_EQUALS_COMPARER>> {
    private:
        using inherited = IImplRep_;
        [[no_unique_address]] const KeyExtractorType    fKeyExtractor_;
        [[no_unique_address]] const KEY_EQUALS_COMPARER fKeyComparer_;

    public:
        Rep_ (const KeyExtractorType& keyExtractor, const KEY_EQUALS_COMPARER& keyComparer)
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
        virtual Iterator<value_type> MakeIterator () const override
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
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            if (optional<size_t> i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *i)};
            }
            return nullptr;
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
            if (auto i = this->Find ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
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
            if (auto i = this->Find ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
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
            if (auto i = this->Find ([this, &key] (ArgByValueType<T> item) { return fKeyComparer_ (fKeyExtractor_ (item), key); },
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
     ***************************** KeyedCollection_Array<T> *************************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection_Array{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<KEY_EQUALS_COMPARER>> (keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, KeyedCollection_Array<T, KEY_TYPE, TRAITS>>)
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_Array{KeyExtractorType{}, equal_to<KEY_TYPE>{}}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection_Array{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (const KeyExtractorType& keyExtractor,
                                                                              KEY_EQUALS_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : KeyedCollection_Array{keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection_Array{KeyExtractorType{}, KEY_EQUALS_COMPARER{}}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (KEY_EQUALS_COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                                                       ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : KeyedCollection_Array{KeyExtractorType{}, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IEqualsComparer<KEY_TYPE> KEY_EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    KeyedCollection_Array<T, KEY_TYPE, TRAITS>::KeyedCollection_Array (const KeyExtractorType& keyExtractor, KEY_EQUALS_COMPARER&& keyComparer,
                                                                       ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : KeyedCollection_Array{keyExtractor, forward<KEY_EQUALS_COMPARER> (keyComparer)}
    {
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
