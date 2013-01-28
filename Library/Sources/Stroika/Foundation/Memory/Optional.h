/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2013.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_  1

#include    "../StroikaPreComp.h"

#include    <memory>

#include    "../Configuration/Common.h"
#include    "BlockAllocated.h"


/**
 * TODO:
 *
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  Optional<T> can be used to store an object which may or may not be present. This can be
             *  used in place of sentinal values (for example if no obvious sentinal value presents itself),
             *  and instead of explicitly using pointers and checking for null all over.
             *
             *  When dereferencing an empty value, there are three plausible interpretions:
             *      (o)     Return the default value T()
             *      (o)     throw bad_alloc()
             *      (o)     Assertion error
             *
             *  Because the 'default value' isn't always well defined, and because throwing bad_alloc
             *  runs the risk of producing surprising expceitons (based on experience),
             *  we treat dereferencing an empty Optional<T> as an Assertion Erorr.
             *
             *  Thread-Safety: Different instances of Optional<T> can be freely used  in different threads,
             *  but a given instance can only be safely used (read + write, or write+write) from a single thread
             *  at a time.
             */
            template    <typename T>
            class   Optional {
            public:
                Optional ();
                Optional (const T& from);
                Optional (T && from);
                Optional (const Optional<T>& from);
                Optional (Optional<T> && from);
            public:
                ~Optional ();

            public:
                nonvirtual  Optional<T>& operator= (const T& from);
                nonvirtual  Optional<T>& operator= (T && from);
                nonvirtual  Optional<T>& operator= (const Optional<T>& from);
                nonvirtual  Optional<T>& operator= (Optional<T> && from);

            public:
                /**
                 *  Erases (destroys) any present value for this Optional<T> instance. After calling clear (),
                 *  the empty () will return true.
                 */
                nonvirtual  void    clear ();
            public:
                /**
                 * Returns true iff the Optional<T> has no valid value. Attempts to access the value of
                 * an Optional<T> (eg. through operator T ()) will result in an error.
                 */
                nonvirtual  bool    empty () const; // means no value (it is optional!)

            public:
                // Unclear if we want a non-const version too?
                // Returns nullptr if value is missing
                nonvirtual  const T*    get () const;

            public:
                /** Require (not empty ())
                 *
                 * Not clear its a good idea to define this. It causes problems with the mixture of CTOR(T)
                 * and operator T - creating ambiguity. However, it appears this ambiguity can always be
                 * replaced by inserting a '*' in front of the 'Optional' element to disambiguate, so
                 * I think it maybe OK
                 *      -- LGP 2012-05-20
                 */
                nonvirtual  operator T () const;

            public:
                // Require (not empty ())
                nonvirtual  const T* operator-> () const;
                // Require (not empty ())
                nonvirtual  T* operator-> ();
                // Require (not empty ())
                nonvirtual  const T& operator* () const;
                // Require (not empty ())
                nonvirtual  T& operator* ();

                // Somewhat arbitrarily, treat NOT-PROVIDED (empty) as < any value of T
            public:
                nonvirtual  bool    operator< (const Optional<T>& rhs) const;
                nonvirtual  bool    operator<= (const Optional<T>& rhs) const;
                nonvirtual  bool    operator> (const Optional<T>& rhs) const;
                nonvirtual  bool    operator>= (const Optional<T>& rhs) const;
                nonvirtual  bool    operator== (const Optional<T>& rhs) const;
                nonvirtual  bool    operator!= (const Optional<T>& rhs) const;

            private:
                /*
                 *  Note: the implementation here is safe via copying - because we never WRITE to the
                 *  value stored in the shared_ptr<T> and never return a copy to someone who could,
                 *  so the value can never change. One changes an Optional<T> by creating a whole new
                 *  value object and assigning it.
                 */
                BlockAllocated<T>*  fValue_;
            };
        }
    }
}
#endif  /*_Stroika_Foundation_Memory_Optional_h_*/





/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Optional.inl"
