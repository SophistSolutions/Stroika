/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_
#define _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "../../Memory/BlockAllocated.h"

#include "../Private/IteratorImplHelper.h"
#include "../Private/PatchingDataStructures/STLContainerWrapper.h"

namespace Stroika {
    namespace Foundation {
        namespace Containers {
            namespace Concrete {

                using Traversal::IteratorOwnerID;

                /*
                 ********************************************************************************
                 ******** DenseDataHyperRectangle_Vector<T, INDEXES...>::Rep_ *******************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                class DenseDataHyperRectangle_Vector<T, INDEXES...>::Rep_ : public DenseDataHyperRectangle<T, INDEXES...>::_IRep, public Memory::UseBlockAllocationIfAppropriate<Rep_> {
                private:
                    using inherited = typename DenseDataHyperRectangle<T, INDEXES...>::_IRep;

                public:
                    using _IterableRepSharedPtr           = typename Iterable<tuple<T, INDEXES...>>::_IterableRepSharedPtr;
                    using _DataHyperRectangleRepSharedPtr = typename inherited::_DataHyperRectangleRepSharedPtr;
                    using _APPLY_ARGTYPE                  = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE             = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ (INDEXES... dimensions)
                        : fDimensions_ (forward<INDEXES> (dimensions)...)
                    {
                        //  AssertNotImplemented ();
                    }
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                        : inherited ()
                        , fData_ (&from->fData_, forIterableEnvelope)
                    {
                        RequireNotNull (from);
                    }

                public:
                    nonvirtual Rep_& operator= (const Rep_&) = delete;

                    // Iterable<tuple<T, INDEXES...>>::_IRep overrides
                public:
                    virtual _IterableRepSharedPtr Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<tuple<T, INDEXES...>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<tuple<T, INDEXES...>> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                    {
/// NYI
#if 0
                        Rep_* NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        return Iterator<tuple<T, INDEXES...>> (Iterator<tuple<T, INDEXES...>>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_));
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
                    virtual void Apply ([[maybe_unused]] _APPLY_ARGTYPE doToElement) const override
                    {
                        AssertNotImplemented ();
#if 0
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
#endif
                    }
                    virtual Iterator<tuple<T, INDEXES...>> FindFirstThat ([[maybe_unused]] _APPLYUNTIL_ARGTYPE doToElement, [[maybe_unused]] IteratorOwnerID suggestedOwner) const override
                    {
                        using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
#if 1
                        /// NYI
                        return RESULT_TYPE::GetEmptyIterator ();
#else
                        shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
                        if (iLink == fData_.end ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (resultRep);
#endif
                    }

                    // DataHyperRectangle<T, INDEXES...>::_IRep overrides
                public:
                    virtual _DataHyperRectangleRepSharedPtr CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
                    {
                        if (fData_.HasActiveIterators ()) {
                            // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                            auto r = Iterable<tuple<T, INDEXES...>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                            r->fData_.clear (); //wrong - must checkjust zero out elts
                            return r;
                        }
                        else {
                            AssertNotReached ();
                            return nullptr;
                            ///return Iterable<T>::template MakeSharedPtr<Rep_> (forward<INDEXES> (fDimensions_)...);
                        }
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
                    using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<vector<T>>;
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
                    : inherited (inherited::template MakeSharedPtr<Rep_> (forward<INDEXES> (dimensions)...))
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
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_DenseDataHyperRectangle_Vector_inl_ */
