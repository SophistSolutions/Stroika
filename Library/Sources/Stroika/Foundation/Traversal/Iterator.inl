/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2026.  All rights reserved
 */
#include "Stroika/Foundation/Debug/Assertions.h"

namespace Stroika::Foundation::Traversal {

    /**
     */
    struct [[deprecated ("Since Stroika v3.0d1")]] IteratorBase {
    public:
        template <typename SHARED_T>
        using PtrImplementationTemplate [[deprecated ("Since Stroika v3.0d1 - use unique_ptr directly")]] = unique_ptr<SHARED_T>;

    public:
        template <typename SHARED_T, typename... ARGS_TYPE>
        [[deprecated ("Since Stroika v3.0d1 - make_unique directly")]] static unique_ptr<SHARED_T> MakeSmartPtr (ARGS_TYPE&&... args)
        {
            return make_unique<SHARED_T> (forward<ARGS_TYPE> (args)...);
        }
    };

    /*
     ********************************************************************************
     ********************** Iterator<T, ITERATOR_TRAITS>::IRep **********************
     ********************************************************************************
     */
#if qStroika_Foundation_Debug_AssertionsChecked
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
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const unique_ptr<IRep>& rep) noexcept
        : fRep_{rep}
        , _fCurrentValue{fRep_->Current ()}
    {
        RequireNotNull (fRep_);
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (unique_ptr<IRep>&& rep) noexcept
        : fRep_{move (rep)}
        , _fCurrentValue{fRep_->Current ()}
    {
        RequireNotNull (fRep_);
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (const Iterator& src)
        : fRep_{src.fRep_ == nullptr ? nullptr : Clone_ (*src.fRep_)}
        , _fCurrentValue{src._fCurrentValue}
    {
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>::Iterator (Iterator&& src) noexcept
        : fRep_{move (src.fRep_)}
        , _fCurrentValue{move (src._fCurrentValue)}
    {
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (const default_sentinel_t&) noexcept
        : fRep_{nullptr}
    {
        Assert (AtEnd ());
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator (nullptr_t) noexcept
        : Iterator{default_sentinel}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr Iterator<T, ITERATOR_TRAITS>::Iterator () noexcept
        : Iterator{default_sentinel}
    {
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>& Iterator<T, ITERATOR_TRAITS>::operator= (const Iterator& rhs)
    {
        if (&rhs != this) [[likely]] {
            fRep_          = rhs.fRep_ == nullptr ? nullptr : Clone_ (*rhs.fRep_);
            _fCurrentValue = rhs._fCurrentValue;
            this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
        }
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline Iterator<T, ITERATOR_TRAITS>& Iterator<T, ITERATOR_TRAITS>::operator= (Iterator&& rhs) noexcept
    {
        if (&rhs != this) [[likely]] {
            fRep_          = move (rhs.fRep_);
            _fCurrentValue = move (rhs._fCurrentValue);
            this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
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
        this->_fCurrentValue = fRep_->Current ();
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline void Iterator<T, ITERATOR_TRAITS>::Invariant () const noexcept
    {
        if constexpr (qStroika_Foundation_Debug_AssertionsChecked) {
            if (fRep_) {
                fRep_->Invariant ();
                // GOOD ASSERT TO ADD BUT DEBUG WHY FAILING
                // Assert ((not this->_fCurrentValue.has_value ()) == fRep_->AtEnd ()); // Iterator<> caches result of rep - make sure in sync
            }
        }
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline const T& Iterator<T, ITERATOR_TRAITS>::Current () const
    {
        RequireNotNull (fRep_);
        Require (_fCurrentValue.has_value ());
        this->Invariant ();
        return *_fCurrentValue;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::AtEnd () const
    {
        this->Invariant ();
        return not _fCurrentValue.has_value ();
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
        Require (not AtEnd ());
        RequireNotNull (fRep_);
        this->Invariant ();
        return *_fCurrentValue;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator->() const -> const value_type*
    {
        Require (not AtEnd ());
        RequireNotNull (fRep_);
        this->Invariant ();
        return _fCurrentValue.operator->();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator++ () -> Iterator&
    {
        Require (not AtEnd ());
        RequireNotNull (fRep_);
        _fCurrentValue = fRep_->More ();
        this->Invariant (); // could do before and after but this is a good cost/benefit trade-off
        return *this;
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::operator++ (int) -> Iterator
    {
        Iterator<T, ITERATOR_TRAITS> result = *this;
        this->operator++ ();
        return result;
    }
    template <typename T, typename ITERATOR_TRAITS>
    Iterator<T, ITERATOR_TRAITS> Iterator<T, ITERATOR_TRAITS>::operator+ (ptrdiff_t i) const
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
        return not AtEnd ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline bool Iterator<T, ITERATOR_TRAITS>::operator== (const Iterator& rhs) const
    {
        /*
         *  Equals is checked by first checking handling the case of special 'AtEnd' iterators. If two
         *  iterators differ on AtEnd () - they cannot be equal. And if they are both AtEnd (this is special -
         *  even if from different sources) they are considered equal.
         *
         *  But then - we check that they are the same dynamic type, and if so, hand to one,
         *  and let it do the dynamic/concrete type specific checks for equality.
         */
        bool lDone = AtEnd ();
        bool rDone = rhs.AtEnd ();
        if (lDone != rDone) [[likely]] {
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
    inline bool Iterator<T, ITERATOR_TRAITS>::operator== (const default_sentinel_t&) const
    {
        return this->AtEnd ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    inline auto Iterator<T, ITERATOR_TRAITS>::Clone_ (const typename Iterator<T, ITERATOR_TRAITS>::IRep& rep) -> unique_ptr<IRep>
    {
        return rep.Clone ();
    }
    template <typename T, typename ITERATOR_TRAITS>
    constexpr inline default_sentinel_t Iterator<T, ITERATOR_TRAITS>::GetEmptyIterator () noexcept
    {
        return default_sentinel;
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
