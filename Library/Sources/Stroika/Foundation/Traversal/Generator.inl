/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2021.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_inl_
#define _Stroika_Foundation_Traversal_Generator_inl_

#include "../Debug/Assertions.h"
#include "../Debug/Cast.h"
#include "IterableFromIterator.h"

namespace Stroika::Foundation::Traversal {

    namespace Private_ {

        template <typename T>
        struct GenItWrapper_ : Iterator<T>::IRep {
            function<optional<T> ()> fFun_;
            optional<T>              fCur_;
            bool                     fSuppressMore_{false}; // needed cuz we need to be able to answer 'isdone' before advance begins
            GenItWrapper_ () = delete;
            GenItWrapper_ (const function<optional<T> ()>& f)
                : fFun_{f}
                , fCur_{fFun_ ()}
            {
            }
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                *result = nullopt;
                if (advance) {
                    // Iterator<T, ITERATOR_TRAITS>::IRep::More() docs say legal to call More(advance) even if at end
                    if (fCur_.has_value ()) {
                        fCur_ = fFun_ ();
                    }
                }
                *result = fCur_;
            }
            virtual bool Equals (const typename Iterator<T>::IRep* rhs) const override
            {
                RequireNotNull (rhs);
                const GenItWrapper_& rrhs = *Debug::UncheckedDynamicCast<const GenItWrapper_*> (rhs);
                // No way to tell equality (so must rethink definition in Iterator<T>::Equals()!!! @todo
                WeakAssert (not fCur_.has_value () or not rrhs.fCur_.has_value ());
                return fCur_.has_value () == rrhs.fCur_.has_value ();
            }
            virtual typename Iterator<T>::RepSmartPtr Clone () const override
            {
                return Iterator<T>::template MakeSmartPtr<GenItWrapper_> (*this);
            }
        };
    }

    /**
     */
    template <typename T>
    Iterator<T> CreateGeneratorIterator (const function<optional<T> ()>& getNext)
    {
        return Iterator<T> (Iterator<T>::template MakeSmartPtr<Private_::GenItWrapper_<T>> (getNext));
    }

    /**
     */
    template <typename T>
    Iterable<T> CreateGenerator (const function<optional<T> ()>& getNext)
    {
        struct MyIterable_ : Iterable<T> {
            using MyContextData_ = function<optional<T> ()>;
            using MyIteratorRep_ = typename Private_::GenItWrapper_<T>;
            struct MyIterableRep_ : IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep, public Memory::UseBlockAllocationIfAppropriate<MyIterableRep_> {
                using inherited             = typename IterableFromIterator<T, MyIteratorRep_, MyContextData_>::_Rep;
                using _IterableRepSharedPtr = typename Iterable<T>::_IterableRepSharedPtr;
                MyIterableRep_ (const MyContextData_& context)
                    : inherited{context}
                {
                }
                virtual _IterableRepSharedPtr Clone () const override
                {
                    return Iterable<T>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const function<optional<T> ()>& getNext)
                // Use Iterable<>() to avoid matching Iterable<>(initializer_list<>... - see docs in Iterable::CTORs...
                : Iterable<T> (Iterable<T>::template MakeSmartPtr<MyIterableRep_> (getNext))
            {
            }
        };
        return MyIterable_{getNext};
    }

}

#endif /* _Stroika_Foundation_Traversal_Generator_inl_ */
