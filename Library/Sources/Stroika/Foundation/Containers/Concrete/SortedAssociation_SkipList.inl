/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/STLContainerWrapper.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ****** SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ***********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IThreeWayComparer<KEY_TYPE>) KEY_COMPARER>
    class SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
        : public Private::SkipListBasedContainerRepImpl<Rep_<KEY_COMPARER>, IImplRepBase_>,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_COMPARER>> {
    private:
        using inherited = Private::SkipListBasedContainerRepImpl<Rep_<KEY_COMPARER>, IImplRepBase_>;

    public:
        static_assert (not is_reference_v<KEY_COMPARER>);

    public:
        Rep_ (const KEY_COMPARER& comparer)
            : fData_{comparer}
        {
        }
        Rep_ (SKIPLIST<KEY_COMPARER>&& src)
            : fData_{move (src)}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep overrides
    public:
        virtual shared_ptr<typename Iterable<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>::_IRep> Clone () const override
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
            fData_.Invariant ();
            return fData_.size ();
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Invariant ();
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const function<bool (ArgByValueType<value_type> item)>& that, Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (that, seq); // @todo rewrite to use fData
        }

        // Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::EqualsComparerAdapter<KEY_TYPE, KEY_COMPARER>{fData_.key_comp ()};
        }
        virtual shared_ptr<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual shared_ptr<typename Association<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual Iterable<mapped_type> Lookup (ArgByValueType<KEY_TYPE> key) const override
        {
            // @todo consider doing custom class here, or using CreateGenerator
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i = fData_.Find (key); // returns iterator to first matching key
            vector<mapped_type>                                   result;
            if (i != fData_.end ()) {
                Assert (GetKeyEqualsComparer () (key, i->fKey));
                result.push_back (i->fValue);
                // the items in a multimap are all in order so we know the current i->key is not less
                Assert (fData_.key_comp () (key, i->fKey) == strong_ordering::equal);
                for (++i; i != fData_.end () and fData_.key_comp () (key, i->fKey) == strong_ordering::equal; ++i) {
                    result.push_back (i->fValue);
                }
            }
            return Iterable<mapped_type>{move (result)};
        }
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            (void)fData_.Add (key, newElt);
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.Find (key);
            if (i != fData_.end ()) {
                fData_.Remove (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto newI = fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, newI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareWriteContext{fData_};
            optional<Iterator<value_type>>                         savedNextI;
            if (nextI != nullptr) {
                *nextI = i;
            }
            fData_.Update (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator, newValue);
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                savedNextI->Refresh ();
            }
        }

        // SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::InOrderComparerAdapter<KEY_TYPE, KEY_COMPARER>{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = SKIPLIST<KEY_COMPARER>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited;
    };

    /*
     ********************************************************************************
     ********* SortedAssociation_SkipList<KEY_TYPE,MAPPED_VALUE_TYPE> ***************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList ()
        : SortedAssociation_SkipList{compare_three_way{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (KEY_COMPARER&& comparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_COMPARER>>> (forward<KEY_COMPARER> (comparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (SKIPLIST<KEY_COMPARER>&& src)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_COMPARER>>> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER>
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (
        KEY_COMPARER&& comparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedAssociation_SkipList{forward<KEY_COMPARER> (comparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation_SkipList{}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (KEY_COMPARER&& comparer, ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation_SkipList{forward<KEY_COMPARER> (comparer)}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation_SkipList{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IThreeWayComparer<KEY_TYPE> KEY_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_SkipList (KEY_COMPARER&& comparer, ITERATOR_OF_ADDABLE&& start,
                                                                                         ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation_SkipList{forward<KEY_COMPARER> (comparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedAssociation_SkipList<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
