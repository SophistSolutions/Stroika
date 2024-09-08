/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Configuration/Concepts.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::Rep_ ***********
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IThreeWayComparer<KEY_TYPE>) COMPARER>
    class SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRepBase_,
                                                                      public Memory::UseBlockAllocationIfAppropriate<Rep_<COMPARER>> {
    public:
        static_assert (not is_reference_v<COMPARER>);

    private:
        using inherited = IImplRepBase_;

    private:
        [[no_unique_address]] const KeyExtractorType fKeyExtractor_;

    public:
        Rep_ (const KeyExtractorType& keyExtractor, const COMPARER& comparer)
            : fKeyExtractor_{keyExtractor}
            , fData_{comparer}
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
            fData_.Apply ([&] (auto arg) { doToElement (arg.fKey); });
        }
        virtual Iterator<T> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, fData_.Find (that))};
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, found)};
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
            return KeyEqualityComparerType{Common::EqualsComparerAdapter<T, COMPARER>{fData_.key_comp ()}};
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, fData_.key_comp ()); // keep extractor/comparer but lose data in clone
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
            auto                                                  i = fData_.find (key);
            if (i == fData_.end ()) {
                if (item != nullptr) {
                    *item = nullopt;
                }
                return false;
            }
            else {
                if (item != nullptr) {
                    *item = *i;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            pair<typename DataStructureImplType_::iterator, bool>  flagAndI = fData_.insert (item);
            if (flagAndI.second) {
                return true;
            }
            else {
                // @todo must patch!!!
                // in case of update, set<> wont update the value so we must remove and re-add, but todo that, use previous iterator as hint
                typename DataStructureImplType_::iterator hint = flagAndI.first;
                ++hint;
                fData_.erase (flagAndI.first);
                fData_.insert (hint, item);
                return false;
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            mir.fIterator.AssertDataMatches (&fData_);
            auto nextIResult = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, nextIResult)};
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.Remove (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }

        // SortedKeyedCollection<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return KeyInOrderKeyComparerType{Common::InOrderComparerAdapter<T, COMPARER>{fData_.key_comp ()}};
        }

    private:
        using DataStructureImplType_ = SKIPLIST<COMPARER>;
        struct IteratorRep_ : Private::IteratorImplHelper_<value_type, DataStructureImplType_> {
            using inherited = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;
            using inherited::inherited; // forward base class constructors
            // override to map just the key part to 'T'
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                this->ValidateChangeCount ();
                if (advance) [[likely]] {
                    Require (not this->fIterator.Done ());
                    ++this->fIterator;
                }
                if (this->fIterator.Done ()) [[unlikely]] {
                    *result = nullopt;
                }
                else {
                    *result = this->fIterator->fKey;
                }
            }
            virtual auto Clone () const -> unique_ptr<typename Iterator<T>::IRep> override
            {
                this->ValidateChangeCount ();
                return make_unique<IteratorRep_> (*this);
            }
        };

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_SkipList<KEY_TYPE,MAPPED_VALUE_TYPE> **********
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList ()
        requires (three_way_comparable<KEY_TYPE> and IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_SkipList{KeyExtractorType{}, compare_three_way{}}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (COMPARER&& keyComparer)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_SkipList{KeyExtractorType{}, forward<COMPARER> (keyComparer)}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor)
        requires (three_way_comparable<KEY_TYPE>)
        : SortedKeyedCollection_SkipList{keyExtractor, compare_three_way{}}
    {
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER>
    SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor, COMPARER&& keyComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<COMPARER>>> (keyExtractor, forward<COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IThreeWayComparer<T> COMPARER>
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>>)
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection_SkipList{KeyExtractorType{}, COMPARER{}}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IThreeWayComparer<T> COMPARER>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor,
                                                                                                COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection_SkipList{keyExtractor, forward<COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IThreeWayComparer<T> COMPARER>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_SkipList{KeyExtractorType{}, COMPARER{}}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IThreeWayComparer<T> COMPARER>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                                                                                ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_SkipList{KeyExtractorType{}, forward<COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IThreeWayComparer<T> COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_SkipList (const KeyExtractorType& keyExtractor,
                                                                                                COMPARER&& keyComparer, ITERATOR_OF_ADDABLE&& start,
                                                                                                ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection_SkipList{keyExtractor, forward<COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection_SkipList<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
