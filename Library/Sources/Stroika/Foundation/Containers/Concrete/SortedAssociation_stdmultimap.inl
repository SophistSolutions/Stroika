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
     ** SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_ ***
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    class SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::IImplRepBase_
        : public SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep {};

    /*
     ********************************************************************************
     ****** SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_ ********
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IInOrderComparer<KEY_TYPE>) KEY_INORDER_COMPARER>
    class SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::Rep_
        : public IImplRepBase_,
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
        Rep_ (STDMULTIMAP<KEY_INORDER_COMPARER>&& src)
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
            return KeyEqualsCompareFunctionType{Common::EqualsComparerAdapter<KEY_TYPE, KEY_INORDER_COMPARER>{fData_.key_comp ()}};
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
            auto                                                  i = fData_.find (key);
            vector<mapped_type>                                   result;
            if (i != fData_.end ()) {
                Assert (GetKeyEqualsComparer () (key, i->first));
                result.push_back (i->second);
                // the items in a multimap are all in order so we know the current i->key is not less
                Assert (not fData_.key_comp () (key, i->first));
                Assert (not fData_.key_comp () (i->first, key));
                for (++i; i != fData_.end () and not fData_.key_comp () (key, i->first); ++i) {
                    result.push_back (i->second);
                }
            }
            return Iterable<mapped_type>{move (result)};
        }
        virtual void Add (ArgByValueType<KEY_TYPE> key, ArgByValueType<MAPPED_VALUE_TYPE> newElt) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            (void)fData_.insert ({key, newElt});
            fChangeCounts_.PerformedChange ();
            fData_.Invariant ();
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
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

        // SortedAssociation<KEY_TYPE, MAPPED_VALUE_TYPE>::_IRep overrides
    public:
        virtual KeyInOrderKeyComparerType GetInOrderKeyComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return KeyInOrderKeyComparerType{fData_.key_comp ()};
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDMULTIMAP<KEY_INORDER_COMPARER>>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********* SortedAssociation_stdmultimap<KEY_TYPE,MAPPED_VALUE_TYPE> ************
     ********************************************************************************
     */
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap ()
        : SortedAssociation_stdmultimap{less<KEY_TYPE>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_INORDER_COMPARER>>> (forward<KEY_INORDER_COMPARER> (inorderComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (STDMULTIMAP<KEY_INORDER_COMPARER>&& src)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<KEY_INORDER_COMPARER>>> (move (src))}
    {
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (
        KEY_INORDER_COMPARER&& inorderComparer, const initializer_list<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>>& src)
        : SortedAssociation_stdmultimap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>>)
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation_stdmultimap{}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IIterableOf<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERABLE_OF_ADDABLE>
    inline SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer,
                                                                                                      ITERABLE_OF_ADDABLE&& src)
        : SortedAssociation_stdmultimap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        AssertRepValidType_ ();
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation_stdmultimap{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    template <IInOrderComparer<KEY_TYPE> KEY_INORDER_COMPARER, IInputIterator<KeyValuePair<KEY_TYPE, MAPPED_VALUE_TYPE>> ITERATOR_OF_ADDABLE>
    SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::SortedAssociation_stdmultimap (KEY_INORDER_COMPARER&& inorderComparer,
                                                                                               ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedAssociation_stdmultimap{forward<KEY_INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename KEY_TYPE, typename MAPPED_VALUE_TYPE>
    inline void SortedAssociation_stdmultimap<KEY_TYPE, MAPPED_VALUE_TYPE>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
