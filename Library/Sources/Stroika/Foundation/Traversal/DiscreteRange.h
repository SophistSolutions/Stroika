/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_DiscreteRange_h_
#define _Stroika_Foundation_Traversal_DiscreteRange_h_  1

#include    "../StroikaPreComp.h"

#include    "Iterable.h"
#include    "Range.h"



/**
 *  \file
 *
 *  STATUS:     PRELIMINARY DRAFT.
 *
 *  TODO:
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Traversal {


            /**
             */
            template    <typename T>
            struct  DefaultDiscreteRangeTraits : DefaultRangeTraits<T> {
                // needed for iterator - return false if no more
                static bool GetNext (T* n);
            };


            /**
             *  EXAMPLE USAGE:
             *      vector<int> v = DiscreteRange (1,10).As<vector<int>> ();
             *      // equiv to vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
             *
             *  OR:
             *      for (auto i : DiscreteRange (1,10)) {
             *          ...i = 1..10
             *      }
             */
            template    <typename T, typename TRAITS = DefaultDiscreteRangeTraits<T>>
            class  DiscreteRange : public Range<T, TRAITS>, public Iterable<T> {
                // must shadow methods that return Range in base class, and provide CTOR

                //
            public:
                struct   MyIteratorRep_ : Iterator<T>::IRep  {
                    T fCur;
                    T fEnd;
                    MyIteratorRep_ (T start, T end)
                        : fCur (start)
                        , fEnd (end) {
                    }
                    virtual void    More (Memory::Optional<T>* result, bool advance) override {
                        RequireNotNull (result);
                        result->clear ();
                        if (advance) {
                            fCur++;     // use traits
                        }
                        if (fCur <= fEnd) {// use traits
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
                struct   MyIteratableRep_ : Iterable<T>::_IRep  {
                    T fStart;
                    T fEnd;
                    MyIteratableRep_ (T start, T end)
                        : fStart (start)
                        , fEnd (end) {
                    }
                    virtual _SharedPtrIRep      Clone () const {
                        return _SharedPtrIRep(new MyIteratableRep_ (fStart, fEnd));
                    }
                    virtual Iterator<T>         MakeIterator () const {
                        return Iterator<T> (Iterator<T>::SharedIRepPtr (new DiscreteRange<T, TRAITS>::MyIteratorRep_ (fStart, fEnd)));
                    }
                    virtual size_t              GetLength () const {
                        // off by one???
                        return fEnd - fStart;
                    }
                    virtual bool                IsEmpty () const {
                        return _IsEmpty ();
                    }
                    virtual void                Apply (_APPLY_ARGTYPE doToElement) const {
                        return _Apply (doToElement);
                    }
                    virtual Iterator<T>         ApplyUntilTrue (_APPLYUNTIL_ARGTYPE doToElement) const {
                        return _ApplyUntilTrue (doToElement);
                    }
                };

            public:
                /**
                 */
                DiscreteRange ()
                    : Range<T, TRAITS> ()
                    , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (Range<T, TRAITS>::begin (), Range<T, TRAITS>::end ()))) {
                }
                explicit DiscreteRange (const Memory::Optional<T>& begin, const Memory::Optional<T>& end)
                    : Range<T, TRAITS> (begin, end)
                    , Iterable<T> (typename Iterable<T>::_SharedPtrIRep (new MyIteratableRep_ (Range<T, TRAITS>::begin (), Range<T, TRAITS>::end ()))) {
                }

            public:
                bool empty () const { return Range<T, TRAITS>::empty(); }

            public:
                nonvirtual  Iterator<T> begin () const {
                    return Iterable<T>::begin ();
                }
                nonvirtual  Iterator<T> end () const {
                    return Iterable<T>::end ();
                }
            public:
                // returns same as begin
                T   GetFirst () const {
                    return Range<T, TRAITS>::begin ();
                }

            public:
                // returns one less than end!!! - the last valid value
                T   GetLast () const {
                    return Range<T, TRAITS>::end () - 1;
                }
            };


        }
    }
}



/*
 ********************************************************************************
 ******************************* Implementation Details *************************
 ********************************************************************************
 */
#include    "DiscreteRange.inl"

#endif  /*_Stroika_Foundation_Traversal_DiscreteRange_h_ */
