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
     ********** SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ *************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (IInOrderComparer<KEY_TYPE>) KEY_INORDER_COMPARER>
    class SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ : public IImplRepBase_,
                                                                    public Memory::UseBlockAllocationIfAppropriate<Rep_<KEY_INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<KEY_INORDER_COMPARER>);

    public:
        Rep_ (const KEY_INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
        {
        }
        Rep_ (STDMAP<KEY_INORDER_COMPARER>&& src)
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

        // Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyEqualsCompareFunctionType GetKeyEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::EqualsComparerAdapter<KEY_TYPE, KEY_INORDER_COMPARER>{fData_.key_comp ()};
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data in clone
        }
        virtual shared_ptr<typename Mapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Lookup (ArgByValueType<KEY_TYPE> key, optional<MAPPED_VALUE_TYPE>* item) const override
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
                    *item = i->second;
                }
                return true;
            }
        }
        virtual bool Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt, AddReplaceMode addReplaceMode) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            bool result{};
            switch (addReplaceMode) {
                case AddReplaceMode::eAddReplaces:
                    result = fData_.insert_or_assign (key, newElt).second; // according to https://en.cppreference.com/w/cpp/container/map/insert_or_assign - no iterator references are invalidated
                    break;
                case AddReplaceMode::eAddIfMissing:
                    result = fData_.insert ({key, newElt}).second; // according to https://en.cppreference.com/w/cpp/container/map/insert no iterator references are invalidated
                    break;
                default:
                    AssertNotReached ();
            }
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
            return result;
        }
        virtual bool RemoveIf (ArgByValueType<KEY_TYPE> key) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.find (key);
            if (i != fData_.end ()) {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto newI = fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, newI)};
            }
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<mapped_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_
                .remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ())
                ->second = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

        // SortedMapping<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyThreeWayComparerType GetKeyThreeWayComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::ThreeWayComparerAdapter<KEY_TYPE, KEY_INORDER_COMPARER>{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDMAP<KEY_INORDER_COMPARER>>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************* SortedMapping_stdmap<KEY_TYPE,MAPPED_VALUE_TYPE> *****************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap ()
        requires (totally_ordered<KEY_TYPE>)
        : SortedMapping_stdmap{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_INORDER_COMPARER>>> (forward<KEY_INORDER_COMPARER> (inorderComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (STDMAP<KEY_INORDER_COMPARER>&& src)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_INORDER_COMPARER>>> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        requires (totally_ordered<KEY_TYPE>)
        : SortedMapping_stdmap{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer,
                                                                                    const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedMapping_stdmap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (ITERABLE_OF_ADDABLE&& src)
        requires (totally_ordered<KEY_TYPE>)
        : SortedMapping_stdmap{}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedMapping_stdmap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        requires (totally_ordered<KEY_TYPE>)
        : SortedMapping_stdmap{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedMapping_stdmap (KEY_INORDER_COMPARER&& inorderComparer,
                                                                             ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedMapping_stdmap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedMapping_stdmap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
