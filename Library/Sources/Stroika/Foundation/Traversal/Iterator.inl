/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Iterator_inl_
#define _Stroika_Foundation_Traversal_Iterator_inl_

#include "../Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ******************************** IteratorBase **********************************
     ********************************************************************************
     */
    template <typename SHARED_T, typename... ARGS_TYPE>
    inline auto IteratorBase::MakeSharedPtr (ARGS_TYPE&&... args) -> SharedPtrImplementationTemplate<SHARED_T>
    {
        if constexpr (kIteratorUsesStroikaSharedPtr) {
            return Memory::MakeSharedPtr<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
        else {
            return make_shared<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
    }

    /*
     ********************************************************************************
     ****************** Iterator<T, ITERATOR_TRAITS>::Rep_Cloner_ *******************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline typename IteratorBase::SharedPtrImplementationTemplate<typename Iterator<T, ITERATOR_TRAITS>::IRep> Iterator<T, ITERATOR_TRAITS>::Rep_Cloner_::Copy (const IRep& t)
    {
        return Iterator<T, ITERATOR_TRAITS>::Clone_ (t);
    }

    /*
     ********************************************************************************
     *************************** Iterator<T, ITERATOR_TRAITS> ***********************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const IteratorRepSharedPtr& rep)
        : fIterator_ (rep)
        , fCurrent_ ()
    {
        RequireNotNull (rep.get ());
        // Reason for cast stuff is to avoid Clone if unneeded.
        const_cast<IRep*> (rep.get ())->More (&fCurrent_, false);
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (IteratorRepSharedPtr&& rep)
        : fIterator_ (move (rep))
        , fCurrent_ ()
    {
        RequireNotNull (fIterator_);
        // Reason for cast stuff is to avoid Clone if unneeded.
        const_cast<IRep*> (fIterator_.get ())->More (&fCurrent_, false);
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (nullptr_t)
        : Iterator (ConstructionFlagForceAtEnd_::ForceAtEnd)
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (ConstructionFlagForceAtEnd_)
        : fIterator_ (nullptr)
        , fCurrent_ ()
    {
        Assert (Done ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline typename Iterator<T, ITERATOR_TRAITS>::IRep& Iterator<T, ITERATOR_TRAITS>::GetRep ()
    {
        EnsureNotNull (fIterator_);
        return *fIterator_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const typename Iterator<T, ITERATOR_TRAITS>::IRep& Iterator<T, ITERATOR_TRAITS>::ConstGetRep () const
    {
        EnsureNotNull (fIterator_);
        return *fIterator_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline T Iterator<T, ITERATOR_TRAITS>::Current () const
    {
        RequireNotNull (fIterator_);
        Require (fCurrent_.has_value ());
        return *fCurrent_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::Done () const
    {
        return not fCurrent_.has_value ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::reset ()
    {
        *this = GetEmptyIterator ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::clear ()
    {
        *this = GetEmptyIterator ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline IteratorOwnerID Iterator<T, ITERATOR_TRAITS>::GetOwner () const
    {
        // We could cache this value, but its only used breaking references and in assertions, so its
        // not clearly worth while
        return fIterator_ == nullptr ? kUnknownIteratorOwnerID : fIterator_->GetOwner ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline T Iterator<T, ITERATOR_TRAITS>::operator* () const
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        return *fCurrent_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator-> () const -> const value_type*
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        return fCurrent_.operator-> ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T>& Iterator<T, ITERATOR_TRAITS>::operator++ ()
    {
        Require (not Done ());
        RequireNotNull (fIterator_);
        fIterator_->More (&fCurrent_, true);
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T> Iterator<T, ITERATOR_TRAITS>::operator++ (int)
    {
        RequireNotNull (fIterator_);
        Require (not Done ());
        Iterator<T> tmp = *this;
        fIterator_->More (&fCurrent_, true);
        return tmp;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS> Iterator<T, ITERATOR_TRAITS>::operator+ (int i) const
    {
        Require (i >= 0);
        Iterator<T, ITERATOR_TRAITS> tmp{*this};
        while (i > 0) {
            --i;
            ++tmp;
        }
        return tmp;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::operator bool () const
    {
        return not Done ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::Equals (const Iterator& rhs) const
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
        bool lDone = Done ();
        bool rDone = rhs.Done ();
        if (lDone != rDone) {
            return false;
        }
        if (lDone) {
            Assert (rDone);
            return true;
        }
        Assert (not lDone and not rDone);
        // assigning to local variables to ensure const version called
        const Iterator<T, ITERATOR_TRAITS>::IRep* lhsRep = fIterator_.cget ();
        const Iterator<T, ITERATOR_TRAITS>::IRep* rhsRep = rhs.fIterator_.cget ();
        Ensure (lhsRep->Equals (rhsRep) == rhsRep->Equals (lhsRep));
        return lhsRep->Equals (rhsRep);
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline typename Iterator<T, ITERATOR_TRAITS>::IteratorRepSharedPtr Iterator<T, ITERATOR_TRAITS>::Clone_ (const typename Iterator<T, ITERATOR_TRAITS>::IRep& rep)
    {
        return rep.Clone ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS> Iterator<T, ITERATOR_TRAITS>::GetEmptyIterator ()
    {
        return Iterator<T, ITERATOR_TRAITS> (ConstructionFlagForceAtEnd_::ForceAtEnd);
    }

    /*
     ********************************************************************************
     **************************** Iterator operators ********************************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline bool operator== (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs)
    {
        return lhs.Equals (rhs);
    }

    template <typename T, typename ITERATOR_TRAITS>
    inline bool operator!= (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs)
    {
        return not lhs.Equals (rhs);
    }

    /*
     ********************************************************************************
     ***************************** Iterator2Pointer *********************************
     ********************************************************************************
     */
    template <typename ITERATOR>
    inline typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i)
    {
        // this overload wont always work.. I hope it gives good compiler error message??? --LGP 2014-10-07
        //
        // note Traversal::Iterator2Pointer (s.end ()) generally crashes in debug mode - windows - _ITERATOR_DEBUG_LEVEL >= 1, but I can find no better way which is portable
        return &*i;
    }

}

#endif /* _Stroika_Foundation_Traversal_Iterator_inl_ */
