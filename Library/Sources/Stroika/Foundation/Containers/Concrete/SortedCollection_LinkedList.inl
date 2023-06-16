/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/LinkedList.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ****************** SortedCollection_LinkedList<T>::IImplRepBase_ ***************
     ********************************************************************************
     */
    template <typename T>
    class SortedCollection_LinkedList<T>::IImplRepBase_ : public SortedCollection<T>::_IRep {};

    /*
     ********************************************************************************
     *********************** SortedCollection_LinkedList<T>::Rep_ *******************
     ********************************************************************************
     */
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    class SortedCollection_LinkedList<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<INORDER_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        static_assert (not is_reference_v<INORDER_COMPARER>);

    public:
        Rep_ (const INORDER_COMPARER& inorderComparer)
            : fInorderComparer_{inorderComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

    private:
        [[no_unique_address]] const INORDER_COMPARER fInorderComparer_;

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
            if (auto iLink = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, iLink)};
            }
            return nullptr;
        }

        // Collection<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Collection<T>::_IRep> CloneEmpty () const override
        {
            return Memory::MakeSharedPtr<Rep_> (fInorderComparer_); // keep comparer, but lose data in clone
        }
        virtual shared_ptr<typename Collection<T>::_IRep> CloneAndPatchIterator (Iterator<T>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Memory::MakeSharedPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            AddWithoutLocks_ (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext           declareWriteContext{fData_};
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            // equals might examine a subset of the object and we still want to update the whole object, but
            // if its not already equal, the sort order could have changed so we must simulate with a remove/add
            if (Common::EqualsComparerAdapter{fInorderComparer_}(mir.fIterator.Current (), newValue)) {
                fData_.SetAt (mir.fIterator, newValue);
            }
            else {
                fData_.RemoveAt (mir.fIterator);
                AddWithoutLocks_ (newValue);
            }
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
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

        // SortedCollection<T>::_IRep overrides
    public:
        virtual InOrderComparerType GetInOrderComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return InOrderComparerType{fInorderComparer_};
        }
        virtual bool Equals ([[maybe_unused]] const typename Collection<T>::_IRep& rhs) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            AssertNotImplemented ();
            return false;
            //return this->_Equals_Reference_Implementation (rhs);
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.Lookup (item, Common::EqualsComparerAdapter{fInorderComparer_}) != nullptr;
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Remove (item, Common::EqualsComparerAdapter{fInorderComparer_});
            fChangeCounts_.PerformedChange ();
        }

    private:
        nonvirtual void AddWithoutLocks_ (ArgByValueType<value_type> item)
        {
            typename Rep_::DataStructureImplType_::ForwardIterator it{&fData_};
            // skip the smaller items
            for (; not it.Done () and fInorderComparer_ (it.Current (), item); ++it)
                ;
            // at this point - we are pointing at the first link >= item, so insert before it
            fData_.AddBefore (it, item);
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ********************* SortedCollection_LinkedList<T> ***************************
     ********************************************************************************
     */
    template <typename T>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList ()
        : SortedCollection_LinkedList{less<T>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (INORDER_COMPARER&& inorderComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<INORDER_COMPARER>>> (inorderComparer)}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (const initializer_list<T>& src)
        : SortedCollection_LinkedList{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, const initializer_list<T>& src)
        : SortedCollection_LinkedList{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterable<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection_LinkedList<T>>)
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src)
        : SortedCollection_LinkedList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterable<T> ITERABLE_OF_ADDABLE>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERABLE_OF_ADDABLE&& src)
        : SortedCollection_LinkedList{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : SortedCollection_LinkedList{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (INORDER_COMPARER&& inOrderComparer, ITERATOR_OF_ADDABLE&& start,
                                                                        ITERATOR_OF_ADDABLE&& end)
        : SortedCollection_LinkedList{forward<INORDER_COMPARER> (inOrderComparer)}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void SortedCollection_LinkedList<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedCollection_LinkedList_inl_ */
