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
     ********************** Iterator<T, ITERATOR_TRAITS>::IRep **********************
     ********************************************************************************
     */
#if qDebug
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::IRep::Invariant () const noexcept
    {
    }
#endif

    /*
     ********************************************************************************
     *************************** Iterator<T, ITERATOR_TRAITS> ***********************
     ********************************************************************************
     */
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const RepSmartPtr& rep) noexcept
        : fRep_{rep}
    {
        RequireNotNull (fRep_);
        fRep_->More (&fCurrentValue_, false);
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (RepSmartPtr&& rep) noexcept
        : fRep_{move (rep)}
    {
        RequireNotNull (fRep_);
        fRep_->More (&fCurrentValue_, false);
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const Iterator& src)
        : fRep_{src.fRep_ == nullptr ? nullptr : Clone_ (*src.fRep_)}
        , fCurrentValue_{src.fCurrentValue_}
    {
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (Iterator&& src) noexcept
        : fRep_{move (src.fRep_)}
        , fCurrentValue_{move (src.fCurrentValue_)}
    {
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (nullptr_t) noexcept
        : Iterator{ConstructionFlagForceAtEnd_::ForceAtEnd}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (ConstructionFlagForceAtEnd_) noexcept
        : fRep_{nullptr}
    {
        Assert (Done ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>& Iterator<T, ITERATOR_TRAITS>::operator= (const Iterator& rhs)
    {
        if (&rhs != this) [[LIKELY_ATTR]] {
            fRep_          = rhs.fRep_ == nullptr ? nullptr : Clone_ (*rhs.fRep_);
            fCurrentValue_ = rhs.fCurrentValue_;
            this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
        }
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>& Iterator<T, ITERATOR_TRAITS>::operator= (Iterator&& rhs) noexcept
    {
        if (&rhs != this) [[LIKELY_ATTR]] {
            fRep_          = move (rhs.fRep_);
            fCurrentValue_ = move (rhs.fCurrentValue_);
            this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
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
    inline void Iterator<T, ITERATOR_TRAITS>::Refresh ()
    {
        fRep_->More (&this->fCurrentValue_, false);
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::Invariant () const noexcept
    {
#if qDebug
        if (fRep_) {
            fRep_->Invariant ();
        }
#endif
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const T& Iterator<T, ITERATOR_TRAITS>::Current () const
    {
        RequireNotNull (fRep_);
        Require (fCurrentValue_.has_value ());
        this->Invariant ();
        return *fCurrentValue_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::Done () const
    {
        this->Invariant ();
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
    inline const T& Iterator<T, ITERATOR_TRAITS>::operator* () const
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        this->Invariant ();
        return *fCurrentValue_;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator-> () const -> const value_type*
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        this->Invariant ();
        return fCurrentValue_.operator-> ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator++ () -> Iterator&
    {
        Require (not Done ());
        RequireNotNull (fRep_);
        fRep_->More (&fCurrentValue_, true);
        this->Invariant (); // could do before and after but this is a good cost/benfit trade-off
        return *this;
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
    template <typename T, typename ITERATOR_TRAITS>
    inline typename Iterator<T, ITERATOR_TRAITS>::RepSmartPtr Iterator<T, ITERATOR_TRAITS>::Clone_ (const typename Iterator<T, ITERATOR_TRAITS>::IRep& rep)
    {
        return rep.Clone ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr inline Iterator<T, ITERATOR_TRAITS> Iterator<T, ITERATOR_TRAITS>::GetEmptyIterator ()
    {
        return Iterator<T, ITERATOR_TRAITS>{ConstructionFlagForceAtEnd_::ForceAtEnd};
    }

    /*
     ********************************************************************************
     ***************************** Iterator2Pointer *********************************
     ********************************************************************************
     */
    template <typename ITERATOR>
    constexpr inline typename iterator_traits<ITERATOR>::pointer Iterator2Pointer (ITERATOR i)
    {
        // this overload wont always work.. I hope it gives good compiler error message??? --LGP 2014-10-07
        //
        // note Traversal::Iterator2Pointer (s.end ()) generally crashes in debug mode - windows - _ITERATOR_DEBUG_LEVEL >= 1, but I can find no better way which is portable
        return &*i;
    }

}

#endif /* _Stroika_Foundation_Traversal_Iterator_inl_ */
