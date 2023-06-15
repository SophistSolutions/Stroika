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
        virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] Execution::SequencePolicy seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            fData_.Apply (doToElement);
        }
        virtual Iterator<value_type> Find (const shared_ptr<typename Iterable<T>::_IRep>&          thisSharedPtr,
                                           const function<bool (ArgByValueType<value_type> item)>& that,
                                           [[maybe_unused]] Execution::SequencePolicy              seq) const override
        {
            Debug::AssertExternallySynchronizedMutex::ReadContext declareContext{fData_};
            return this->inherited::Find (thisSharedPtr, that, seq); // @todo rewrite to use fData
        }

        // Collection<T>::_IRep overrides
    public:
        virtual shared_ptr<typename Collection<T>::_IRep> CloneEmpty () const override { return Memory::MakeSharedPtr<Rep_> (); }
        virtual shared_ptr<typename Collection<T>::_IRep> CloneAndPatchIterator (Iterator<value_type>* i) const override
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
            // Horrible API - must revisit/rethink. Maybe just a bad fit? But to erase an element from a forward_list,
            // given a link to it, you must walk from the start of the list and find its prev pointer
            typename STDFORWARDLIST::const_iterator victim =
                Debug::UncheckedDynamicCast<const IteratorRep_&> (i.ConstGetRep ()).fIterator.GetUnderlyingIteratorRep ();
            typename STDFORWARDLIST::const_iterator prevI;
            for (prevI = fData_.before_begin (); std::next (prevI) != victim; ++prevI)
                ;
            Assert (prevI != fData_.end ()); // must be able to find prevI (if
            auto nextStdI = fData_.erase_after (prevI);
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
        : inherited{Memory::MakeSharedPtr<Rep_> ()}
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    template <input_iterator ITERATOR_OF_ADDABLE>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (ITERATOR_OF_ADDABLE&& start, ITERATOR_OF_ADDABLE&& end)
        : Collection_stdforward_list{}
    {
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
#if !qCompilerAndStdLib_RequiresNotMatchInlineOutOfLineForTemplateClassBeingDefined_Buggy
    template <typename T>
    template <IIterableOfT<T> ITERABLE_OF_ADDABLE>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (ITERABLE_OF_ADDABLE&& src)
        requires (not is_base_of_v<Collection_stdforward_list<T>, decay_t<ITERABLE_OF_ADDABLE>>)
        : Collection_stdforward_list{}
    {
        this->AddAll (forward<ITERABLE_OF_ADDABLE> (src));
        AssertRepValidType_ ();
    }
#endif
    template <typename T>
    inline void Collection_stdforward_list<T>::AssertRepValidType_ () const
    {
        if constexpr (qDebug) {
            typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
        }
    }

}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_ */
