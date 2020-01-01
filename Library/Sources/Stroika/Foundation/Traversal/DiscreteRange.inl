/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_inl_
#define _Stroika_Foundation_Traversal_DiscreteRange_inl_

#include "../Debug/Assertions.h"
#include "../Memory/BlockAllocated.h"

namespace Stroika::Foundation::Traversal {

    /*
     ********************************************************************************
     ***************** DiscreteRange<T, TRAITS>::MyIteratorRep_ *********************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    struct DiscreteRange<T, TRAITS>::MyIteratorRep_ : Iterator<T>::IRep, public Memory::UseBlockAllocationIfAppropriate<MyIteratorRep_> {
        using inherited = typename Iterator<T>::IRep;
        T    fCur;
        T    fEnd;
        bool fAtEnd;
        MyIteratorRep_ ()
            : fCur (TRAITS::kLowerBound)
            , fEnd (TRAITS::kLowerBound)
            , fAtEnd (true)
        {
        }
        MyIteratorRep_ (T start, T end)
            : fCur (start)
            , fEnd (end)
            , fAtEnd (false)
        {
        }
        virtual void More (optional<T>* result, bool advance) override
        {
            RequireNotNull (result);
            *result = nullopt;
            if (advance) {
                Require (not fAtEnd);
                if (fCur == fEnd) {
                    fAtEnd = true;
                }
                else {
                    fCur = TRAITS::GetNext (fCur);
                }
            }
            if (not fAtEnd) {
                *result = fCur;
            }
        }
        virtual IteratorOwnerID GetOwner () const override
        {
            /*
                *  This return value allows any two DiscreteRange iterators (of the same type) to be compared.
                */
            return typeid (*this).name ();
        }
        virtual bool Equals ([[maybe_unused]] const typename Iterator<T>::IRep* rhs) const override
        {
            RequireNotNull (rhs);
            AssertNotImplemented ();
            return false;
        }
        virtual typename inherited::RepSmartPtr Clone () const override
        {
            Traversal::IteratorBase::PtrImplementationTemplate<MyIteratorRep_> tmp{new MyIteratorRep_ (fCur, fEnd)};
            tmp->fAtEnd = fAtEnd;
            return tmp;
        }
    };

    /*
     ********************************************************************************
     ***************** DiscreteRange<T, TRAITS>::MyIterable_ ************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    struct DiscreteRange<T, TRAITS>::MyIterable_ : Iterable<T> {
        struct MyRep_ : Iterable<T>::_IRep, public Memory::UseBlockAllocationIfAppropriate<MyRep_> {
            using inherited             = typename Iterable<T>::_IRep;
            using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
            using _APPLY_ARGTYPE        = typename Iterable<T>::_IRep::_APPLY_ARGTYPE;
            using _APPLYUNTIL_ARGTYPE   = typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE;
            T    fStart;
            T    fEnd;
            bool fForcedEnd;
            MyRep_ ()
                : fStart (TRAITS::kLowerBound)
                , fEnd (TRAITS::kLowerBound)
                , fForcedEnd (true)
            {
            }
            MyRep_ (T start, T end)
                : fStart (start)
                , fEnd (end)
                , fForcedEnd (false)
            {
            }
            virtual _IterableRepSharedPtr Clone ([[maybe_unused]] IteratorOwnerID forIterableEnvelope) const
            {
                // DiscreteRange doesn't track specific 'envelope' owner
                return Iterable<T>::template MakeSmartPtr<MyRep_> (*this);
            }
            virtual Iterator<T> MakeIterator ([[maybe_unused]] IteratorOwnerID suggestedOwner) const
            {
                // DiscreteRange doesn't track specific 'envelope' owner
                if (fForcedEnd) {
                    return Iterator<T> (Iterator<T>::template MakeSmartPtr<DiscreteRange<T, TRAITS>::MyIteratorRep_> ());
                }
                else {
                    return Iterator<T> (Iterator<T>::template MakeSmartPtr<DiscreteRange<T, TRAITS>::MyIteratorRep_> (fStart, fEnd));
                }
            }
            virtual size_t GetLength () const
            {
                using SignedDifferenceType = typename TRAITS::SignedDifferenceType;
                if (fForcedEnd) {
                    return static_cast<SignedDifferenceType> (0);
                }
                else {
                    return 1 + DiscreteRange<T, TRAITS>{fStart, fEnd}.GetDistanceSpanned ();
                }
            }
            virtual bool IsEmpty () const
            {
                if (fForcedEnd) {
                    return true;
                }
                else {
                    return false;
                    //return fStart == fEnd;
                }
            }
            virtual void Apply (_APPLY_ARGTYPE doToElement) const
            {
                this->_Apply (doToElement);
            }
            virtual Iterator<T> FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
            {
                return this->_FindFirstThat (doToElement, suggestedOwner);
            }
        };
        MyIterable_ ()
            : Iterable<T> (Iterable<T>::template MakeSmartPtr<MyRep_> ())
        {
        }
        MyIterable_ (T start, T end)
            : Iterable<T> (Iterable<T>::template MakeSmartPtr<MyRep_> (start, end))
        {
        }
    };

    /*
     ********************************************************************************
     ***************************** DiscreteRange<T> *********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS>::DiscreteRange (T begin, T end)
        : inherited (begin, end)
    {
        Require (begin <= end);
    }
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS>::DiscreteRange (const optional<T>& begin, const optional<T>& end)
        : inherited (begin, end)
    {
    }
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS>::DiscreteRange (const Range<T, typename TRAITS::RangeTraitsType>& r)
        : inherited ()
    {
        // Could do more efficiently
        if (not r.empty ()) {
            *this = DiscreteRange (r.GetLowerBound (), r.GetUpperBound ());
        }
    }
    template <typename T, typename TRAITS>
    inline DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::FullRange ()
    {
        return DiscreteRange<T, TRAITS> (TRAITS::kLowerBound, TRAITS::kUpperBound);
    }
    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> DiscreteRange<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
    {
        return inherited::Intersection (rhs);
    }
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::Intersection (const DiscreteRange<T, TRAITS>& rhs) const
    {
        return DiscreteRange<T, TRAITS> (inherited::Intersection (rhs));
    }
    template <typename T, typename TRAITS>
    inline Range<T, TRAITS> DiscreteRange<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
    {
        return inherited::UnionBounds (rhs);
    }
    template <typename T, typename TRAITS>
    DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::UnionBounds (const DiscreteRange<T, TRAITS>& rhs) const
    {
        auto r = inherited::UnionBounds (rhs);
        return DiscreteRange<T, TRAITS> (r.GetLowerBound (), r.GetUpperBound ());
    }
    template <typename T, typename TRAITS>
    typename DiscreteRange<T, TRAITS>::UnsignedDifferenceType DiscreteRange<T, TRAITS>::GetNumberOfContainedPoints () const
    {
        if (this->empty ()) {
            return 0;
        }
        else {
            return this->GetDistanceSpanned () + 1;
        }
    }
    template <typename T, typename TRAITS>
    inline auto DiscreteRange<T, TRAITS>::Offset (SignedDifferenceType o) const -> DiscreteRange
    {
        Require (not this->empty ());
        return DiscreteRange (inherited::Offset (o));
    }
    template <typename T, typename TRAITS>
    Iterable<T> DiscreteRange<T, TRAITS>::Elements () const
    {
        return this->empty () ? MyIterable_ () : MyIterable_ (this->GetLowerBound (), this->GetUpperBound ());
    }
    template <typename T, typename TRAITS>
    inline DiscreteRange<T, TRAITS>::operator Iterable<T> () const
    {
        return Elements ();
    }
    template <typename T, typename TRAITS>
    Iterator<T> DiscreteRange<T, TRAITS>::begin () const
    {
        if (this->empty ()) {
            return Iterator<T>::GetEmptyIterator ();
        }
        else {
            return Iterator<T> (Iterator<T>::template MakeSmartPtr<MyIteratorRep_> (this->GetLowerBound (), this->GetUpperBound ()));
        }
    }
    template <typename T, typename TRAITS>
    inline Iterator<T> DiscreteRange<T, TRAITS>::end () const
    {
        return Iterator<T>::GetEmptyIterator ();
    }

    /*
     ********************************************************************************
     *********************************** operator^ **********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    inline DiscreteRange<T, TRAITS> operator^ (const DiscreteRange<T, TRAITS>& lhs, const DiscreteRange<T, TRAITS>& rhs)
    {
        return lhs.Intersection (rhs);
    }

}

#endif /* _Stroika_Foundation_Traversal_DiscreteRange_inl_ */
