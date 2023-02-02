/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
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
            : fCur{TRAITS::kLowerBound}
            , fEnd{TRAITS::kLowerBound}
            , fAtEnd{true}
        {
        }
        MyIteratorRep_ (T start, T end)
            : fCur{start}
            , fEnd{end}
            , fAtEnd{false}
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
        virtual bool Equals ([[maybe_unused]] const typename Iterator<T>::IRep* rhs) const override
        {
            RequireNotNull (rhs);
            AssertNotImplemented ();
            return false;
        }
        virtual unique_ptr<typename Iterator<T>::IRep> Clone () const override
        {
            unique_ptr<MyIteratorRep_> tmp = make_unique<MyIteratorRep_> (fCur, fEnd);
            tmp->fAtEnd                    = fAtEnd;
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
            T    fStart;
            T    fEnd;
            bool fForcedEnd;
            MyRep_ ()
                : fStart{TRAITS::kLowerBound}
                , fEnd{TRAITS::kLowerBound}
                , fForcedEnd{true}
            {
            }
            MyRep_ (T start, T end)
                : fStart{start}
                , fEnd{end}
                , fForcedEnd{false}
            {
            }
            virtual _IterableRepSharedPtr Clone () const override { return Memory::MakeSharedPtr<MyRep_> (*this); }
            virtual Iterator<T>           MakeIterator ([[maybe_unused]] const _IterableRepSharedPtr& thisSharedPtr) const override
            {
                if (fForcedEnd) {
                    return Iterator<T>{make_unique<DiscreteRange::MyIteratorRep_> ()};
                }
                else {
                    return Iterator<T>{make_unique<DiscreteRange::MyIteratorRep_> (fStart, fEnd)};
                }
            }
            virtual size_t size () const override
            {
                using SignedDifferenceType = typename TRAITS::SignedDifferenceType;
                if (fForcedEnd) {
                    return TRAITS::DifferenceToSizeT (SignedDifferenceType{0});
                }
                else {
                    return TRAITS::DifferenceToSizeT (SignedDifferenceType{1} + DiscreteRange{fStart, fEnd}.GetDistanceSpanned ());
                }
            }
            virtual bool empty () const override
            {
                if (fForcedEnd) {
                    return true;
                }
                else {
                    return false;
                    //return fStart == fEnd;
                }
            }
            virtual void Apply (const function<void (ArgByValueType<value_type> item)>& doToElement) const override
            {
                this->_Apply (doToElement);
            }
            virtual Iterator<T> Find (const _IterableRepSharedPtr& thisSharedPtr, const function<bool (ArgByValueType<value_type> item)>& that) const override
            {
                return this->_Find (thisSharedPtr, that);
            }
            virtual Iterator<value_type> Find_equal_to (const _IterableRepSharedPtr& thisSharedPtr, const ArgByValueType<value_type>& v) const override
            {
                return this->_Find_equal_to_default_implementation (thisSharedPtr, v);
            }
        };
        MyIterable_ ()
            : Iterable<T>{Memory::MakeSharedPtr<MyRep_> ()}
        {
        }
        MyIterable_ (T start, T end)
            : Iterable<T>{Memory::MakeSharedPtr<MyRep_> (start, end)}
        {
        }
    };

    /*
     ********************************************************************************
     ***************************** DiscreteRange<T> *********************************
     ********************************************************************************
     */
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS>::DiscreteRange (T begin, T end)
        : inherited{begin, end}
    {
        Require (begin <= end);
    }
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS>::DiscreteRange (const optional<T>& begin, const optional<T>& end)
        : inherited{begin, end}
    {
    }
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS>::DiscreteRange (const Range<T, TRAITS>& r)
    {
        // Could do more efficiently
        if (not r.empty ()) {
            Require (r.GetLowerBoundOpenness () == Openness::eClosed);
            Require (r.GetUpperBoundOpenness () == Openness::eClosed);
            *this = DiscreteRange{r.GetLowerBound (), r.GetUpperBound ()};
        }
    }
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::FullRange ()
    {
        return DiscreteRange{TRAITS::kLowerBound, TRAITS::kUpperBound};
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> DiscreteRange<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
    {
        return inherited::Intersection (rhs);
    }
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::Intersection (const DiscreteRange& rhs) const
    {
        return DiscreteRange{inherited::Intersection (rhs)};
    }
    template <typename T, typename TRAITS>
    constexpr Range<T, TRAITS> DiscreteRange<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
    {
        return inherited::UnionBounds (rhs);
    }
    template <typename T, typename TRAITS>
    constexpr DiscreteRange<T, TRAITS> DiscreteRange<T, TRAITS>::UnionBounds (const DiscreteRange& rhs) const
    {
        auto r = inherited::UnionBounds (rhs);
        return DiscreteRange{r.GetLowerBound (), r.GetUpperBound ()};
    }
    template <typename T, typename TRAITS>
    constexpr typename DiscreteRange<T, TRAITS>::UnsignedDifferenceType DiscreteRange<T, TRAITS>::GetNumberOfContainedPoints () const
    {
        if (this->empty ()) {
            return 0;
        }
        else {
            return this->GetDistanceSpanned () + 1;
        }
    }
    template <typename T, typename TRAITS>
    constexpr auto DiscreteRange<T, TRAITS>::Offset (SignedDifferenceType o) const -> DiscreteRange
    {
        Require (not this->empty ());
        return inherited::Offset (o);
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
            return Iterator<T>{make_unique<MyIteratorRep_> (this->GetLowerBound (), this->GetUpperBound ())};
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
