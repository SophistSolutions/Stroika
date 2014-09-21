/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_inl_
#define _Stroika_Foundation_Traversal_DiscreteRange_inl_

#include    "../Debug/Assertions.h"
#include    "../Memory/BlockAllocated.h"



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /*
             ********************************************************************************
             ***************** DiscreteRange<T, TRAITS>::MyIteratorRep_ *********************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            struct   DiscreteRange<T, TRAITS>::MyIteratorRep_ : Iterator<T>::IRep  {
                DECLARE_USE_BLOCK_ALLOCATION(MyIteratorRep_);
                T fCur;
                T fEnd;
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
                virtual void    More (Memory::Optional<T>* result, bool advance) override
                {
                    RequireNotNull (result);
                    result->clear ();
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
                virtual bool    Equals (const typename Iterator<T>::IRep* rhs) const override
                {
                    RequireNotNull (rhs);
                    AssertNotImplemented ();
                    return false;
                }
                virtual shared_ptr<typename Iterator<T>::IRep>    Clone () const override
                {
                    shared_ptr<MyIteratorRep_>  tmp = shared_ptr<MyIteratorRep_> (new MyIteratorRep_ (fCur, fEnd));
                    tmp->fAtEnd = fAtEnd;
                    return tmp;
                }
            };


            /*
             ********************************************************************************
             ************* DiscreteRange<T, TRAITS>::MyIteratableRep_ ***********************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            struct   DiscreteRange<T, TRAITS>::MyIteratableRep_ : Iterable<T>::_IRep {
                using   inherited = typename Iterable<T>::_IRep;
                using   _SharedPtrIRep = typename Iterable<T>::_SharedPtrIRep;
                using   _APPLY_ARGTYPE = typename Iterable<T>::_IRep::_APPLY_ARGTYPE;
                using   _APPLYUNTIL_ARGTYPE = typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE;
                DECLARE_USE_BLOCK_ALLOCATION (MyIteratableRep_);
                T       fStart;
                T       fEnd;
                bool    fForcedEnd;
                MyIteratableRep_ ()
                    : fStart (TRAITS::kLowerBound)
                    , fEnd (TRAITS::kLowerBound)
                    , fForcedEnd (true)
                {
                }
                MyIteratableRep_ (T start, T end)
                    : fStart (start)
                    , fEnd (end)
                    , fForcedEnd (false)
                {
                }
                virtual _SharedPtrIRep      Clone (IteratorOwnerID forIterableEnvelope) const
                {
                    // DiscreteRange doesnt track specific 'envelope' owner
                    return _SharedPtrIRep (new MyIteratableRep_ (*this));
                }
                virtual Iterator<T>         MakeIterator (IteratorOwnerID suggestedOwner) const
                {
                    // DiscreteRange doesnt track specific 'envelope' owner
                    if (fForcedEnd) {
                        return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new DiscreteRange<T, TRAITS>::MyIteratorRep_ ()));
                    }
                    else {
                        return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new DiscreteRange<T, TRAITS>::MyIteratorRep_ (fStart, fEnd)));
                    }
                }
                virtual size_t              GetLength () const
                {
                    using   SignedDifferenceType        =   typename TRAITS::SignedDifferenceType;
                    if (fForcedEnd) {
                        return static_cast<SignedDifferenceType> (0);
                    }
                    else {
                        return 1 + static_cast<SignedDifferenceType> (fEnd) - static_cast<SignedDifferenceType> (fStart);
                    }
                }
                virtual bool                IsEmpty () const
                {
                    if (fForcedEnd) {
                        return true;
                    }
                    else {
                        return false;
                        //return fStart == fEnd;
                    }
                }
                virtual void                Apply (_APPLY_ARGTYPE doToElement) const
                {
                    this->_Apply (doToElement);
                }
                virtual Iterator<T>         FindFirstThat (_APPLYUNTIL_ARGTYPE doToElement, IteratorOwnerID suggestedOwner) const
                {
                    return this->_FindFirstThat (doToElement, suggestedOwner);
                }
            };


            /*
             ********************************************************************************
             ***************************** DiscreteRange<T> *********************************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>::DiscreteRange ()
                : inherited_RangeType ()
                , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ ()))
            {
            }
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>::DiscreteRange (T begin, T end)
                : inherited_RangeType (begin, end)
                , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (begin, end)))
            {
            }
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>::DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : inherited_RangeType (begin, end)
                , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (this->GetLowerBound (), this->GetUpperBound ())))
            {
            }
            template    <typename T, typename TRAITS>
            inline  DiscreteRange<T, TRAITS>    DiscreteRange<T, TRAITS>::FullRange ()
            {
                return DiscreteRange<T, TRAITS> (TRAITS::kLowerBound, TRAITS::kUpperBound);
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    DiscreteRange<T, TRAITS>::Intersection (const Range<T, TRAITS>& rhs) const
            {
                return inherited_RangeType::Intersection (rhs);
            }
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>    DiscreteRange<T, TRAITS>::Intersection (const DiscreteRange<T, TRAITS>& rhs) const
            {
                auto    r   =   inherited_RangeType::Intersection (rhs);
                return DiscreteRange<T, TRAITS> (r.GetLowerBound (), r.GetUpperBound ());
            }
            template    <typename T, typename TRAITS>
            inline  Range<T, TRAITS>    DiscreteRange<T, TRAITS>::UnionBounds (const Range<T, TRAITS>& rhs) const
            {
                return inherited_RangeType::UnionBounds (rhs);
            }
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>    DiscreteRange<T, TRAITS>::UnionBounds (const DiscreteRange<T, TRAITS>& rhs) const
            {
                auto    r   =   inherited_RangeType::UnionBounds (rhs);
                return DiscreteRange<T, TRAITS> (r.GetLowerBound (), r.GetUpperBound ());
            }
            template    <typename T, typename TRAITS>
            typename DiscreteRange<T, TRAITS>::UnsignedDifferenceType DiscreteRange<T, TRAITS>::GetNumberOfContainedPoints () const
            {
                if (empty ()) {
                    return 0;
                }
                else {
                    return this->GetDistancespanned () + 1;
                }
            }
            template    <typename T, typename TRAITS>
            inline  bool DiscreteRange<T, TRAITS>::empty () const
            {
                Ensure (inherited_RangeType::empty () == Iterable<T>::empty ());
                return inherited_RangeType::empty ();
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DiscreteRange_inl_ */
