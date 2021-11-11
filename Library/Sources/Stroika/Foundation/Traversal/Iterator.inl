/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
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
    inline auto IteratorBase::MakeSmartPtr (ARGS_TYPE&&... args) -> PtrImplementationTemplate<SHARED_T>
    {
        return make_unique<SHARED_T> (forward<ARGS_TYPE> (args)...);
    }

    /*
     ********************************************************************************
     *************************** Iterator<T, ITERATOR_TRAITS> ***********************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const Iterator& src)
        : fRep_{src.fRep_ == nullptr ? nullptr : Clone_ (*src.fRep_)}
        , fCurrentValue_{src.fCurrentValue_}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (RepSmartPtr&& rep)
        : fRep_{move (rep)}
    {
        RequireNotNull (fRep_);
        fRep_->More (&fCurrentValue_, false);
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (nullptr_t)
        : Iterator{ConstructionFlagForceAtEnd_::ForceAtEnd}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (ConstructionFlagForceAtEnd_)
        : fRep_{nullptr}
    {
        Assert (Done ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    Iterator<T, ITERATOR_TRAITS>& Iterator<T, ITERATOR_TRAITS>::operator= (const Iterator& rhs)
    {
        if (&rhs != this) [[LIKELY_ATTR]] {
            fRep_     = rhs.fRep_ == nullptr ? nullptr : Clone_ (*rhs.fRep_);
            fCurrentValue_ = rhs.fCurrentValue_;
        }
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline typename Iterator<T, ITERATOR_TRAITS>::IRep& Iterator<T, ITERATOR_TRAITS>::GetRep ()
    {
        EnsureNotNull (fRep_);
        return *fRep_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const typename Iterator<T, ITERATOR_TRAITS>::IRep& Iterator<T, ITERATOR_TRAITS>::ConstGetRep () const
    {
        EnsureNotNull (fRep_);
        return *fRep_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::Refresh () const
    {
        auto mutableThis = const_cast<Iterator*> (this);
        fRep_->More (&mutableThis->fCurrentValue_, false);
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline T Iterator<T, ITERATOR_TRAITS>::Current () const
    {
        RequireNotNull (fRep_);
        Require (fCurrentValue_.has_value ());
        return *fCurrentValue_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::Done () const
    {
        return not fCurrentValue_.has_value ();
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
    inline T Iterator<T, ITERATOR_TRAITS>::operator* () const
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        return *fCurrentValue_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator-> () const -> const value_type*
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        return fCurrentValue_.operator-> ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T>& Iterator<T, ITERATOR_TRAITS>::operator++ ()
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        fRep_->More (&fCurrentValue_, true);
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T> Iterator<T, ITERATOR_TRAITS>::operator++ (int)
    {
        RequireNotNull (fRep_);
        Require (not Done ());
        Iterator<T> tmp = *this;
        fRep_->More (&fCurrentValue_, true);
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
#if __cpp_impl_three_way_comparison >= 201907
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::operator== (const Iterator& rhs) const
    {
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
        if (lDone != rDone) [[LIKELY_ATTR]] {
            return false;
        }
        if (lDone) {
            Assert (rDone);
            return true;
        }
        Assert (not lDone and not rDone);
        const Iterator<T, ITERATOR_TRAITS>::IRep* lhsRep = fRep_.get ();
        const Iterator<T, ITERATOR_TRAITS>::IRep* rhsRep = rhs.fRep_.get ();
        Ensure (lhsRep->Equals (rhsRep) == rhsRep->Equals (lhsRep));
        return lhsRep->Equals (rhsRep);
    }
#endif
    template <typename T, typename ITERATOR_TRAITS>
    inline typename Iterator<T, ITERATOR_TRAITS>::RepSmartPtr Iterator<T, ITERATOR_TRAITS>::Clone_ (const typename Iterator<T, ITERATOR_TRAITS>::IRep& rep)
    {
        return rep.Clone ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS> Iterator<T, ITERATOR_TRAITS>::GetEmptyIterator ()
    {
        return Iterator<T, ITERATOR_TRAITS>{ConstructionFlagForceAtEnd_::ForceAtEnd};
    }

#if __cpp_impl_three_way_comparison < 201907
    /*
     ********************************************************************************
     **************************** Iterator operators ********************************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline bool operator== (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs)
    {
        bool lDone = lhs.Done ();
        bool rDone = rhs.Done ();
        if (lDone != rDone) [[LIKELY_ATTR]] {
            return false;
        }
        if (lDone) {
            Assert (rDone);
            return true;
        }
        Assert (not lDone and not rDone);
        const typename Iterator<T, ITERATOR_TRAITS>::IRep* lhsRep = lhs.fRep_.get ();
        const typename Iterator<T, ITERATOR_TRAITS>::IRep* rhsRep = rhs.fRep_.get ();
        Ensure (lhsRep->Equals (rhsRep) == rhsRep->Equals (lhsRep));
        return lhsRep->Equals (rhsRep);
    }

    template <typename T, typename ITERATOR_TRAITS>
    inline bool operator!= (const Iterator<T, ITERATOR_TRAITS>& lhs, const Iterator<T, ITERATOR_TRAITS>& rhs)
    {
        return not(lhs == rhs);
    }
#endif

    /*
     ********************************************************************************
     ***************************** Iterator2Pointer *********************************
     ********************************************************************************
     */
    template <typename ITERATOR>
    constexpr typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i)
    {
        // this overload wont always work.. I hope it gives good compiler error message??? --LGP 2014-10-07
        //
        // note Traversal::Iterator2Pointer (s.end ()) generally crashes in debug mode - windows - _ITERATOR_DEBUG_LEVEL >= 1, but I can find no better way which is portable
        return &*i;
    }

}

#endif /* _Stroika_Foundation_Traversal_Iterator_inl_ */
