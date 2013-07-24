/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_
#define _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/PatchingDataStructures/Array.h"
#include    "../Private/IteratorImplHelper.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                /*
                 ********************************************************************************
                 **************************** Bag_Array<T,TRAITS>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Bag_Array<T, TRAITS>::Rep_ : public Bag<T, TRAITS>::_IRep {
                private:
                    typedef typename    Bag<T, TRAITS>::_IRep   inherited;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from);
                    NO_ASSIGNMENT_OPERATOR(Rep_);

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual typename Iterable<T>::_SharedPtrIRep    Clone () const override;
                    virtual Iterator<T>                             MakeIterator () const override;
                    virtual size_t                                  GetLength () const override;
                    virtual bool                                    IsEmpty () const override;
                    virtual void                                    Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>                             ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const override;

                    // Bag<T,TRAITS>::_IRep overrides
                public:
                    virtual bool    Equals (const typename Bag<T, TRAITS>::_IRep& rhs) const override;
                    virtual bool    Contains (T item) const override;
                    virtual size_t  TallyOf (T item) const override;
                    virtual void    Add (T item) override;
                    virtual void    Update (const Iterator<T>& i, T newValue) override;
                    virtual void    Remove (T item) override;
                    virtual void    Remove (const Iterator<T>& i) override;
                    virtual void    RemoveAll () override;

                private:
                    typedef Private::PatchingDataStructures::Array_Patch <
                    T,
                    Private::DataStructures::Array_DefaultTraits<T, typename TRAITS::EqualsCompareFunctionType>
                    >
                    DataStructureImplType_;

                private:
                    Private::ContainerRepLockDataSupport_   fLockSupport_;
                    DataStructureImplType_                  fData_;


                private:
                    typedef typename Private::IteratorImplHelper_<T, DataStructureImplType_>    IteratorRep_;
                };


                /*
                ********************************************************************************
                *********************** Bag_Array<T,TRAITS>::Rep_ ******************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Bag_Array<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fLockSupport_ ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Bag_Array<T, TRAITS>::Rep_::Rep_ (const Rep_& from)
                    : fLockSupport_ ()
                    , fData_ ()
                {
                    CONTAINER_LOCK_HELPER_START (from.fLockSupport_) {
                        fData_ = from.fData_;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                typename Iterable<T>::_SharedPtrIRep  Bag_Array<T, TRAITS>::Rep_::Clone () const
                {
                    // no lock needed cuz src locked in Rep_ CTOR
                    return typename Iterable<T>::_SharedPtrIRep (new Rep_ (*this));
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Bag_Array<T, TRAITS>::Rep_::MakeIterator () const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (&NON_CONST_THIS->fLockSupport_, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    Iterator<T> tmp = Iterator<T> (tmpRep);
                    tmp++;  //tmphack - redo iterator impl itself
                    return tmp;
                }
                template    <typename T, typename TRAITS>
                size_t  Bag_Array<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Bag_Array<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.GetLength () == 0);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Bag_Array<T, TRAITS>::Rep_::Apply (typename Rep_::_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Bag_Array<T, TRAITS>::Rep_::ApplyUntilTrue (typename Rep_::_APPLYUNTIL_ARGTYPE doToElement) const
                {
                    return this->_ApplyUntilTrue (doToElement);
                }
                template    <typename T, typename TRAITS>
                bool    Bag_Array<T, TRAITS>::Rep_::Equals (const typename Bag<T, TRAITS>::_IRep& rhs) const
                {
                    return this->_Equals_Reference_Implementation (rhs);
                }
                template    <typename T, typename TRAITS>
                bool    Bag_Array<T, TRAITS>::Rep_::Contains (T item) const
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        return (fData_.Contains (item));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                size_t    Bag_Array<T, TRAITS>::Rep_::TallyOf (T item) const
                {
                    return this->_TallyOf_Reference_Implementation (item);
                }
                template    <typename T, typename TRAITS>
                void    Bag_Array<T, TRAITS>::Rep_::Add (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        // Appending is fastest
                        fData_.InsertAt (fData_.GetLength (), item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_Array<T, TRAITS>::Rep_::Update (const Iterator<T>& i, T newValue)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto       mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.SetAt (mir.fIterator, newValue);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_Array<T, TRAITS>::Rep_::Remove (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        /*
                         *  Iterate backwards since removing from the end of an array will be faster.
                         */
                        // DEBUG WHY USING BACKWARD ITERATOR GENERATES ERROR ON MSVC - COMPILE ERROR ABOUT TYPE MISMATCH-- WHEN I SWITCHED TO USING DataStructureImplType_...
                        // NOT IMPORTANT NOW _ SO DEFER
                        //for (typename DataStructureImplType_::BackwardIterator it (fData_); it.More (nullptr, true);) {
                        for (typename DataStructureImplType_::ForwardIterator it (fData_); it.More (nullptr, true);) {
                            if (TRAITS::EqualsCompareFunctionType::Equals (it.Current (), item)) {
                                fData_.RemoveAt (it.CurrentIndex ());
                                return;
                            }
                        }
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_Array<T, TRAITS>::Rep_::Remove (const Iterator<T>& i)
                {
                    const typename Iterator<T>::IRep&    ir  =   i.GetRep ();
                    AssertMember (&ir, IteratorRep_);
                    auto      mir =   dynamic_cast<const IteratorRep_&> (ir);
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAt (mir.fIterator);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Bag_Array<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fLockSupport_) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                ***************************** Bag_Array<T,TRAITS> ******************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Bag_Array<T, TRAITS>::Bag_Array ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                }
                template    <typename T, typename TRAITS>
                Bag_Array<T, TRAITS>::Bag_Array (const Bag<T, TRAITS>& bag)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    SetCapacity (bag.GetLength ());
                    this->AddAll (bag);
                }
                template    <typename T, typename TRAITS>
                Bag_Array<T, TRAITS>::Bag_Array (const T* start, const T* end)
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    Require ((start == end) or (start != nullptr and end != nullptr));
                    if (start != end) {
                        SetCapacity (end - start);
                        this->AddAll (start, end);
                    }
                }
                template    <typename T, typename TRAITS>
                inline  Bag_Array<T, TRAITS>::Bag_Array (const Bag_Array<T, TRAITS>& bag)
                    : inherited (static_cast<const inherited&> (bag))
                {
                }
                template    <typename T, typename TRAITS>
                inline  Bag_Array<T, TRAITS>&   Bag_Array<T, TRAITS>::operator= (const Bag_Array<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    return *this;
                }
                template    <typename T, typename TRAITS>
                inline  const typename Bag_Array<T, TRAITS>::Rep_&  Bag_Array<T, TRAITS>::GetRep_ () const
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<const Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename TRAITS>
                inline  typename Bag_Array<T, TRAITS>::Rep_&    Bag_Array<T, TRAITS>::GetRep_ ()
                {
                    /*
                     * This cast is safe since we there is no Iterable<T>::_SetRep() - and so no way to ever change
                     * the type of rep our CTOR bases to Iterable<T>.
                     */
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return (static_cast<Rep_&> (inherited::_GetRep ()));
                }
                template    <typename T, typename TRAITS>
                inline  void    Bag_Array<T, TRAITS>::Compact ()
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.Compact ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  size_t  Bag_Array<T, TRAITS>::GetCapacity () const
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        return (GetRep_ ().fData_.GetCapacity ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                inline  void    Bag_Array<T, TRAITS>::SetCapacity (size_t slotsAlloced)
                {
                    CONTAINER_LOCK_HELPER_START (GetRep_ ().fLockSupport_) {
                        GetRep_ ().fData_.SetCapacity (slotsAlloced);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


            }
        }
    }
}

#endif /* _Stroika_Foundation_Containers_Concrete_Bag_Array_inl_ */
