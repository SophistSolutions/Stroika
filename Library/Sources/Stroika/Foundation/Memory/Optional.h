/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2015.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_Optional_h_
#define _Stroika_Foundation_Memory_Optional_h_  1

#include    "../StroikaPreComp.h"

#include    "../Common/Compare.h"
#include    "../Configuration/Common.h"
#include    "../Execution/SpinLock.h"
#include    "../Execution/Synchronized.h"


#define qUseDirectlyEmbeddedDataInOptionalBackEndImpl_  0
#if !qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
#include    "BlockAllocated.h"
#endif


/**
 *  \file
 *
 *  \version    <a href="code_status.html#Beta">Beta</a>
 *
 *  TODO:
 *
 *
 *      @todo   FIX operator<, etc to match what we did for operator== and operator!=, and document!!!
 *              since COMPARE is part of traits we do NOT want to allow compare with differnt traits (so MUST FIX EUQalas as well)
 *
 *      @todo   COULD make operator==, operator<, etc compares more efficeint when comparing with T
 *              by adding a slew (3x) more overloads. (DID TO SOME EXTENT BUT COULD IMPROVE); And could improve iterop
 *              by adding operator XXX (where XXX is ==, etc) with (T, OPTIONAL);
 *
 *      @todo   Condsider (maybe test) if implemenation using member buffer Byte buf[sizeof(T)]; with appropriate alignof stuff -
 *              would perform better than BlockAllocated? It's important that this class be low-cost, low-overhead!
 *
 *              One trick is must be careful with copy-semantics. But almost certainly would perform better (just cuz no
 *              lock needed with blockallocated).
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  Default traits object for type T (for use in Optional<T, TRAITS>). This can generally be ignored.
             *  It requires operator== and operator< are defined for type T (if they are not, you must
             *  provide your own traits to use Optional<T, TRAITS>).
             */
            template    <typename T>
            struct   Optional_DefaultTraits {
                static  int     Compare (T lhs, T rhs);
                static  bool    Equals (T lhs, T rhs);
            };


            /**
             *  Optional<T, TRAITS> can be used to store an object which may or may not be present. This can be
             *  used in place of sentinal values (for example if no obvious sentinal value presents itself),
             *  and instead of explicitly using pointers and checking for null all over.
             *
             *  When dereferencing an 'IsMissing' value, there are three plausible interpretions:
             *      -   Return the default value T()
             *      -   throw bad_alloc()
             *      -   Assertion error
             *
             *  Because the 'default value' isn't always well defined, and because throwing bad_alloc
             *  runs the risk of producing surprising exceptions, we treat dereferencing an 'IsMissing'
             *  Optional<T, TRAITS> as an Assertion Erorr.
             *
             *  However, see @ref Value()
             *
             *  \note   To use Optional with un-copyable things, use:
             *          Optional<NotCopyable>   n2 (std::move (NotCopyable ()));    // use r-value reference to move
             *
             *  \note   After C++14, C++ may be introducing std::optional<> which may possibly make this
             *          obsolete. We'll see.
             *
             *  \note   \em Design-Note - why not SharedByValue<T>
             *      -   We considered using the SharedByValue<T> template which would be more efficient
             *          when we copy Optional objects of type T, where T is expensive to copy. But for
             *          T where T is cheap to copy, just using BlockAllocated<T> is probably cheaper
             *          to copy. Which is more common? Who knows, so this probably doesn't matter much.
             *          Also - consider perhaps if there is a way to use a TRAITS argument?
             *          If we chose to use SharedByValue<T>, then non-const methods like 'operator* ()'
             *          would just 'BreakReferences'.
             *
             *  \note   \em Design-Note - Internal pointers
             *      -   Several APIs return internal pointers - both const and non-const. Because of this
             *          it up to callers to assure thread-safety. The lifetime of the returned pointers
             *          is gauranteed only until the start of the next call on a the particular
             *          Optional<> instance.
             *      -   Why we allow internal pointers to be returned from
             *          get (), operator-> (), and operator* ().
             *          -   GIST is this class is intrinsically not thread safe anyhow. Main reason
             *              to not allow that stuff is to assure thread safety and controlled update.
             *          -   Allowing returning const ptrs is just as unsafe and returning non-const pointers
             *              cuz another thread could still modify that data.
             *          -   And returning non-const pointers very confenenti when doing optional of a struct,
             *              and allowing people to modify parts of the struct.
             *
             *              \code
             *              Optional<some_object> o;
             *              ...
             *              o->x = 1;   // assert fails if o not set first
             *              o->y = 2;
             *              return o->z;
             *              \endcode
             *
             *  \note   \em Design-Note - why no operator T()
             *      -   We considered having an operator T () method. This has advantages, in that
             *          it makes more seemless replacing use of a T with an Optional<T, TRAITS>. But it has
             *          the disadvantage that, when coupled with the Optional<T, TRAITS> (T) CTOR, you can
             *          get overloading problems.
             *      -   Plus, one must carefully check each use of a variable of type T, being converted
             *          to type Optional<T, TRAITS>, so being forced to say "*" first isn't totally unreasonable.
             *
             *  \note   \em Thread-Safety   <a href="thread_safety.html#POD-Level-Thread-Safety">POD-Level-Thread-Safety</a>
             *          It would have been impractical to make Optional<T, TRAITS> fully thread-safe, due to its returning
             *          of internal pointers.
             *
             *  \note   \em Design-Note - Optional<void>
             *          I considered supporting Optional<void>, but there seemed no natural way to 'assign' something to it
             *          to make it present. I found this might be useful, for situations like:
             *              struct VariantFoo {
             *                  Optional<infoa> a;
             *                  Optional<extraInfoB> b;
             *                  Optional<void>  c;
             *              };
             *              where VariantFoo was basically a variant union, where the variant present
             *              was the one where the optional was there. But
             *              this can be done - I think - just as easily using 'bool c' for the variants with
             *              no extra data.
             */
            template    <typename T, typename TRAITS = Optional_DefaultTraits<T>>
            class   Optional {
            public:
                /**
                 *  Variant taking const T* is exerpimental. Idea is arg can be null, and means missing, and if non null,
                 *  derefrence and copy. Experiemental cuz could mess up type deducation of Optional<T>...
                 */
#if     !qCompilerAndStdLib_constexpr_Buggy
                constexpr
#endif
                Optional ();
                Optional (const T& from);
                Optional (T&&  from);
                Optional (const Optional<T, TRAITS>& from);
                Optional (Optional<T, TRAITS>&& from);
                explicit Optional (const T* from);

            public:
                ~Optional ();

            public:
                /**
                 */
                nonvirtual  Optional<T, TRAITS>& operator= (const T& rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (T && rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (const Optional<T, TRAITS>& rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (Optional<T, TRAITS> && rhs);
                nonvirtual  Optional<T, TRAITS>& operator= (const T* rhs);

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
                nonvirtual  constexpr   bool    IsMissing () const noexcept; // means no value (it is optional!)

            public:
                /**
                 *  Returns true iff the Optional<T, TRAITS> has a valid value ( not IsMissing ());
                 */
                nonvirtual  constexpr   bool    IsPresent () const noexcept;

            public:
                /**
                 *  Return true iff IsPresent ().
                 *
                 *  @todo   Unsure if I want this?
                 */
                nonvirtual  explicit operator bool () const noexcept;

            public:
                /**
                 *  Always safe to call. If IsMissing, returns argument 'default' or 'sentinal' value.
                 *
                 *  @see AssignIf
                 *  @see CheckedValue
                 */
                nonvirtual  T   Value (T defaultValue = T {}) const;

            public:
                /**
                 *  Always safe to call. If IsMissing, throws argument exception2ThrowIfMissing.
                 *
                 *  @see AssignIf
                 *  @see Value
                 */
                template    <typename   THROW_IF_MISSING_TYPE>
                nonvirtual  T   CheckedValue (const THROW_IF_MISSING_TYPE& exception2ThrowIfMissing = THROW_IF_MISSING_TYPE ()) const;

            public:
                /**
                 *  Assign the value held by this optional if one is present.
                 *
                 *  The point of this to to faciltate a common idiom, where you want to maintain an existing value unless you
                 *  get an update. This function is ANALAGOUS to
                 *      if (o.IsPresent()) {
                 *          destVal = *o;
                 *      }
                 *
                 *  but can be done in a single line.
                 *
                 *  EXAMPLE-USE:
                 *      int curValue = 3;
                 *      Optional<long>  oVal = someMap.Lookup (KEY_VALUE);
                 *      oVal.AssignIf (&curValue);
                 *
                 *  @see Value
                 */
                template    <typename   CONVERTABLE_TO_TYPE = T>
                nonvirtual  void    AssignIf (CONVERTABLE_TO_TYPE* to) const;

            public:
                /**
                 *  Returns nullptr if value is missing
                 *
                 *  \warning
                 *      This method returns a pointer internal to (owned by) Optional<T, TRAITS>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T, TRAITS> instance.
                 */
                nonvirtual  T*          get ();
                nonvirtual  const T*    get () const;

            public:
                /**
                 *  \pre (IsPresent ())
                 *
                 *  \warning
                 *      This method returns a pointer internal to (owned by) Optional<T, TRAITS>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T, TRAITS> instance.
                 *
                 *  This is really just syntactic sugar equivalent to get () - except that it requires
                 *  not-null - but more convenient since it allows the use of an optional to
                 *  syntactically mirror dereferencing a pointer.
                 */
                nonvirtual  T* operator-> ();
                nonvirtual  const T* operator-> () const;

            public:
                /**
                 *  \pre (IsPresent ())
                 *
                 *  \warning
                 *      This method returns a pointer internal to (owned by) Optional<T, TRAITS>, and its lifetime
                 *      is only guaranteed until the next method call on this Optional<T, TRAITS> instance.
                 */
                nonvirtual  T& operator* ();
                nonvirtual  const T& operator* () const;

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
                 *  Return true if *this logically equals rhs. Note if either side 'IsMissing'
                 *  is different, then they compare as not Equals()
                 */
                nonvirtual  bool    Equals (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  bool    Equals (T rhs) const;

            public:
                /**
                 *  Return < 0 if *this < rhs, return 0 if equal, and return > 0 if *this > rhs.
                 *  Somewhat arbitrarily, treat NOT-PROVIDED (IsMissing) as < any value of T
                 */
                nonvirtual  int Compare (const Optional<T, TRAITS>& rhs) const;
                nonvirtual  int Compare (T rhs) const;

            private:
                /*
                 *  Carefully manage the storage ourselves - not doing shared copy. No shared
                 *  copy (copyonwrite) because these are rarely copied, and this is cheaper
                 *  (since can use block-allocation - even if type T not block allocated) -
                 *  and no extra count infrastructure, or threadafe locking.
                 */
#if     qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
                Memory::Byte fBuffer_[10000/*sizeof(T)*/];  // intentionally uninitialized
                T*  fValue_ = nullptr;
#else
                AutomaticallyBlockAllocated<T>*  fValue_ = nullptr;
#endif

#if qUseDirectlyEmbeddedDataInOptionalBackEndImpl_
            private:
                static  void    destroy_ (T* p);
#endif
            };


            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Compare (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator< (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            bool    operator< (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs);

            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Compare (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator<= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            bool    operator<= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs);

            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Equals (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator== (const Optional<T, TRAITS>& lhs, T rhs);
            template    <typename T, typename TRAITS>
            bool    operator== (T lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS>
            bool    operator== (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<T, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
            bool    operator== (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs);
#if 0
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename X1 = typename enable_if<is_convertible<RHS_CONVERTABLE_TO_OPTIONAL, Optional<T, TRAITS>>::value >::type>
            bool    operator== (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return lhs.Equals (Optional<T, TRAITS> (rhs));
            }
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<T, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
            bool    operator== (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return lhs.Equals (Optional<T, TRAITS> (rhs));
            }
#endif




            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Equals (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator!= (const Optional<T, TRAITS>& lhs, T rhs);
            template    <typename T, typename TRAITS>
            bool    operator!= (T lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS>
            bool    operator!= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<Optional<T, TRAITS>, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
            bool    operator!= (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs);

#if 0
            // @TODO GET THIS WORKING
            template    <typename T, typename TRAITS, typename RHS_CONVERTABLE_TO_OPTIONAL, typename SFINAE_CHECK = typename enable_if<is_constructible<Optional<T, TRAITS>, RHS_CONVERTABLE_TO_OPTIONAL>::value >::type>
            bool    operator!= (const Optional<T, TRAITS>& lhs, RHS_CONVERTABLE_TO_OPTIONAL rhs)
            {
                return not lhs.Equals (Optional<T, TRAITS> { rhs });
            }
#endif


            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Compare (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator>= (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            bool    operator>= (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs);

            /**
             *  Simple overloaded operator which calls @Optional<T,TRAITS>::Compare (const Optional<T, TRAITS>& rhs)
             */
            template    <typename T, typename TRAITS>
            bool    operator> (const Optional<T, TRAITS>& lhs, const Optional<T, TRAITS>& rhs);
            template    <typename T, typename TRAITS, typename   RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS>
            bool    operator> (const Optional<T, TRAITS>& lhs, const RHS_CONVERTIBLE_TO_OPTIONAL_OF_T_TRAITS& rhs);


        }
    }
}



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "Optional.inl"

#endif  /*_Stroika_Foundation_Memory_Optional_h_*/
