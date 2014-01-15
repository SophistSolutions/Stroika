/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#ifndef _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_
#define _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_

#include    <map>

#include    "../../Memory/BlockAllocated.h"
#include    "../STL/Compare.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"

namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ******* SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ ***************
                 ********************************************************************************
                 */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                class   SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_ : public SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep {
                private:
                    typedef typename    SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>::_IRep  inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
#if     qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  Clone () const override
                    {
                        return Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                    }
#else
                    virtual typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep     Clone () const override;
#endif
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        MakeIterator () const override;
                    virtual size_t                                              GetLength () const override;
                    virtual bool                                                IsEmpty () const override;
                    virtual void                                                Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>        ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep overrides
                public:
                    virtual bool                Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const override;
                    virtual void                RemoveAll () override;
                    virtual Iterable<KEY_TYPE>  Keys () const override;
                    virtual bool                Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const override;
                    virtual void                Add (KEY_TYPE key, VALUE_TYPE newElt) override;
                    virtual void                Remove (KEY_TYPE key) override;
                    virtual void                Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i) override;

                private:
                    typedef Private::PatchingDataStructures::STLContainerWrapper <
                    map <KEY_TYPE, VALUE_TYPE, STL::less<KEY_TYPE, typename TRAITS::KeyWellOrderCompareFunctionType>>
                            >
                            DataStructureImplType_;

#if 1
                private:
#if     qCompilerAndStdLib_SharedPtrOfPrivateTypes_Buggy
                public:
#endif

                    // @todo - TRY AND DEBUG WHY below simple typedef didnt work????Private::IteratorImplHelper_ - if good reason - document - else fix

                    class  IteratorRep_ : public Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep {
                    private:
                        typedef typename    Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep   inherited;

                    public:
                        using   OwnerID = typename inherited::OwnerID;

                    public:
                        explicit IteratorRep_ (Private::ContainerRepLockDataSupport_* sharedLock, DataStructureImplType_* data)
                            : inherited ()
                            , fLockSupport (*sharedLock)
                            , fIterator (data)
                        {
                            RequireNotNull (sharedLock);
                            RequireNotNull (data);
                            fIterator.More (static_cast<pair<KEY_TYPE, VALUE_TYPE>*> (nullptr), true);   //tmphack cuz current backend iterators require a first more() - fix that!
                        }

                    public:
                        DECLARE_USE_BLOCK_ALLOCATION (IteratorRep_);

                        // Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep
                    public:
                        virtual typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr Clone () const override
                        {
                            CONTAINER_LOCK_HELPER_START (fLockSupport) {
                                return typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (*this));
                            }
                            CONTAINER_LOCK_HELPER_END ();
                        }
                        virtual OwnerID GetOwner () const override
                        {
                            //tmphack but adequate
                            // should NOT require locking is readonly immutable value provided at construction
                            return nullptr;
                        }
                        virtual void                                More (Memory::Optional<KeyValuePair<KEY_TYPE, VALUE_TYPE>>* result, bool advance) override
                        {
                            RequireNotNull (result);
                            CONTAINER_LOCK_HELPER_START (fLockSupport) {
                                if (result == nullptr) {
                                    fIterator.More (static_cast<pair<KEY_TYPE, VALUE_TYPE>*> (nullptr), advance);
                                }
                                else {
                                    Memory::Optional<pair<KEY_TYPE, VALUE_TYPE>>    tmp;
                                    fIterator.More (&tmp, advance);
                                    if (tmp.IsPresent ()) {
                                        *result = KeyValuePair<KEY_TYPE, VALUE_TYPE> (tmp->first, tmp->second);
                                    }
                                    else {
                                        result->clear ();
                                    }
                                }
                            }
                            CONTAINER_LOCK_HELPER_END ();
                        }
                        virtual bool                                Equals (const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep* rhs) const override
                        {
                            AssertNotImplemented ();
                            return false;
                        }

                    public:
                        Private::ContainerRepLockDataSupport_&           fLockSupport;
                        mutable typename DataStructureImplType_::ForwardIterator    fIterator;
                    };
#else
                private:
                    typedef typename Private::IteratorImplHelper_<KeyValuePair<KEY_TYPE, VALUE_TYPE>, DataStructureImplType_>    IteratorRep_;
#endif

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                ******************** SortedMapping_stdmap<Key,T>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    fData_.Invariant ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_.Invariant ();
                        from.fData_.Invariant ();
                        fData_ = from.fData_;
                        from.fData_.Invariant ();
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
#if     !qCompilerAndStdLib_IllUnderstoodTemplateConfusionOverT_Buggy
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::_SharedPtrIRep (new Rep_ (*this));
                }
#endif
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> (tmpRep);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                size_t  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.size ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void      SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>     SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Equals (const typename Mapping<KEY_TYPE, VALUE_TYPE, typename TRAITS::MappingTraitsType>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                Iterable<KEY_TYPE>    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Keys () const
                {
                    AssertNotImplemented ();
                    return *(Iterable<KEY_TYPE>*)nullptr;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                bool    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Lookup (KEY_TYPE key, Memory::Optional<VALUE_TYPE>* item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            if (item != nullptr) {
                                item->clear ();
                            }
                            return false;
                        }
                        else {
                            if (item != nullptr) {
                                *item = i->second;
                            }
                            return true;
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Add (KEY_TYPE key, VALUE_TYPE newElt)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i == fData_.end ()) {
                            i = fData_.insert (pair<KEY_TYPE, VALUE_TYPE> (key, newElt)).first;
                            // no need to patch map<>
                        }
                        else {
                            i->second = newElt;
                        }
                        fData_.Invariant ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (KEY_TYPE key)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.Invariant ();
                        auto i = fData_.find (key);
                        if (i != fData_.end ()) {
                            fData_.erase_WithPatching (i);
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                void    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_::Remove (Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>> i)
                {
                    const typename Iterator<KeyValuePair<KEY_TYPE, VALUE_TYPE>>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto    mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        mir.fIterator.RemoveCurrent ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ************************* SortedMapping_stdmap<Key,T> **************************
                ********************************************************************************
                */
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping_stdmap ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping_stdmap (const SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& src)
                    : inherited (static_cast<const SortedMapping<KEY_TYPE, VALUE_TYPE, TRAITS>&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                template    <typename CONTAINER_OF_PAIR_KEY_T>
                inline  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::SortedMapping_stdmap (const CONTAINER_OF_PAIR_KEY_T& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    this->AddAll (src);
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>&   SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::operator= (const SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  const typename SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_&  SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename KEY_TYPE, typename VALUE_TYPE, typename TRAITS>
                inline  typename SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::Rep_&    SortedMapping_stdmap<KEY_TYPE, VALUE_TYPE, TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_SortedMapping_stdmap_inl_ */
