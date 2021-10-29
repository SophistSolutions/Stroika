/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Debug/Cast.h"
#include "../../Memory/BlockAllocated.h"

#include "../DataStructures/STLContainerWrapper.h"
#include "../Private/IteratorImplHelper.h"

namespace Stroika::Foundation::Containers::Concrete {

    using Traversal::IteratorOwnerID;

    /*
     ********************************************************************************
     *********** DenseDataHyperRectangle_Vector<T, INDEXES...>::Rep_ ****************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    class DenseDataHyperRectangle_Vector<T, INDEXES...>::Rep_ : public DenseDataHyperRectangle<T, INDEXES...>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
    private:
        using inherited = typename DenseDataHyperRectangle<T, INDEXES...>::_IRep;

    public:
        Rep_ (INDEXES... dimensions)
            : fDimensions_ (forward<INDEXES> (dimensions)...)
        {
            //  AssertNotImplemented ();
        }
        Rep_ (const Rep_& from) = delete;
        Rep_ (const Rep_* from, [[maybe_unused]] IteratorOwnerID forIterableEnvelope)
            : fData_{from->fData_}
        {
            RequireNotNull (from);
        }

    public:
        nonvirtual Rep_& operator= (const Rep_&) = delete;

        // Iterable<tuple<T, INDEXES...>>::_IRep overrides
    public:
        virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
        {
            return Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (this, forIterableEnvelope);
        }
        virtual Iterator<tuple<T, INDEXES...>> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
        {
/// NYI
#if 0
            return Iterator<value_type>{Iterator<value_type>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_)};
#endif
            using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
            return RESULT_TYPE::GetEmptyIterator ();
        }
        virtual size_t GetLength () const override
        {
            return fData_.size ();
        }
        virtual bool IsEmpty () const override
        {
            return fData_.empty ();
        }
        virtual void Apply ([[maybe_unused]] const function<void (ArgByValueType<value_type> item)>& doToElement) const override
        {
            AssertNotImplemented ();
#if 0
            // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
            // use iterator (which currently implies lots of locks) with this->_Apply ()
            fData_.Apply (doToElement);
#endif
        }
        virtual Iterator<tuple<T, INDEXES...>> FindFirstThat ([[maybe_unused]] const function<bool (ArgByValueType<value_type> item)>& doToElement, [[maybe_unused]] IteratorOwnerID suggestedOwner) const override
        {
            using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
#if 1
            /// NYI
            return RESULT_TYPE::GetEmptyIterator ();
#else
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            auto                                                       iLink = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
            if (iLink == fData_.end ()) {
                return RESULT_TYPE::GetEmptyIterator ();
            }
            Traversal::IteratorBase::PtrImplementationTemplate<IteratorRep_> resultRep = Iterator<T>::template MakeSmartPtr<IteratorRep_> (suggestedOwner, &fData_);
            resultRep->fIterator.SetCurrentLink (iLink);
            return RESULT_TYPE (move (resultRep));
#endif
        }

        // DataHyperRectangle<T, INDEXES...>::_IRep overrides
    public:
        virtual _DataHyperRectangleRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
        {
            // @todo - fix so using differnt CTOR - with no data to remove
            auto r = Iterable<tuple<T, INDEXES...>>::template MakeSmartPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
            r->fData_.clear (); //wrong - must checkjust zero out elts
            return r;
        }
        DISABLE_COMPILER_MSC_WARNING_START (4100)
        virtual T GetAt (INDEXES... indexes) const override
        {
            shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            /// NYI
            return T{};
        }
        virtual void SetAt ([[maybe_unused]] INDEXES... indexes, [[maybe_unused]] Configuration::ArgByValueType<T> v) override
        {
            lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
            /// NYI
            AssertNotImplemented ();
        }
        DISABLE_COMPILER_MSC_WARNING_END (4100)

    private:
        using DataStructureImplType_ = DataStructures::STLContainerWrapper<vector<T>>;
        using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

    private:
        tuple<INDEXES...>      fDimensions_;
        DataStructureImplType_ fData_;
    };

    /*
     ********************************************************************************
     ************** DenseDataHyperRectangle_Vector<T, INDEXES...> *******************
     ********************************************************************************
     */
    template <typename T, typename... INDEXES>
    DenseDataHyperRectangle_Vector<T, INDEXES...>::DenseDataHyperRectangle_Vector (INDEXES... dimensions)
        : inherited (inherited::template MakeSmartPtr<Rep_> (forward<INDEXES> (dimensions)...))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle_Vector<T, INDEXES...>::DenseDataHyperRectangle_Vector (const DenseDataHyperRectangle_Vector<T, INDEXES...>& src)
        : inherited (static_cast<const inherited&> (src))
    {
        AssertRepValidType_ ();
    }
    template <typename T, typename... INDEXES>
    inline DenseDataHyperRectangle_Vector<T, INDEXES...>& DenseDataHyperRectangle_Vector<T, INDEXES...>::operator= (const DenseDataHyperRectangle_Vector<T, INDEXES...>& rhs)
    {
        AssertRepValidType_ ();
        inherited::operator= (static_cast<const inherited&> (rhs));
        AssertRepValidType_ ();
        return *this;
    }
    template <typename T, typename... INDEXES>
    inline void DenseDataHyperRectangle_Vector<T, INDEXES...>::AssertRepValidType_ () const
    {
#if qDebug
        typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
#endif
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_ */
