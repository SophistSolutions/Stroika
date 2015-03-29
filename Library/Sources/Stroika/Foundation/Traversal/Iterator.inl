/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_inl_
#define _Stroika_Foundation_Traversal_Iterator_inl_

#include    "../Debug/Assertions.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
            ********************************************************************************
            **************** Iterator<T, BASE_STD_ITERATOR>::Rep_Cloner_ *******************
            ********************************************************************************
            */
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  typename IteratorBase::SharedPtrImplementationTemplate<typename Iterator<T, BASE_STD_ITERATOR>::IRep>  Iterator<T, BASE_STD_ITERATOR>::Rep_Cloner_::Copy (const IRep& t)
            {
                return Iterator<T, BASE_STD_ITERATOR>::Clone_ (t);
            }


            /*
            ********************************************************************************
            ******************** Iterator<T, BASE_STD_ITERATOR>::IRep **********************
            ********************************************************************************
            */
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>::IRep::IRep ()
            {
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>::IRep::~IRep ()
            {
            }


            /*
            ********************************************************************************
            ************************* Iterator<T, BASE_STD_ITERATOR> ***********************
            ********************************************************************************
            */
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>::Iterator (const Iterator& from)
                : inherited ()
                , fIterator_ (from.fIterator_)
                , fCurrent_ (from.fCurrent_)
            {
                Require (fIterator_.cget () != nullptr or Done ());  // if not special 'auto-done' we must have legit iterator rep
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>::Iterator (const SharedIRepPtr& rep)
                : fIterator_ (rep)
                , fCurrent_ ()
            {
                RequireNotNull (rep.get ());
                // Reason for cast stuff is to avoid Clone if unneeded.
                const_cast<IRep*> (rep.get ())->More (&fCurrent_, false);
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>::Iterator (ConstructionFlagForceAtEnd_)
                : fIterator_ (nullptr)
                , fCurrent_ ()
            {
                Assert (Done ());
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline Iterator<T, BASE_STD_ITERATOR>&    Iterator<T, BASE_STD_ITERATOR>::operator= (const Iterator& rhs)
            {
                Require (rhs.fIterator_.cget () != nullptr or rhs.Done ());  // if not special 'auto-done' we must have legit iterator rep
                fIterator_ = rhs.fIterator_;
                fCurrent_ = rhs.fCurrent_;
                return *this;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  typename    Iterator<T, BASE_STD_ITERATOR>::IRep&         Iterator<T, BASE_STD_ITERATOR>::GetRep ()
            {
                EnsureNotNull (fIterator_);
                return *fIterator_;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  const typename Iterator<T, BASE_STD_ITERATOR>::IRep&   Iterator<T, BASE_STD_ITERATOR>::GetRep () const
            {
                EnsureNotNull (fIterator_);
                return *fIterator_;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline T   Iterator<T, BASE_STD_ITERATOR>::Current () const
            {
                RequireNotNull (fIterator_);
                Require (fCurrent_.IsPresent ());
                return *fCurrent_;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline bool    Iterator<T, BASE_STD_ITERATOR>::Done () const
            {
                return fCurrent_.IsMissing ();
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline IteratorOwnerID    Iterator<T, BASE_STD_ITERATOR>::GetOwner () const
            {
                // We could cache this value, but its only used breaking references and in assertions, so its
                // not clearly worth while
                return fIterator_ == nullptr ? kUnknownIteratorOwnerID : fIterator_->GetOwner ();
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline    T   Iterator<T, BASE_STD_ITERATOR>::operator* () const
            {
                Require (not Done ());
                RequireNotNull (fIterator_);
                return *fCurrent_;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline    const T*   Iterator<T, BASE_STD_ITERATOR>::operator-> () const
            {
                Require (not Done ());
                RequireNotNull (fIterator_);
                return fCurrent_.peek ();
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline   Iterator<T>&   Iterator<T, BASE_STD_ITERATOR>::operator++ ()
            {
                Require (not Done ());
                RequireNotNull (fIterator_);
                fIterator_->More (&fCurrent_, true);
                return *this;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline   Iterator<T>   Iterator<T, BASE_STD_ITERATOR>::operator++ (int)
            {
                RequireNotNull (fIterator_);
                Require (not Done ());
                Iterator<T> tmp = *this;
                fIterator_->More (&fCurrent_, true);
                return tmp;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline   Iterator<T, BASE_STD_ITERATOR>   Iterator<T, BASE_STD_ITERATOR>::operator+ (int i) const
            {
                Require (i >= 0);
                Iterator<T, BASE_STD_ITERATOR> tmp { *this };
                while (i > 0) {
                    --i;
                    ++tmp;
                }
                return tmp;
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  Iterator<T, BASE_STD_ITERATOR>::operator bool () const
            {
                return not Done ();
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  bool    Iterator<T, BASE_STD_ITERATOR>::Equals (const Iterator& rhs) const
            {
                Require (GetOwner () == rhs.GetOwner () or GetOwner () == kUnknownIteratorOwnerID or rhs.GetOwner () == kUnknownIteratorOwnerID);
                /*
                 *  Equals is checked by first checking handling the case of special 'done' iterators. If two
                 *  iterators differ on Done () - they cannot be equal. And if they are both done (this is special -
                 *  even if from different sources) they are considered equal.
                 *
                 *  But then - we check that they are the same dynamic type, and if so, hand to one,
                 *  and let it do the dynamic/concrete type specific checks for equality.
                 */
                bool    lDone   =   Done ();
                bool    rDone   =   rhs.Done ();
                if (lDone != rDone) {
                    return false;
                }
                if (lDone) {
                    Assert (rDone);
                    return true;
                }
                Assert (not lDone and not rDone);
                // assigning to local variables to ensure const version called
                const   Iterator<T, BASE_STD_ITERATOR>::IRep* lhsRep = fIterator_.cget ();
                const   Iterator<T, BASE_STD_ITERATOR>::IRep* rhsRep = rhs.fIterator_.cget ();
                Ensure (lhsRep->Equals (rhsRep) == rhsRep->Equals (lhsRep));
                return lhsRep->Equals (rhsRep);
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  typename Iterator<T, BASE_STD_ITERATOR>::SharedIRepPtr   Iterator<T, BASE_STD_ITERATOR>::Clone_ (const typename Iterator<T, BASE_STD_ITERATOR>::IRep& rep)
            {
                return rep.Clone ();
            }
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  Iterator<T, BASE_STD_ITERATOR> Iterator<T, BASE_STD_ITERATOR>::GetEmptyIterator ()
            {
                return Iterator<T, BASE_STD_ITERATOR> (ConstructionFlagForceAtEnd_::ForceAtEnd);
            }


            /*
             ********************************************************************************
             ************************* Iterator operators ***********************************
             ********************************************************************************
             */
            template    <typename T, typename BASE_STD_ITERATOR>
            inline  bool   operator== (const Iterator<T, BASE_STD_ITERATOR>& lhs, const Iterator<T, BASE_STD_ITERATOR>& rhs)
            {
                return lhs.Equals (rhs);
            }

            template    <typename T, typename BASE_STD_ITERATOR>
            inline bool   operator!= (const Iterator<T, BASE_STD_ITERATOR>& lhs, const Iterator<T, BASE_STD_ITERATOR>& rhs)
            {
                return not lhs.Equals (rhs);
            }


            /*
             ********************************************************************************
             ***************************** Iterator2Pointer *********************************
             ********************************************************************************
             */
            template    <typename   ITERATOR>
            inline  typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i)
            {
                // this overload wont always work.. I hope it gives good compiler error message??? --LGP 2014-10-07
                return &*i;
            }


        }
    }
}

#endif /* _Stroika_Foundation_Traversal_Iterator_inl_ */
