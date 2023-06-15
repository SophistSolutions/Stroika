/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Set_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Set_Array_inl_

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
     ********************************************************************************
     ********************* MultiSet_Array<T, TRAITS>::IImplRepBase_ *****************
     ********************************************************************************
     */
    template <typename T>
    class Set_Array<T>::IImplRepBase_ : public Set<T>::_IRep {
    public:
        virtual size_t capacity () const             = 0;
        virtual void   reserve (size_t slotsAlloced) = 0;
        virtual void   shrink_to_fit ()              = 0;
    };

    /**
     */
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    class Set_Array<T>::Rep_ : public IImplRepBase_, public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = IImplRepBase_;

    public:
        Rep_ (const EQUALS_COMPARER& equalsComparer)
            : fEqualsComparer_{equalsComparer}
        {
        }
        Rep_ (const Rep_& from) = default;

    private:
        [[no_unique_address]] EQUALS_COMPARER fEqualsComparer_;

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
            if (optional<size_t> i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *i)};
            }
            return nullptr;
        }

        // Set<T>::_IRep overrides
    public:
        virtual ElementEqualityComparerType GetElementEqualsComparer () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return ElementEqualityComparerType{fEqualsComparer_};
        }
        virtual shared_ptr<typename Set<T>::_IRep> CloneEmpty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Memory::MakeSharedPtr<Rep_> (fEqualsComparer_); // copy comparers, but not data
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
            Debug::AssertExternallySynchronizedMutex::ReadContext readLock{fData_};
            if (optional<size_t> l = fData_.Find ([this, item] (ArgByValueType<value_type> i) { return fEqualsComparer_ (i, item); })) {
                if (oResult != nullptr) {
                    *oResult = fData_[*l];
                }
                if (iResult != nullptr) {
                    *iResult = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *l)};
                }
                return true;
            }
            return false;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    return;
                }
            }
            fData_.push_back (item); // order meaningless for set, append cheapest way for array
            fChangeCounts_.PerformedChange ();
        }
        virtual bool RemoveIf (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fEqualsComparer_ (it.Current (), item)) {
                    fData_.RemoveAt (it);
                    fChangeCounts_.PerformedChange ();
                    return true;
                }
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            optional<size_t> savedUnderlyingIndex;
            static_assert (is_same_v<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            fData_.RemoveAt (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }

        // Set_Array<T>::_IRep overrides
    public:
        virtual size_t capacity () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.capacity ();
        }
        virtual void reserve (size_t slotsAlloced) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.reserve (slotsAlloced);
            fChangeCounts_.PerformedChange ();
        }
        virtual void shrink_to_fit () override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.shrink_to_fit ();
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
     ******************************** Set_Array<T> **********************************
     ********************************************************************************
     */
    template <typename T>
    inline Set_Array<T>::Set_Array ()
        : Set_Array{equal_to<value_type>{}}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline Set_Array<T>::Set_Array (EQUALS_COMPARER&& equalsComparer)
        : inherited{Memory::MakeSharedPtr<Rep_<remove_cvref_t<EQUALS_COMPARER>>> (forward<EQUALS_COMPARER> (equalsComparer))}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Set_Array<T>::Set_Array (const initializer_list<value_type>& src)
        : Set_Array{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER>
    inline Set_Array<T>::Set_Array (EQUALS_COMPARER&& equalsComparer, const initializer_list<value_type>& src)
        : Set_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IIterable<T> ITERABLE_OF_ADDABLE, enable_if_t<not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Set_Array<T>>>*>
    inline Set_Array<T>::Set_Array (ITERABLE_OF_ADDABLE&& src)
        : Set_Array{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER, IIterable<T> ITERABLE_OF_ADDABLE>
    inline Set_Array<T>::Set_Array (EQUALS_COMPARER&& equalsComparer, ITERABLE_OF_ADDABLE&& src)
        : Set_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Set_Array<T>::Set_Array (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set_Array{}
    {
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER, IInputIterator<T> ITERATOR_OF_ADDABLE>
    inline Set_Array<T>::Set_Array (EQUALS_COMPARER&& equalsComparer, ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Set_Array{forward<EQUALS_COMPARER> (equalsComparer)}
    {
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline size_t Set_Array<T>::capacity () const
    {
        using _SafeReadRepAccessor = typename inherited::template _SafeReadRepAccessor<IImplRepBase_>;
        _SafeReadRepAccessor accessor{this};
        return accessor._ConstGetRep ().capacity ();
    }
    template <typename T>
    inline void Set_Array<T>::reserve (size_t slotsAlloced)
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().reserve (slotsAlloced);
    }
    template <typename T>
    inline void Set_Array<T>::shrink_to_fit ()
    {
        using _SafeReadWriteRepAccessor = typename inherited::template _SafeReadWriteRepAccessor<IImplRepBase_>;
        _SafeReadWriteRepAccessor accessor{this};
        accessor._GetWriteableRep ().shrink_to_fit ();
    }
    template <typename T>
    inline void Set_Array<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Set_Array_inl_ */
