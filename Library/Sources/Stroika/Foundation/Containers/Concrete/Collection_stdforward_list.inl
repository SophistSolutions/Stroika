/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_
#define _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    <forward_list>

#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/STLContainerWrapper.h"
#include    "../Private/SynchronizationUtils.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 ********************* Collection_stdforward_list<T>::Rep_ **********************
                 ********************************************************************************
                 */
                template    <typename T>
                class   Collection_stdforward_list<T>::Rep_ : public Collection<T>::_IRep {
                private:
                    using   inherited   =   typename    Collection<T>::_IRep;

                public:
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;
                    using   _IteratorOwnerID = typename inherited::_IteratorOwnerID;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator (_IteratorOwnerID suggestedOwner) const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Collection<T>::_IRep overrides
                public:
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;

                private:
                    typedef Private::PatchingDataStructures::STLContainerWrapper<std::forward_list<T>>      DataStructureImplType_;
                    typedef Private::IteratorImplHelper_<T, DataStructureImplType_>                         IteratorRep_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;
                };


                /*
                ********************************************************************************
                ********************** Collection_stdforward_list<T>::Rep_ *********************
                ********************************************************************************
                */
                template    <typename T>
                inline  Collection_stdforward_list<T>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>::Rep_::Rep_ (const Rep_& from)
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                typename Iterable<T>::_SharedPtrIRep  Collection_stdforward_list<T>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T>
                Iterator<T>  Collection_stdforward_list<T>::Rep_::MakeIterator (_IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T>
                size_t  Collection_stdforward_list<T>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        size_t  cnt = 0;
                        for (auto i = fData_.begin (); i != fData_.end (); ++i, cnt++)
                            ;
                        return cnt;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                bool  Collection_stdforward_list<T>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.empty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void      Collection_stdforward_list<T>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T>
                Iterator<T>     Collection_stdforward_list<T>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T>
                void    Collection_stdforward_list<T>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.push_front (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Collection_stdforward_list<T>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto      mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Assert (not i.Done ());
                        *mir.fIterator.fStdIterator = newValue;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Collection_stdforward_list<T>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto      mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        //mir.fIterator_.RemoveCurrent ();

                        // HORRIBLE BUT ADEQUITE IMPL...FOR NOW...
                        {
                            auto ei = fData_.before_begin ();
                            for (auto i = fData_.begin (); i != fData_.end (); ++i) {
                                if (i == mir.fIterator.fStdIterator) {
                                    Memory::SmallStackBuffer<typename DataStructureImplType_::ForwardIterator*>   items2Patch (0);
                                    fData_.TwoPhaseIteratorPatcherPass1 (i, &items2Patch);
                                    auto newI = fData_.erase_after (ei);
                                    fData_.TwoPhaseIteratorPatcherPass2 (&items2Patch, newI);
                                    return;
                                }
                                ei = i;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T>
                void    Collection_stdforward_list<T>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.clear_WithPatching ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }



                /*
                ********************************************************************************
                ************************* Collection_stdforward_list<T> ************************
                ********************************************************************************
                */
                template    <typename T>
                Collection_stdforward_list<T>::Collection_stdforward_list ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                Collection_stdforward_list<T>::Collection_stdforward_list (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (start, end);
                }
                template    <typename T>
                Collection_stdforward_list<T>::Collection_stdforward_list (const Collection<T>& src)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                    this->AddAll (src);
                }
                template    <typename T>
                Collection_stdforward_list<T>::Collection_stdforward_list (const Collection_stdforward_list<T>& src)
                    : inherited (static_cast<const inherited&> (src))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T>
                inline  Collection_stdforward_list<T>& Collection_stdforward_list<T>::operator= (const Collection_stdforward_list<T>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Collection_stdforward_list_inl_ */
