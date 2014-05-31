/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Containers_Optional_h_
#define _Stroika_Foundation_Containers_Optional_h_  1

#include    "../StroikaPreComp.h"

#include    "../Memory/Optional.h"



/**
 *  \file
 *
 *  \version    <a href="code_status.html#Alpha">Alpha</a>
 *
 *  TODO:
 *      @todo
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Containers {


            /**
             *  Default traits object for type T (for use in Optional<T, TRAITS>). This can generally be ignored.
             *  It requires operator== and operator< are defined for type T (if they are not, you must
             *  provide your own traits to use Optional<T, TRAITS>).
             */
            template    <typename T>
            using   Optional_DefaultTraits = Memory::Optional_DefaultTraits<T>;


            /**
             *  Optional<T, TRAITS> can be used to store an object which may or may not be present. This can be
             *  used in place of sentinal values (for example if no obvious sentinal value presents itself),
             *  and instead of explicitly using pointers and checking for null all over.
             *
             *  This is a thread-safe, but to be threadsafe, slightly less functioanl variant of
             *  Memory::Optional<>
             *
             *  \note   To use Optional with un-copyable things, use:
             *          Optional<NotCopyable>   n2 (std::move (NotCopyable ()));    // use r-value reference to move
             *
             *  \note   C++14 will be introucing std::optional<> which may possibly make this obsolete.
             *          We'll see.
             *
             *  \note   \em Design-Note - why no operator T()
             *      -   We considered having an operator T () method. This has advantages, in that
             *          it makes more seemless replacing use of a T with an Optional<T, TRAITS>. But it has
             *          the disadvantage that, when coupled with the Optional<T, TRAITS> (T) CTOR, you can
             *          get overloading problems.
             *      -   Plus, one must carefully check each use of a variable of type T, being converted
             *          to type Optional<T, TRAITS>, so being forced to say "*" first isn't totally unreasonable.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#Automatically-Synchronized-Thread-Safety">Automatically-Synchronized-Thread-Safety</a>
             */
            template    <typename T, typename TRAITS = Optional_DefaultTraits<T>>
            class   Optional {
            public:
                /**
                 */
                Optional ();
                Optional (const T& from);
                Optional (T&&  from);
                Optional (const Optional<T, TRAITS>& from);
                Optional (Optional<T, TRAITS>&& from);
                Optional (const Memory::Optional<T, TRAITS>& from);

            public:
                /**
                 */
                nonvirtual  Optional<T, TRAITS>& operator= (const T& rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (T && rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (const Optional<T, TRAITS>& rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (Optional<T, TRAITS> && rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (const Memory::Optional<T, TRAITS>& rhs);

            public:
                /**
                 *  Erases (destroys) any present value for this Optional<T, TRAITS> instance. After calling clear (),
                 *  the IsMissing () will return true.
                 */
                nonvirtual  void    clear ();

            public:
                /**
                *  Returns true iff the Optional<T, TRAITS> has no valid value. Attempts to access the value of
                *  an Optional<T, TRAITS> (eg. through operator* ()) will result in an assertion error.
                */
                nonvirtual  bool    IsMissing () const; // means no value (it is optional!)

            public:
                /**
                 *  Returns true iff the Optional<T, TRAITS> has a valid value ( not empty ());
                 */
                nonvirtual  bool    IsPresent () const;

#if 0
            public:
                /**
                 *  Unsure if I want this?
                 */
                nonvirtual  operator bool () const noexcept
                {
                    return IsPresent ();
                }
#endif

            public:
                /**
                 *  Always safe to call. If IsMissing, returns argument 'default' or 'sentinal' value.
                 */
                nonvirtual  T   Value (T defaultValue = T {}) const;

            public:
                /**
                 *  \pre (IsPresent ())
                 *
                 *  \warning
                 *      This method returns a pointer internal to (owned by) Optional<T, TRAITS>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T, TRAITS> instance.
                 */
                nonvirtual  T   operator* () const;

            public:
                /**
                 *  \req (IsPresent ())
                 *
                 *  Each of these methods (+=, -=, *=, /= are defined iff the underlying operator is defined on T.
                 */
                nonvirtual  Optional<T, TRAITS>&    operator+= (const T& rhs);
                nonvirtual  Optional<T, TRAITS>&    operator-= (const T& rhs);
                nonvirtual  Optional<T, TRAITS>&    operator*= (const T& rhs);
                nonvirtual  Optional<T, TRAITS>&    operator/= (const T& rhs);

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 *  Somewhat arbitrarily, treat NOT-PROVIDED (empty) as < any value of T
                 */
                nonvirtual  int Compare (const Optional<T, TRAITS>& rhs) const;

            public:
                /**
                 *  Basic operator overloads with the obivous meaning, and simply indirect to @Compare (const Optional<T, TRAITS>& rhs)
                 */
                nonvirtual  bool    operator< (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator<= (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator> (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator>= (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator== (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const Optional<T, TRAITS>& rhs) const;

            private:
                mutex               fMutex_;

            private:
                Memory::Optional<T> fValue_;
            };


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Optional.inl"

#endif  /*_Stroika_Foundation_Containers_Optional_h_*/
