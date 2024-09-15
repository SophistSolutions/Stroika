/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/LinkedList.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********************** SortedCollection_LinkedList<T>::Rep_ *******************
     ********************************************************************************
     */
    template <typename T>
    template <qCompilerAndStdLib_ConstraintDiffersInTemplateRedeclaration_BWA (Common::IInOrderComparer<T>) INORDER_COMPARER>
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
        virtual void Add (ArgByValueType<value_type> item, Iterator<value_type>* oAddedI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   addedI = Add_ (item);
            fChangeCounts_.PerformedChange ();
            if (oAddedI != nullptr) [[unlikely]] {
                *oAddedI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, addedI)};
            }
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
            if (Common::EqualsComparerAdapter<value_type, INORDER_COMPARER>{fInorderComparer_}(*mir.fIterator, newValue)) {
                fData_.SetAt (mir.fIterator, newValue);
            }
            else {
                fData_.Remove (mir.fIterator);
                Add_ (newValue);
            }
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ());
            if (nextI == nullptr) {
                fData_.Remove (mir.fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                auto ret = fData_.erase (mir.fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, ret)};
            }
        }

        // SortedCollection<T>::_IRep overrides
    public:
        virtual ElementThreeWayComparerType GetElementThreeWayComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Common::ThreeWayComparerAdapter<T, INORDER_COMPARER>{fInorderComparer_};
        }
        virtual bool Contains (ArgByValueType<value_type> item) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.Find (item, Common::EqualsComparerAdapter<value_type, INORDER_COMPARER>{fInorderComparer_}) != nullptr;
        }
        virtual void Remove (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.Remove (item, Common::EqualsComparerAdapter<value_type, INORDER_COMPARER>{fInorderComparer_});
            fChangeCounts_.PerformedChange ();
        }

    private:
        using DataStructureImplType_ = DataStructures::LinkedList<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        nonvirtual auto Add_ (ArgByValueType<value_type> item) -> typename DataStructureImplType_::ForwardIterator
        {
            using ForwardIterator = typename DataStructureImplType_::ForwardIterator;
            ForwardIterator it{&fData_};
            // skip the smaller items
            for (; not it.Done () and fInorderComparer_ (*it, item); ++it)
                ;
            // at this point - we are pointing at the first link >= item, so insert before it
            ForwardIterator addedAt;
            fData_.AddBefore (it, item, &addedAt);
            return addedAt;
        }

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     *********************** SortedCollection_LinkedList<T> *************************
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
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, SortedCollection_LinkedList<T>>)
    inline SortedCollection_LinkedList<T>::SortedCollection_LinkedList (ITERABLE_OF_ADDABLE&& src)
        : SortedCollection_LinkedList{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <Common::IInOrderComparer<T> INORDER_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
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
