/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2020.  All rights reserved
 */
#ifndef _Stroika_Foundation_Traversal_Generator_inl_
#define _Stroika_Foundation_Traversal_Generator_inl_

#include "../Debug/Assertions.h"
#include "IterableFromIterator.h"

namespace Stroika::Foundation::Traversal {

    namespace Private_ {

        // @todo - I think we can lose fAtEnd and use the fCur.has_value ()
        template <typename T>
        struct GenItWrapper_ : Iterator<T>::IRep {
            function<optional<T> ()> fFun_;
            optional<T>              fCur_;
            bool                     fAtEnd_{false};
            GenItWrapper_ (function<optional<T> ()> f)
                : fFun_ (f)
            {
                fCur_ = fFun_ ();
                if (not fCur_.has_value ()) {
                    fAtEnd_ = true;
                }
            }
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
                *result = nullopt;
                if (advance) {
                    Require (not fAtEnd_);
                    optional<T> n = fFun_ ();
                    if (not n.has_value ()) {
                        fAtEnd_ = true;
                    }
                    else {
                        fCur_ = n;
                    }
                }
                if (not fAtEnd_) {
                    Assert (fCur_.has_value ());
                    *result = fCur_;
                }
            }
            virtual bool Equals (const typename Iterator<T>::IRep* rhs) const override
            {
                RequireNotNull (rhs);
                // No way to tell equality (some must rethink definition in Iterator<T>::Equals()!!! @todo
                AssertMember (rhs, GenItWrapper_);
                const GenItWrapper_& rrhs = *dynamic_cast<const GenItWrapper_*> (rhs);
                return fAtEnd_ == rrhs.fAtEnd_;
            }
            virtual typename Iterator<T>::RepSmartPtr Clone () const override
            {
                return Iterator<T>::template MakeSmartPtr<GenItWrapper_> (*this);
            }
            virtual IteratorOwnerID GetOwner () const override
            {
                /*
                 *  This return value allows any two DiscreteRange iterators (of the same type) to be compared.
                 */
                return typeid (*this).name ();
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
                    : inherited (context)
                {
                }
                virtual _IterableRepSharedPtr Clone (IteratorOwnerID /*forIterableEnvelope*/) const override
                {
                    // For now - generators have no owner, so we ignore forIterableEnvelope
                    return Iterable<T>::template MakeSmartPtr<MyIterableRep_> (*this);
                }
            };
            MyIterable_ (const function<optional<T> ()>& getNext)
                : Iterable<T> (Iterable<T>::template MakeSmartPtr<MyIterableRep_> (getNext))
            {
            }
        };
        return MyIterable_ (getNext);
    }

}

#endif /* _Stroika_Foundation_Traversal_Generator_inl_ */
