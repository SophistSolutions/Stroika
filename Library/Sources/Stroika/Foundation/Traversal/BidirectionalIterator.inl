/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_BidirectionalIterator_inl_
#define _Stroika_Foundation_Traversal_BidirectionalIterator_inl_

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

#if 0

    /*
    ********************************************************************************
    *********************** Iterator<T>::Rep_Cloner_ *******************************
    ********************************************************************************
    */
    template    <typename T>
    inline  shared_ptr<typename    Iterator<T>::IRep>  Iterator<T>::Rep_Cloner_::Copy (const IRep& t)
    {
        return Iterator<T>::Clone_ (t);
    }


    /*
    ********************************************************************************
    ****************************** Iterator<T>::IRep *******************************
    ********************************************************************************
    */
    template    <typename T>
    inline Iterator<T>::IRep::IRep ()
    {
    }
    template    <typename T>
    inline Iterator<T>::IRep::~IRep ()
    {
    }


    /*
    ********************************************************************************
    ********************************** Iterator<T> *********************************
    ********************************************************************************
    */
    template    <typename T>
    inline Iterator<T>::Iterator (const Iterator<T>& from)
        : inherited ()
        , fIterator_ (from.fIterator_)
        , fCurrent_ (from.fCurrent_)
    {
        Require (fIterator_.get () != nullptr or Done ());  // if not special 'auto-done' we must have legit iterator rep
    }
    template    <typename T>
    inline Iterator<T>::Iterator (const BidirectionalIteratorRepSharedPtr& rep)
        : fIterator_ (rep)
        , fCurrent_ ()
    {
        RequireNotNull (rep.get ());
        // Reason for cast stuff is to avoid Clone if unneeded.
        const_cast<IRep*> (rep.get ())->More (&fCurrent_, false);
    }
    template    <typename T>
    inline Iterator<T>::Iterator (ConstructionFlagForceAtEnd_)
        : fIterator_ (nullptr)
        , fCurrent_ ()
    {
        Assert (Done ());
    }
    template    <typename T>
    inline Iterator<T>&    Iterator<T>::operator= (const Iterator<T>& rhs)
    {
        Require (rhs.fIterator_.get () != nullptr or rhs.Done ());  // if not special 'auto-done' we must have legit iterator rep
        fIterator_ = rhs.fIterator_;
        fCurrent_ = rhs.fCurrent_;
        return *this;
    }
    template    <typename T>
    inline  typename    Iterator<T>::IRep&         Iterator<T>::GetRep ()
    {
        EnsureNotNull (fIterator_);
        return *fIterator_;
    }
    template    <typename T>
    inline  const typename Iterator<T>::IRep&   Iterator<T>::GetRep () const
    {
        EnsureNotNull (fIterator_);
        return *fIterator_;
    }
    template    <typename T>
    inline T   Iterator<T>::Current () const
    {
        RequireNotNull (fIterator_);
        Require (fCurrent_.has_value ());
        return *fCurrent_;
    }
    template    <typename T>
    inline bool    Iterator<T>::Done () const
    {
        return not fCurrent_.has_value ();
    }
    template    <typename T>
    inline IteratorOwnerID    Iterator<T>::GetOwner () const
    {
        // We could cache this value, but its only used breaking references and in assertions, so its
        // not clearly worth while
        return fIterator_ == nullptr ? kUnknownIteratorOwnerID : fIterator_->GetOwner ();
    }
    template    <typename T>
    inline    T   Iterator<T>::operator* () const
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        return *fCurrent_;
    }
    template    <typename T>
    inline    T*   Iterator<T>::operator-> ()
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        return fCurrent_.get ();
    }
    template    <typename T>
    inline    const T*   Iterator<T>::operator-> () const
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        return fCurrent_.get ();
    }
    template    <typename T>
    inline   void   Iterator<T>::operator++ ()
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        fIterator_->More (&fCurrent_, true);
    }
    template    <typename T>
    inline   void   Iterator<T>::operator++ (int)
    {
        RequireNotNull (fIterator_);
        Require (not Done ());
        fIterator_->More (&fCurrent_, true);
    }
    template    <typename T>
    inline  Iterator<T>::operator bool () const
    {
        return not Done ();
    }
    template    <typename T>
    inline  bool    Iterator<T>::Equals (const Iterator& rhs) const
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
        const   Iterator<T>::IRep* lhsRep = fIterator_.get ();
        const   Iterator<T>::IRep* rhsRep = rhs.fIterator_.get ();
        Ensure (lhsRep->Equals (rhsRep) == rhsRep->Equals (lhsRep));
        return lhsRep->Equals (rhsRep);
    }
    template    <typename T>
    inline  bool   Iterator<T>::operator== (const Iterator& rhs)  const
    {
        return Equals (rhs);
    }
    template    <typename T>
    inline bool   Iterator<T>::operator!= (const Iterator& rhs)  const
    {
        return not Equals (rhs);
    }
    template    <typename T>
    inline  typename Iterator<T>::RepSmartPtr   Iterator<T>::Clone_ (const typename Iterator<T>::IRep& rep)
    {
        return rep.Clone ();
    }
    template    <typename T>
    inline  Iterator<T> Iterator<T>::GetEmptyIterator ()
    {
        return Iterator<T> (ConstructionFlagForceAtEnd_::ForceAtEnd);
    }
#endif

}

#endif /* _Stroika_Foundation_Traversal_BidirectionalIterator_inl_ */
