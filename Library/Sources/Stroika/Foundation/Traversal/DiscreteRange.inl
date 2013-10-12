/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
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
                MyIteratorRep_ (T start, T end)
                    : fCur (start)
                    , fEnd (end)
                    , fAtEnd (false) {
                }
                virtual void    More (Memory::Optional<T>* result, bool advance) override {
                    RequireNotNull (result);
                    result->clear ();
                    if (advance) {
                        Require (not fAtEnd);
                        if (fCur == fEnd) {
                            fAtEnd = true;
                        }
                        else {
                            TRAITS::GetNext (&fCur);
                        }
                    }
                    if (not fAtEnd) {
                        *result = fCur;
                    }
                }
                virtual bool    StrongEquals (const typename Iterator<T>::IRep* rhs) const override {
                    RequireNotNull (rhs);
                    AssertNotImplemented ();
                    return false;
                }
                virtual shared_ptr<typename Iterator<T>::IRep>    Clone () const override {
                    AssertNotImplemented ();
                    return nullptr;
                }
            };


            /*
             ********************************************************************************
             ************* DiscreteRange<T, TRAITS>::MyIteratableRep_ ***********************
             ********************************************************************************
             */
            template    <typename T, typename TRAITS>
            struct   DiscreteRange<T, TRAITS>::MyIteratableRep_ : Iterable<T>::_IRep  {
                DECLARE_USE_BLOCK_ALLOCATION(MyIteratableRep_);
                T fStart;
                T fEnd;
                MyIteratableRep_ (T start, T end)
                    : fStart (start)
                    , fEnd (end) {
                }
                virtual typename Iterable<T>::_SharedPtrIRep      Clone () const {
                    return typename Iterable<T>::_SharedPtrIRep(new MyIteratableRep_ (fStart, fEnd));
                }
                virtual Iterator<T>         MakeIterator () const {
                    return Iterator<T> (typename Iterator<T>::SharedIRepPtr (new DiscreteRange<T, TRAITS>::MyIteratorRep_ (fStart, fEnd)));
                }
                virtual size_t              GetLength () const {
                    typedef typename TRAITS::SignedDifferenceType    SignedDifferenceType;
                    return static_cast<SignedDifferenceType> (fEnd) - static_cast<SignedDifferenceType> (fStart);
                }
                virtual bool                IsEmpty () const {
                    return fStart == fEnd;
                }
                virtual void                Apply (typename Iterable<T>::_IRep::_APPLY_ARGTYPE doToElement) const {
                    return this->_Apply (doToElement);
                }
                virtual Iterator<T>         ApplyUntilTrue (typename Iterable<T>::_IRep::_APPLYUNTIL_ARGTYPE doToElement) const {
                    return this->_ApplyUntilTrue (doToElement);
                }
            };


            /*
             ********************************************************************************
             ***************************** DiscreteRange<T> *********************************
             ********************************************************************************
             */
#if 0
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>::DiscreteRange ()
                : Range<T, TRAITS> (TRAITS::kMin, TRAITS::kMax, Openness::eClosed, Openness::eClosed)
                , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (Range<T, TRAITS>::begin (), Range<T, TRAITS>::end ())))
            {
            }
#endif
            template    <typename T, typename TRAITS>
            DiscreteRange<T, TRAITS>::DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                : Range<T, TRAITS> (begin, end, Range<T, TRAITS>::Openness::eClosed, Range<T, TRAITS>::Openness::eClosed)
                , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (Range<T, TRAITS>::begin (), Range<T, TRAITS>::end ())))
            {
            }
            template    <typename T, typename TRAITS>
            inline  DiscreteRange<T, TRAITS>    DiscreteRange<T, TRAITS>::FullRange ()
            {
                return DiscreteRange<T, TRAITS> (TRAITS::kMin, TRAITS::kMax);
            }
            template    <typename T, typename TRAITS>
            inline  bool DiscreteRange<T, TRAITS>::empty () const
            {
                return Range<T, TRAITS>::empty ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterator<T> DiscreteRange<T, TRAITS>::begin () const
            {
                return Iterable<T>::begin ();
            }
            template    <typename T, typename TRAITS>
            inline  Iterator<T> DiscreteRange<T, TRAITS>::end () const
            {
                return Iterable<T>::end ();
            }


        }
    }
}
#endif /* _Stroika_Foundation_Traversal_DiscreteRange_inl_ */
