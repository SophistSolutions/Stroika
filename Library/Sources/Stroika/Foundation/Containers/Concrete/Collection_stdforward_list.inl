/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"

namespace Stroika::Foundation::Containers::Concrete {

    /*
     ********************************************************************************
     ******************* Collection_stdforward_list<T>::Rep_ ************************
     ********************************************************************************
     */
    template <typename T>
    class Collection_stdforward_list<T>::Rep_ : public Collection<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename Collection<T>::_IRep;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = default;

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
        }
        virtual Iterator<value_type> MakeIterator ([[maybe_unused]] const _IterableRepSharedPtr& thisSharedPtr) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_)};
        }
        virtual size_t size () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            size_t                                                cnt = 0;
            for (auto i = fData_.begin (); i != fData_.end (); ++i, ++cnt)
                ;
            return cnt;
        }
        virtual bool empty () const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return fData_.empty ();
        }
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const _IterableRepSharedPtr& thisSharedPtr, const function<bool (ArgByValueType<value_type> item)>& that) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->_Find (thisSharedPtr, that);
        }
        virtual Iterator<value_type> Find_equal_to (const _IterableRepSharedPtr& thisSharedPtr, const ArgByValueType<value_type>& v) const override
        {
            return this->_Find_equal_to_default_implementation (thisSharedPtr, v);
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty () const override { return Iterable<T>::template MakeSmartPtr<Rep_> (); }
        virtual _CollectionRepSharedPtr CloneAndPatchIterator (Iterator<value_type>* i) const override
        {
            RequireNotNull (i);
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            auto                                                  result = Iterable<value_type>::template MakeSmartPtr<Rep_> (*this);
            auto& mir = Debug::UncheckedDynamicCast<const IteratorRep_&> (i->ConstGetRep ());
            result->fData_.MoveIteratorHereAfterClone (&mir.fIterator, &fData_);
            i->Refresh (); // reflect updated rep
            return result;
        }
        virtual void Add (ArgByValueType<value_type> item) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            fData_.push_front (item);
            fChangeCounts_.PerformedChange ();
        }
        virtual void Update (const Iterator<value_type>& i, ArgByValueType<value_type> newValue, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            Require (not i.Done ());
            optional<typename DataStructureImplType_::UnderlyingIteratorRep> savedUnderlyingIndex;
            if (nextI != nullptr) {
                savedUnderlyingIndex = Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            }
            *fData_.remove_constness (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ()) = newValue;
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, *savedUnderlyingIndex)};
            }
        }
        virtual void Remove (const Iterator<value_type>& i, Iterator<value_type>* nextI) override
        {
            Debug::AssertExternallySynchronizedMutex::WriteContext declareContext{fData_};
            auto                                                   nextStdI =
                fData_.erase_after (Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ());
            fChangeCounts_.PerformedChange ();
            if (nextI != nullptr) {
                *nextI = Iterator<value_type>{make_unique<IteratorRep_> (&fData_, &fChangeCounts_, nextStdI)};
            }
        }

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<STDFORWARDLIST>;
        using IteratorRep_           = Private::IteratorImplHelper_<value_type, DataStructureImplType_>;

    private:
        DataStructureImplType_                                     fData_;
        [[no_unique_address]] Private::ContainerDebugChangeCounts_ fChangeCounts_;
    };

    /*
     ********************************************************************************
     ************************* Collection_stdforward_list<T> ************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_stdforward_list<T>::Collection_stdforward_list ()
        : inherited{inherited::template MakeSmartPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERATOR_OF_ADDABLE>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Collection_stdforward_list{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERATOR_OF_ADDABLE>>);
        this->AddAll (forward<ITERATOR_OF_ADDABLE> (start), forward<ITERATOR_OF_ADDABLE> (end));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (const initializer_list<value_type>& src)
        : Collection_stdforward_list{}
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    template <typename ITERABLE_OF_ADDABLE,
              enable_if_t<Configuration::IsIterable_v<ITERABLE_OF_ADDABLE> and not is_base_of_v<Collection_stdforward_list<T>, decay_t<ITERABLE_OF_ADDABLE>>>*>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (ITERABLE_OF_ADDABLE&& src)
        : Collection_stdforward_list{}
    {
        static_assert (IsAddable_v<ExtractValueType_t<ITERABLE_OF_ADDABLE>>);
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Collection_stdforward_list<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_ */
