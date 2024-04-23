/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2024.  All rights reserved
 */
#include "IterableFromIterator.h"
#include "Stroika/Foundation/Debug/Assertions.h"
#include "Stroika/Foundation/Debug/Cast.h"
#include "Stroika/Foundation/Memory/BlockAllocated.h"

namespace Stroika::Foundation::Traversal {

    /**
     */
    template <typename T>
    Iterator<T> CreateGeneratorIterator (const function<optional<T> ()>& getNext)
    {
        struct GenItWrapper_ : Iterator<T>::IRep, public Memory::UseBlockAllocationIfAppropriate<GenItWrapper_> {
            function<optional<T> ()> fFun_;
            optional<T>              fCur_;
            GenItWrapper_ () = delete;
            GenItWrapper_ (const function<optional<T> ()>& f)
                : fFun_{f}
                , fCur_{fFun_ ()}
            {
            }
            virtual void More (optional<T>* result, bool advance) override
            {
                RequireNotNull (result);
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
            virtual unique_ptr<typename Iterator<T>::IRep> Clone () const override
            {
                return make_unique<GenItWrapper_> (*this);
            }
        };
        return Iterator<T>{make_unique<GenItWrapper_> (getNext)};
    }

    /**
     */
    template <typename T>
    inline Iterable<T> CreateGenerator (const function<optional<T> ()>& getNext)
    {
        return MakeIterableFromIterator (CreateGeneratorIterator (getNext));
    }

}
