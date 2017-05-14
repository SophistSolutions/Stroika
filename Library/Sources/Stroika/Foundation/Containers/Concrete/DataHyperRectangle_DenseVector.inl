/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2017.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_inl_
#define _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_inl_

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
                 ******** DataHyperRectangle_DenseVector<T, INDEXES...>::Rep_ *******************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                class DataHyperRectangle_DenseVector<T, INDEXES...>::Rep_ : public DataHyperRectangle<T, INDEXES...>::_IRep {
                private:
                    using inherited = typename DataHyperRectangle<T, INDEXES...>::_IRep;

                public:
                    using _IterableSharedPtrIRep = typename Iterable<tuple<T, INDEXES...>>::_SharedPtrIRep;
                    using _SharedPtrIRep         = typename inherited::_SharedPtrIRep;
                    using _APPLY_ARGTYPE         = typename inherited::_APPLY_ARGTYPE;
                    using _APPLYUNTIL_ARGTYPE    = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ (INDEXES... dimensions)
                        : fDimensions_ (std::forward<INDEXES> (dimensions)...)
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

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<tuple<T, INDEXES...>>::_IRep overrides
                public:
                    virtual _IterableSharedPtrIRep Clone (IteratorOwnerID forIterableEnvelope) const override
                    {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return Iterable<tuple<T, INDEXES...>>::template MakeSharedPtr<Rep_> (const_cast<Rep_*> (this), forIterableEnvelope);
                    }
                    virtual Iterator<tuple<T, INDEXES...>> MakeIterator (IteratorOwnerID suggestedOwner) const override
                    {
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
                    virtual void Apply (_APPLY_ARGTYPE doToElement) const override
                    {
#if 0
                        // empirically faster (vs2k13) to lock once and apply (even calling stdfunc) than to
                        // use iterator (which currently implies lots of locks) with this->_Apply ()
                        fData_.Apply (doToElement);
#endif
                    }
                    virtual Iterator<tuple<T, INDEXES...>> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override
                    {
                        using RESULT_TYPE = Iterator<tuple<T, INDEXES...>>;
#if 1
                        return RESULT_TYPE::GetEmptyIterator ();
#else
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        using SHARED_REP_TYPE = Traversal::IteratorBase::SharedPtrImplementationTemplate<IteratorRep_>;
                        auto iLink            = const_cast<DataStructureImplType_&> (fData_).FindFirstThat (doToElement);
                        if (iLink == fData_.end ()) {
                            return RESULT_TYPE::GetEmptyIterator ();
                        }
                        Rep_*           NON_CONST_THIS = const_cast<Rep_*> (this); // logically const, but non-const cast cuz re-using iterator API
                        SHARED_REP_TYPE resultRep      = Iterator<T>::template MakeSharedPtr<IteratorRep_> (suggestedOwner, &NON_CONST_THIS->fData_);
                        resultRep->fIterator.SetCurrentLink (iLink);
                        // because Iterator<T> locks rep (non recursive mutex) - this CTOR needs to happen outside CONTAINER_LOCK_HELPER_START()
                        return RESULT_TYPE (typename RESULT_TYPE::SharedIRepPtr (resultRep));
#endif
                    }

                    // DataHyperRectangle<T, INDEXES...>::_IRep overrides
                public:
                    virtual _SharedPtrIRep CloneEmpty (IteratorOwnerID forIterableEnvelope) const override
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
                            ///return Iterable<T>::template MakeSharedPtr<Rep_> (std::forward<INDEXES> (fDimensions_)...);
                        }
                    }
                    virtual T GetAt (INDEXES... indexes) const override
                    {
                        std::shared_lock<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                        return T{};
                    }
                    virtual void SetAt (INDEXES... indexes, Configuration::ArgByValueType<T> v) override
                    {
                        std::lock_guard<const Debug::AssertExternallySynchronizedLock> critSec{fData_};
                    }

                private:
                    using DataStructureImplType_ = Private::PatchingDataStructures::STLContainerWrapper<vector<T>>;
                    using IteratorRep_           = typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    tuple<INDEXES...>      fDimensions_;
                    DataStructureImplType_ fData_;
                };

                /*
                 ********************************************************************************
                 ************** DataHyperRectangle_DenseVector<T, INDEXES...> *******************
                 ********************************************************************************
                 */
                template <typename T, typename... INDEXES>
                DataHyperRectangle_DenseVector<T, INDEXES...>::DataHyperRectangle_DenseVector (INDEXES... dimensions)
                    : inherited (inherited::template MakeSharedPtr<Rep_> (std::forward<INDEXES> (dimensions)...))
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename... INDEXES>
                inline DataHyperRectangle_DenseVector<T, INDEXES...>::DataHyperRectangle_DenseVector (const DataHyperRectangle_DenseVector<T, INDEXES...>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertRepValidType_ ();
                }
                template <typename T, typename... INDEXES>
                inline DataHyperRectangle_DenseVector<T, INDEXES...>& DataHyperRectangle_DenseVector<T, INDEXES...>::operator= (const DataHyperRectangle_DenseVector<T, INDEXES...>& rhs)
                {
                    AssertRepValidType_ ();
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertRepValidType_ ();
                    return *this;
                }
                template <typename T, typename... INDEXES>
                inline void DataHyperRectangle_DenseVector<T, INDEXES...>::AssertRepValidType_ () const
                {
#if qDebug
                    typename inherited::template _SafeReadRepAccessor<Rep_> tmp{this}; // for side-effect of AssertMember
#endif
                }
            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_DataHyperRectangle_DenseVector_inl_ */
