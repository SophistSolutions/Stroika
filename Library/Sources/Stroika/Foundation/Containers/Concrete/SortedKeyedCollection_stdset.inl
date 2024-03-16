/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdset_inl_

#include "../../Configuration/Concepts.h"
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *** SortedKeyedCollection_stdset<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::IImplRepBase_ : public SortedKeyedCollection<T, KEY_TYPE, TRAITS>::_IRep {};

    /*
     ********************************************************************************
     ********** SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ *************
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IInOrderComparer<KEY_TYPE>) KEY_INORDER_COMPARER>
    class SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::Rep_ : public IImplRepBase_,
                                                                    public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_INORDER_COMPARER>> {
    public:
        static_assert (not is_reference_v<KEY_INORDER_COMPARER>);

    private:
        using inherited = IImplRepBase_;

    private:
        [[no_unique_address]] const KeyExtractorType     fKeyExtractor_;
        [[no_unique_address]] const KEY_INORDER_COMPARER fKeyComparer_;

    public:
        Rep_ (const KeyExtractorType& keyExtractor, const KEY_INORDER_COMPARER& inorderComparer)
            : fKeyExtractor_{keyExtractor}
            , fKeyComparer_{inorderComparer}
#if qCompilerAndStdLib_deduce_template_arguments_CTOR_Buggy
            , fData_{SetInOrderComparer<KEY_INORDER_COMPARER> {
                keyExtractor,
                inorderComparer
            }}
#else
            , fData_{SetInOrderComparer{keyExtractor, inorderComparer}}
#endif
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
        virtual Iterator<T> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, found)};
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
            auto                                                  keyComparer = fKeyComparer_;
            return KeyEqualityComparerType{
                [keyComparer] (const KEY_TYPE& lhs, const KEY_TYPE& rhs) { return keyComparer (lhs, rhs) and keyComparer (rhs, lhs); }};
        }
        virtual shared_ptr<typename KeyedCollection<T, KEY_TYPE, TRAITS>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (this->fKeyExtractor_, this->fKeyComparer_); // keep extractor/comparer but lose data in clone
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
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto nextIResult = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, nextIResult)};
            }
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
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
            return KeyInOrderKeyComparerType{fKeyComparer_};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDSET<KEY_INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************ SortedKeyedCollection_stdset<KEY_TYPE,MAPPED_VALUE_TYPE> **********
     ********************************************************************************
     */
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_stdset{KeyExtractorType{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor, KEY_INORDER_COMPARER&& keyComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_INORDER_COMPARER>>> (keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer))}
    {
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS> and
                  not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>>)
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (ITERABLE_OF_ADDABLE&& src)
        : SortedKeyedCollection_stdset{KeyExtractorType{}, KEY_INORDER_COMPARER{}}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#endif
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer, ITERABLE_OF_ADDABLE&& src)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor,
                                                                                            KEY_INORDER_COMPARER&&  keyComparer,
                                                                                            ITERABLE_OF_ADDABLE&&   src)
        : SortedKeyedCollection_stdset{keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_stdset{KeyExtractorType{}, KEY_INORDER_COMPARER{}}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE, IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (KEY_INORDER_COMPARER&& keyComparer,
                                                                                            ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (IKeyedCollection_ExtractorCanBeDefaulted<T, KEY_TYPE, TRAITS>)
        : SortedKeyedCollection_stdset{KeyExtractorType{}, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::SortedKeyedCollection_stdset (const KeyExtractorType& keyExtractor,
                                                                                            KEY_INORDER_COMPARER&&  keyComparer,
                                                                                            ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedKeyedCollection_stdset{keyExtractor, forward<KEY_INORDER_COMPARER> (keyComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T, typename KEY_TYPE, typename TRAITS>
    inline void SortedKeyedCollection_stdset<T, KEY_TYPE, TRAITS>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedKeyedCollection_stdmap_inl_ */
