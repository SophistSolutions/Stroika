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
 *  \file
 *
 *  TODO:
 *      @todo   Cleanup ifdefed out code
 */

namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  Default traits object for type T (for use in Optional<T>). This can generally be ignored.
             *  It requires operator== and operator< are defined for type T (if they are not, you must
             *  provide your own traits to use Optional<T>).
             */
            template    <typename T>
            struct   Optional_DefaultTraits {
                static  int Compare (T lhs, T rhs);
            };


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
             *  runs the risk of producing surprising exceptions, we treat dereferencing an empty
             *  Optional<T> as an Assertion Erorr.
             *
             *  \note   \em Thread-Safety
             *      Different instances of Optional<T> can be freely used  in different threads,
             *      but a given instance can only be safely used (read + write, or write + write)
             *      from a single thread at a time.
             *
             *  \note   \em Design-Note
             *      We considered using the SharedByValue<T> template which would be more efficient when we copy
             *      Optional objects of type T, where T is expensive to copy. But for T where T is cheap to copy,
             *      just using BlockAllocated<T> is probably cheaper to copy. Which is more common? Who knows, so this
             *      probably doesn't matter much. Also - consider perhaps if there is a way to use a TRAITS argument?
             *      If we chose to use SharedByValue<T>, then non-const methods like 'operator* ()' would just
             *      'BreakReferences'.
             *
             *  \note   \em Design-Note
             *      operator T();
             *
             *      We considered having an operator T () method. This has advantages, in that it makes more seemless
             *      replacing use of a T with an Optional<T>. But it has the disadvantage that, when coupled with
             *      the Optional<T> (T) CTOR, you can get overloading problems.
             *
             *      Plus, one must carefully check each use of variable o of type T, being converted to type
             *      Optional<T>, so being forced to say "*" first isn't totally unreasonable.
             *
             *
             *
             *  @todo   DOCUMENT FACT THAT WE NEVER RETURN INTERNAL POINTER EXCEPT CONST, and THEN ONLY VALID TEXT NEXT METHOD
             *          CALL ON OPTIONAL.
             */
            template    <typename T, typename TRAITS = Optional_DefaultTraits<T> >
            class   Optional {
            public:
                Optional ();
                Optional (const T& from);
                Optional (T && from);
                Optional (const Optional<T, TRAITS>& from);
                Optional (Optional<T, TRAITS> && from);
            public:
                ~Optional ();

            public:
                nonvirtual  Optional<T, TRAITS>& operator= (const T& from);
                nonvirtual  Optional<T, TRAITS>& operator= (T && from);
                nonvirtual  Optional<T, TRAITS>& operator= (const Optional<T, TRAITS>& from);
                nonvirtual  Optional<T, TRAITS>& operator= (Optional<T, TRAITS> && from);

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
                /**
                 *  Always safe to call. If empty, returns argument 'default' or sentinal value.
                 */
                nonvirtual  T   Value (T defaultValue = T ()) const;


            public:
                /*
                 * Unclear if we want a non-const version too?
                 * Returns nullptr if value is missing
                 *
                 *  \note   \em Warning
                 *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T> instance.
                 */
                nonvirtual  const T*    get () const;

            public:
#if 0
                /**
                 *  \pre (not empty ())
                 *
                 *  \note   \em Warning
                 *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T> instance.
                 */
                nonvirtual  const T* operator-> () const;

                /**
                 *  \pre (not empty ())
                 *
                 *  \note   \em Warning
                 *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T> instance.
                 */
                nonvirtual  T* operator-> ();
#endif

                /**
                 *  \pre (not empty ())
                 *
                 *  \note   \em Warning
                 *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T> instance.
                 */
                nonvirtual  const T& operator* () const;

#if 0
                /**
                 *  \pre (not empty ())
                 *
                 *  \note   \em Warning
                 *      This method returns a pointer internal to (owned by) Optional<T>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T> instance.
                 */
                nonvirtual  T& operator* ();
#endif

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 *  Somewhat arbitrarily, treat NOT-PROVIDED (empty) as < any value of T
                 */
                nonvirtual  int Compare (const Optional<T>& rhs) const;

            public:
                nonvirtual  bool    operator< (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator<= (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator> (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator>= (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator== (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Optional<T, TRAITS>& rhs) const;

            private:
                /*
                 *  Carefully manage the storage ourselves - not doing shared copy. No shared copy (copyonwrite) because these
                 *  are rarely copied, and this is cheaper (since can use block-allocation - even if type T not
                 *  block allocated) - and no extra count infrastructure, or threadafe locking.
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
