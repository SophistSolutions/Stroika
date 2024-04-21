/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "Stroika/Foundation/Common/Compare.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /**
     */
    template <typename T>
    class Set_stdset<T>::IImplRepBase_ : public Set<T>::_IRep {};

    /**
     */
    template <typename T>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (Common::IInOrderComparer<T>) INORDER_COMPARER>
    class Set_stdset<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<INORDER_COMPARER>);

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fData_{inorderComparer}
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
        virtual Iterator<value_type> Find_equal_to (const ArgByValueType<value_type>& v, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            // if doing a find by 'equals-to' - we already have this indexed
            auto found = fData_.find (v);
            Ensure ((found == fData_.end () and this->inherited::Find_equal_to (v, seq) == Iterator<value_type>{nullptr}) or
                    (found == Debug::UncheckedDynamicCast<const IteratorRep_&> (this->inherited::Find_equal_to (v, seq).ConstGetRep ())
                                  .fIterator.GetUnderlyingIteratorRep ()));
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, found)};
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementEqualityComparerType{Common::EqualsComparerAdapter{fData_.key_comp ()}};
        }
        virtual shared_ptr<typename Set<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fData_.key_comp ()); // keep comparer, but lose data
        }
        virtual shared_ptr<typename Set<T>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual bool Equals (const typename Iterable<value_type>::_IRep& rhs) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Lookup (ArgByValueType<value_type> item, optional<value_type>* oResult, Iterator<value_type>* iResult) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  i       = fData_.find (item);
            bool                                                  notDone = i != fData_.end ();
            if (oResult != nullptr and notDone) {
                *oResult = *i;
            }
            if (iResult != nullptr and notDone) {
                *iResult = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, i)};
            }
            return notDone;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.insert (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual bool RemoveIf (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Invariant ();
            auto i = fData_.find (item);
            if (i != fData_.end ()) [[likely]] {
                fData_.erase (i);
                fChangeCounts_.PerformedChange ();
                return true;
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Require (not i.Done ());
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            Assert (mir.fIterator.GetReferredToData () == &fData_);
            auto nextIR = fData_.erase (mir.fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, nextIR)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDSET<INORDER_COMPARER>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********************************** Set_stdset<T> *******************************
     ********************************************************************************
     */
    template <typename T>
    inline Set_stdset<T>::Set_stdset ()
        : Set_stdset{less<value_type>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline Set_stdset<T>::Set_stdset (INORDER_COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<INORDER_COMPARER>>> (forward<INORDER_COMPARER> (inorderComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_stdset<T>::Set_stdset (const initializer_list<value_type>& src)
        : Set_stdset{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline Set_stdset<T>::Set_stdset (INORDER_COMPARER&& inorderComparer, const initializer_list<value_type>& src)
        : Set_stdset{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Set_stdset<T>>)
    inline Set_stdset<T>::Set_stdset (ITERABLE_OF_ADDABLE&& src)
        : Set_stdset{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
    inline Set_stdset<T>::Set_stdset (INORDER_COMPARER&& inorderComparer, ITERABLE_OF_ADDABLE&& src)
        : Set_stdset{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Set_stdset<T>::Set_stdset (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set_stdset{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Set_stdset<T>::Set_stdset (INORDER_COMPARER&& inorderComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set_stdset{forward<INORDER_COMPARER> (inorderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Set_stdset<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_Set_stdset_inl_ */
