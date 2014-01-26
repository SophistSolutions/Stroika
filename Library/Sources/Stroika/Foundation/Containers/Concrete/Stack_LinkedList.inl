/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_
#define _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "../../Memory/BlockAllocated.h"

#include    "../Private/IteratorImplHelper.h"
#include    "../Private/PatchingDataStructures/LinkedList.h"
#include    "../Private/SynchronizationUtils.h"


namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {
            namespace   Concrete {


                using   Traversal::IteratorOwnerID;


                /*
                 ********************************************************************************
                 ******************** Stack_LinkedList<T, TRAITS>::Rep_ *************************
                 ********************************************************************************
                 */
                template    <typename T, typename TRAITS>
                class   Stack_LinkedList<T, TRAITS>::Rep_ : public Stack<T, TRAITS>::_IRep {
                private:
                    using   inherited   =   typename    Stack<T, TRAITS>::_IRep;

                public:
                    using   _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;
                    using   _APPLY_ARGTYPE = typename inherited::_APPLY_ARGTYPE;
                    using   _APPLYUNTIL_ARGTYPE = typename inherited::_APPLYUNTIL_ARGTYPE;

                public:
                    Rep_ ();
                    Rep_ (const Rep_& from) = delete;
                    Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope);

                public:
                    nonvirtual  const Rep_& operator= (const Rep_&) = delete;

                public:
                    DECLARE_USE_BLOCK_ALLOCATION (Rep_);

                    // Iterable<T>::_IRep overrides
                public:
                    virtual _SharedPtrIRep    Clone (IteratorOwnerID forIterableEnvelope) const override;
                    virtual Iterator<T>       MakeIterator (IteratorOwnerID suggestedOwner) const override;
                    virtual size_t            GetLength () const override;
                    virtual bool              IsEmpty () const override;
                    virtual void              Apply (_APPLY_ARGTYPE doToElement) const override;
                    virtual Iterator<T>       ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const override;

                    // Stack<T, TRAITS>::_IRep overrides
                public:
                    virtual void    Push (T item) override;
                    virtual T       Pop () override;
                    virtual T       Top () const override;
                    virtual void    RemoveAll () override;

                private:
                    using   DataStructureImplType_  =   Private::PatchingDataStructures::LinkedList<T, Private::ContainerRepLockDataSupport_>;
                    using   IteratorRep_            =   typename Private::IteratorImplHelper_<T, DataStructureImplType_>;

                private:
                    DataStructureImplType_  fData_;
                };


                /*
                ********************************************************************************
                ******************** Stack_LinkedList<T, TRAITS>::Rep_ *************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                inline  Stack_LinkedList<T, TRAITS>::Rep_::Rep_ ()
                    : inherited ()
                    , fData_ ()
                {
                }
                template    <typename T, typename TRAITS>
                inline  Stack_LinkedList<T, TRAITS>::Rep_::Rep_ (Rep_* from, IteratorOwnerID forIterableEnvelope)
                    : inherited ()
                    , fData_ (&from->fData_, forIterableEnvelope)
                {
                    RequireNotNull (from);
                }
                template    <typename T, typename TRAITS>
                typename Stack_LinkedList<T, TRAITS>::Rep_::_SharedPtrIRep  Stack_LinkedList<T, TRAITS>::Rep_::Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        // const cast because though cloning LOGICALLY makes no changes in reality we have to patch iterator lists
                        return _SharedPtrIRep (new Rep_ (const_cast<Rep_*> (this), forIterableEnvelope));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                Iterator<T>  Stack_LinkedList<T, TRAITS>::Rep_::MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    typename Iterator<T>::SharedIRepPtr tmpRep;
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        Rep_*   NON_CONST_THIS  =   const_cast<Rep_*> (this);       // logically const, but non-const cast cuz re-using iterator API
                        tmpRep = typename Iterator<T>::SharedIRepPtr (new IteratorRep_ (suggestedOwner, &NON_CONST_THIS->fData_));
                    }
                    CONTAINER_LOCK_HELPER_END ();
                    return Iterator<T> (tmpRep);
                }
                template    <typename T, typename TRAITS>
                size_t  Stack_LinkedList<T, TRAITS>::Rep_::GetLength () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.GetLength ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                bool  Stack_LinkedList<T, TRAITS>::Rep_::IsEmpty () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return (fData_.IsEmpty ());
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void      Stack_LinkedList<T, TRAITS>::Rep_::Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                template    <typename T, typename TRAITS>
                Iterator<T>     Stack_LinkedList<T, TRAITS>::Rep_::ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    return this->_ApplyUntilTrue (doToElement, suggestedOwner);
                }
                template    <typename T, typename TRAITS>
                void    Stack_LinkedList<T, TRAITS>::Rep_::RemoveAll ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.RemoveAll ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                void    Stack_LinkedList<T, TRAITS>::Rep_::Push (T item)
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        fData_.Append (item);
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Stack_LinkedList<T, TRAITS>::Rep_::Pop ()
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        T   result  =   fData_.GetFirst ();
                        fData_.RemoveFirst ();
                        // FIX/PATCH
                        return result;
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }
                template    <typename T, typename TRAITS>
                T    Stack_LinkedList<T, TRAITS>::Rep_::Top () const
                {
                    CONTAINER_LOCK_HELPER_START (fData_.fLockSupport) {
                        return fData_.GetFirst ();
                    }
                    CONTAINER_LOCK_HELPER_END ();
                }


                /*
                ********************************************************************************
                *************************** Stack_LinkedList<T, TRAITS> ************************
                ********************************************************************************
                */
                template    <typename T, typename TRAITS>
                Stack_LinkedList<T, TRAITS>::Stack_LinkedList ()
                    : inherited (typename inherited::_SharedPtrIRep (new Rep_ ()))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                inline  Stack_LinkedList<T, TRAITS>::Stack_LinkedList (const Stack_LinkedList<T, TRAITS>& s)
                    : inherited (static_cast<const inherited&> (s))
                {
                    AssertMember (&inherited::_GetRep (), Rep_);
                }
                template    <typename T, typename TRAITS>
                inline  Stack_LinkedList<T, TRAITS>&   Stack_LinkedList<T, TRAITS>::operator= (const Stack_LinkedList<T, TRAITS>& rhs)
                {
                    inherited::operator= (static_cast<const inherited&> (rhs));
                    AssertMember (&inherited::_GetRep (), Rep_);
                    return *this;
                }


            }
        }
    }
}
#endif /* _Stroika_Foundation_Containers_Concrete_Stack_LinkedList_inl_ */
