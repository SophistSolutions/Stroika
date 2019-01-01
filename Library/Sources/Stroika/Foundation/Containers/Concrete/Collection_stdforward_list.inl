/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2019.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include <forward_list>

#include "../../Memory/BlockAllocated.h"

#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

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
        using _IterableRepSharedPtr   = typename Iterable<T>::_IterableRepSharedPtr;
        using _CollectionRepSharedPtr = typename Collection<T>::_CollectionRepSharedPtr;
        using _APPLY_ARGTYPE          = typename inherited::_APPLY_ARGTYPE;
        using _APPLYUNTIL_ARGTYPE     = typename inherited::_APPLYUNTIL_ARGTYPE;

    public:
        Rep_ ()                 = default;
        Rep_ (const Rep_& from) = delete;
        Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
            : inherited ()
            , fData_ (&from->fData_, forIterableEnvelope)
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<T>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
            return Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
        }
        virtual Iterator<T> MakeIterator (IteratorOwnerID suggestedOwner) const override
        {
            Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
        }
        virtual size_t GetLength () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            size_t                                                     cnt = 0;
            for (auto i = fData_.begin (); i != fData_.end (); ++i, cnt++)
                ;
            return cnt;
        }
        virtual bool IsEmpty () const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return fData_.empty ();
        }
        virtual void Apply (_APPLY_ARGTYPE doToElement) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
        }
        virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            return this->_FindFirstThat (doToElement, suggestedOwner);
        }

        // Collection<T>::_IRep overrides
    public:
        virtual _CollectionRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            if (fData_.HasActiveIterators ()) {
                // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                auto r = Iterable<T>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                r->fData_.clear_WithPatching ();
                return r;
            }
            else {
                return Iterable<T>::template MakeSmartPtr<Rep_> ();
            }
        }
        virtual void Add (ArgByValueType<T> item) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.push_front (item);
        }
        virtual void Update (const Iterator<T>& i, ArgByValueType<T> newValue) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            Assert (not i.Done ());
            *mir.fIterator.fStdIterator = newValue;
        }
        virtual void Remove (const Iterator<T>& i) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            const typename Iterator<T>::IRep&                         ir = i.ConstGetRep ();
            AssertMember (&ir, IteratorRep_);
            auto& mir = dynamic_cast<const IteratorRep_&> (ir);
            //mir.fIterator_.RemoveCurrent ();

            // HORRIBLE BUT ADEQUITE IMPL...FOR NOW...
            {
                auto ei = fData_.before_begin ();
                for (auto ii = fData_.begin (); ii != fData_.end (); ++ii) {
                    if (ii == mir.fIterator.fStdIterator) {
                        Memory::SmallStackBuffer<typename DataStructureImplType_::ForwardIterator*> items2Patch (0);
                        fData_.TwoPhaseIteratorPatcherPass1 (ii, &items2Patch);
                        auto newI = fData_.erase_after (ei);
                        fData_.TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                        return;
                    }
                    ei = ii;
                }
            }
        }
#if qDebug
        virtual void AssertNoIteratorsReferenceOwner (IteratorOwnerID oBeingDeleted) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            fData_.AssertNoIteratorsReferenceOwner (oBeingDeleted);
        }
#endif

    private:
        using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<forward_list<T>>;
        using IteratorRep_           = Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************************* Collection_stdforward_list<T> ************************
     ********************************************************************************
     */
    template <typename T>
    inline Collection_stdforward_list<T>::Collection_stdforward_list ()
        : inherited (inherited::template MakeSmartPtr<Rep_> ())
    {
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (const T* start, const T* end)
        : Collection_stdforward_list ()
    {
        Require ((start == end) or (start != nullptr and end != nullptr));
        this->AddAll (start, end);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline Collection_stdforward_list<T>::Collection_stdforward_list (const Collection<T>& src)
        : Collection_stdforward_list ()
    {
        this->AddAll (src);
        AssertRepValidType_ ();
    }
    template <typename T>
    inline void Collection_stdforward_list<T>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMemeber
#endif
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_ */
