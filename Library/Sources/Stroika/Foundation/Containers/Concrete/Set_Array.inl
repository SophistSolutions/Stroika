/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "Stroika/Foundation/Containers/DataStructures/Array.h"
#include "Stroika/Foundation/Containers/Private/ArraySupport.h"
#include "Stroika/Foundation/Containers/Private/IteratorImplHelper.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     *********************** MultiSet_Array<T, TRAITS>::Rep_ ************************
     ********************************************************************************
     */
    template <typename T>
    template <BWA_Helper_ContraintInMemberClassSeparateDeclare_ (IEqualsComparer<T>) EQUALS_COMPARER>
    class Set_Array<T>::Rep_
        : public Private::ArrayBasedContainerRepImpl<Set_Array<T>::Rep_<EQUALS_COMPARER>, typename Set_Array<T>::IImplRepBase_>,
          public Memory::UseBlockAllocationIfAppropriate<Rep_<EQUALS_COMPARER>> {
    private:
        using inherited = Private::ArrayBasedContainerRepImpl<Set_Array<T>::Rep_<EQUALS_COMPARER>, typename Set_Array<T>::IImplRepBase_>;

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
            if (auto i = fData_.Find (that)) {
                return Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, i)};
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
            if (auto l = fData_.Find ([this, item] (ArgByValueType<value_type> i) { return fEqualsComparer_ (i, item); })) {
                if (oResult != nullptr) {
                    *oResult = *l;
                }
                if (iResult != nullptr) {
                    *iResult = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, l)};
                }
                return true;
            }
            return false;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            for (typename DataStructureImplType_::ForwardIterator it{&fData_}; not it.Done (); ++it) {
                if (fEqualsComparer_ (*it, item)) {
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
                if (fEqualsComparer_ (*it, item)) {
                    fData_.Remove (it);
                    fChangeCounts_.PerformedChange ();
                    return true;
                }
            }
            return false;
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            static_assert (same_as<size_t, typename DataStructureImplType_::UnderlyingIteratorRep>); // else must do slightly differently
            if (nextI == nullptr) {
                fData_.Remove (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
                fChangeCounts_.PerformedChange ();
            }
            else {
                auto retI = fData_.erase (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator);
                fChangeCounts_.PerformedChange ();
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fChangeCounts_, retI)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::Array<value_type>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;

    private:
        friend inherited;
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
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOf<T> ITERABLE_OF_ADDABLE>
        requires (not derived_from<remove_cvref_t<ITERABLE_OF_ADDABLE>, Set_Array<T>>)
    inline Set_Array<T>::Set_Array (ITERABLE_OF_ADDABLE&& src)
        : Set_Array{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    template <IEqualsComparer<T> EQUALS_COMPARER, IIterableOf<T> ITERABLE_OF_ADDABLE>
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
    inline void Set_Array<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<IImplRepBase_> tmp{this}; // for side-effect of AssertMember
        }
    }

}
