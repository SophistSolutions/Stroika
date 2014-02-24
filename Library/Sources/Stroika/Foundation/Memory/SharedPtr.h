/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2014.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedPtr_h_
#define _Stroika_Foundation_Memory_SharedPtr_h_ 1

#include    "../StroikaPreComp.h"

#include    <atomic>
#include    <cstdint>

#include    "../Configuration/Common.h"



/**
 *  \file
 *
 *      TODO:
 *
 *      @todo   Add to regrssion test code somethning to assure enable_shared_from_this<> still works. I'm sure
 *              it used to but who knows now!!!
 *
 *      @todo   MASSIVE CLEANUPS SO UPDTATE FOR STROIKA STANDARDS
 *
 *      @todo   Get rid of legacy crap
 *
 *      @todo   MoveCTORs
 *              supported && move operations!
 *
 *      @todo   See if I can transparently add (optional traits) locker, to make it threadsafe.
 *              (at least copying envelope safe)
 *
 *      @todo   CLEAR DOCS!!! - once we have stuff stable...
 *
 *          (o)     CAREFULLY writeup differences between this class and shared_ptr<>
 *                  +   I DONT BELIEVE weak_ptr<T> makes sense, and seems likely to generate bugs in multithreaded
 *                      applications. Maybe I'm missing something. Ask around a bit...
 *                      FOR THE MOST PART.
 *
 *                      There are specific (rare) cases where weak_ptr IS important, and I wnat to find (TODO)
 *                      SOME way to implemnet athat (e.g. PHRDB:: shared DB stuff).
 *
 *          (o)     BETTER DOCUMENT - USE ShaerdPtrBase stuff in other module
 *          (o)     Cleanup documentation, especially about the purpose/point, and how to use.
 *
 */



namespace   Stroika {
    namespace   Foundation {
        namespace   Memory {


            /**
             *  This is like the std::enable_shared_from_this - making your type inherit from it, allows you to recover the
             *  underlying SharedPtr<> given a plain C++ pointer to T.
             */
            template    <typename   T>
            class   enable_shared_from_this;


            namespace   Private_ {
                namespace   SharedPtr_Default_Traits_Helpers_ {
                    /*
                     * Note - though we COULD use a smaller reference count type (e.g. uint32_t - for 64bit machines) -
                     * if we use one smaller than sizeof(void*) we cannot use BlockAllocation<> code -
                     * which currently requires sizeof (T) >= sizeof (void*)
                     */
                    using   ReferenceCountType_ =    uint32_t;
                    // This is used to wrap/combine the shared pointer with the counter.
                    template    <typename   T>
                    class   Envelope_;
                }
                namespace   enable_shared_from_this_Traits_Helpers_ {
                    // size_t of counter should be enough for any reasonable application
                    using   ReferenceCountType_ = uint32_t;
                    // This is used to wrap/combine the shared pointer with the counter.
                    template    <typename   T>
                    class   Envelope_;
                }
            }


            /**
             * Default 'TRAITS' object controlling how SharedPtr<T,T_TRAITS> works. This typically will
             * not be used directly, but just part of using @SharedPtr<T>
             */
            template    <typename   T>
            struct  SharedPtr_Default_Traits {
                using   ReferenceCountType          =   Private_::SharedPtr_Default_Traits_Helpers_::ReferenceCountType_;
                using   Envelope                    =   Private_::SharedPtr_Default_Traits_Helpers_::Envelope_<T>;
            };



            /*
             * SharedPtrFromThis_Traits is the TRAITS object to use with SharedPtr, and T must already
             * inherit from SharedPtrBase.
             *
             *  Example usage:
             *
             *      class   VeryFancyObj : SharedPtrBase {
             *      };
             *
             *      using VeryFancySmartPointer =  SharedPtr<VeryFancyObj,SharedPtrFromThis_TraitsVeryFancyObj>>;
             *
             *      THEN - VeryFancySmartPointer will work like a regular smart pointer - EXCEPT THAT IN ADDITION,
             *      you can ALWAYS safely create a VeryFancySmartPointer from an already existing VeryFancyObj -
             *      just by wrapping/constructing the smart pointer (because the reference count is already in
             *      the base wrapped data type).
             */
            template    <typename   T>
            struct  SharedPtrFromThis_Traits {
                using   ReferenceCountType  =    Private_::enable_shared_from_this_Traits_Helpers_::ReferenceCountType_ ;
                using   Envelope            =    Private_::enable_shared_from_this_Traits_Helpers_::Envelope_<T>;
            };




            /**
            *
            * NEW NOTES:


            * SIMILAR TO std::shared_ptr<> with these exceptions/notes:
            *
            *       >   Doesnt support weak ptr.
            *
            *       >   Emprically appears faster than std::shared_ptr<> (probably due to block allocaiton of envelope and
            *           not suppoprting weak_ptr)
            *
            *       >   SOON will (optionally through template param) support 'lock flag' so can be used automatically threadsafe copies.
            *           (STILL MSUST THINK THROUGH IF MAKES SENSE)
            *
            *
            @CLASS:         SharedPtr<T,T_TRAITS>
            @DESCRIPTION:
                    <p>This class is for keeping track of a data structure with reference counts,
                and disposing of that structure when the reference count drops to zero.
                Copying one of these Shared<T> just increments the referce count,
                and destroying/overwriting one decrements it.
                </p>

                    <p>You can have a ptr having a nullptr value, and it can be copied.
                (Implementation detail - the reference count itself is NEVER nil except upon
                failure of alloction of memory in ctor and then only valid op on class is
                destruction). You can access the value with GetPointer () but this is not
                advised - only if it may be legitimately nullptr do you want to do this.
                Generaly just use ptr-> to access the data, and this will do the
                RequireNotNull (POINTER) for you.
                </p>

                    <p>This class can be enourmously useful in implementing letter/envelope -
                type data structures - see String, or Shapes, for examples.
                </p>

                    <p>Example Usage</p>

                <code>
                    {
                        SharedPtr<int>  p (new int ());
                        *p = 3;
                        // 'when 'p' goes out of scope - the int will be automatically deleted
                    }
                </code>

                    <p>SharedPtr<T> is a simple utility class - very much akin to the C++11 class
                std::shared_ptr<T>. SharedPtr<T> contains the following basic differences:

                    <li>There is no std::weak_ptr - or at least if there is - we must document it clearly
                        how/why via extra sharedPTR tmeplate arg(to be worked out)</li>
                    <li>There is an extra template T_TRAITS that allows for solving special problems that
                        come up with shared_ptr<> - namely recovering the
                        'shared' version of 'T' when only given a plain copy of 'T'
                    </li>

                    Otherwise, the intention is that they should operate very similarly, and SharedPtr<T>
                    should work with most classes that expect shared_ptr<T> (so long
                    as they are templated, and not looking for the particular type name 'shared_ptr').

                    <p>TODO: CHECK EXACT API DIFFERENCES WITH shared_ptr - BUT - they should be reasonably small -
                    neglecting the weak_ptr stuff.</p>

                    <p>See also @SharedPtrBase module for how to do much FANCIER SharedPtr<> usage</p>

            */
            template    <typename   T, typename T_TRAITS = SharedPtr_Default_Traits<T>>
            class   SharedPtr {
            public:
                using   element_type    =   T;

            public:
                SharedPtr () noexcept;
                explicit SharedPtr (T* from);
                explicit SharedPtr (const typename T_TRAITS::Envelope& from);
                SharedPtr (const SharedPtr<T, T_TRAITS>& from);
                SharedPtr (SharedPtr<T, T_TRAITS>&& from);

                template <typename T2, typename T2_TRAITS>
                /*
                @METHOD:        SharedPtr::SharedPtr
                @DESCRIPTION:   <p>This CTOR is meant to allow for the semantics of assigning a sub-type pointer to a pointer
                            to the base class. There isn't any way to express this in template requirements, but this template
                            will fail to compile (error assigning to its fPtr_ member in the CTOR) if its ever used to
                            assign inappropriate pointer combinations.</p>
                */
                SharedPtr (const SharedPtr<T2, T2_TRAITS>& from);

            public:
                nonvirtual      SharedPtr<T, T_TRAITS>& operator= (const SharedPtr<T, T_TRAITS>& rhs);

            public:
                ~SharedPtr ();

            public:
                nonvirtual  bool        IsNull () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::GetRep
                @DESCRIPTION:   <p>Requires that the pointer is non-nullptr. You can call SharedPtr<T,T_TRAITS>::get ()
                    which whill return null without asserting if the pointer is allowed to be null.</p>
                */
                nonvirtual  T&          GetRep () const;

            public:
                /**
                 *  \em Note - this CAN NOT return nullptr (because -> semantics are typically invalid for a logically null pointer)
                 */
                nonvirtual  T* operator-> () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::operator*
                @DESCRIPTION:   <p></p>
                */
                nonvirtual  T& operator* () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::operator T*
                @DESCRIPTION:   <p>Note - this CAN return nullptr</p>
                */
                nonvirtual  operator T* () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::get
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
                            asserts about it being non-null.</p>
                */
                nonvirtual  T*      get () const noexcept;
            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::release
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer nullptr, but first return the
                            pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
                            <br>
                            NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
                            If you want the pointer before release, explicitly call get () first!!!
                            </p>
                */
                nonvirtual  void    release ();

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::clear
                @DESCRIPTION:   <p>Synonymn for SharedPtr<T,T_TRAITS>::release ()
                            </p>
                */
                nonvirtual  void    clear ();

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::reset
                @DESCRIPTION:   <p>Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
                            pre-existing pointer value. Unreference any previous value. Note - if there were more than one references
                            to the underlying object, its not destroyed.</p>
                */
                nonvirtual  void    reset (T* p = nullptr);

            public:
                template <typename T2>
                /*
                @METHOD:        SharedPtr::Dynamic_Cast
                @DESCRIPTION:   <p>Similar to SharedPtr<T2> () CTOR - which does base type. NB couldn't call this dynamic_cast -
                            thats a reserved word.</p>
                */
                nonvirtual  SharedPtr<T2>   Dynamic_Cast () const;

            public:
                // Returns true iff reference count of owned pointer is 1 (false if 0 or > 1)
                nonvirtual  bool    IsUnique () const;

            public:
                // Alias for IsUnique()
                nonvirtual  bool    unique () const;

            public:
                /*
                @METHOD:        SharedPtr<T,T_TRAITS>::CurrentRefCount
                @DESCRIPTION:   <p>I used to keep this available only for debugging, but I've found a few
                cases where its handy outside the debugging context so not its awlays available (it has
                no cost to keep available).</p>
                */
                typename T_TRAITS::ReferenceCountType   CurrentRefCount () const;

            public:
                // Alias for CurrentRefCount()
                typename T_TRAITS::ReferenceCountType   use_count () const;

            public:
                nonvirtual  bool    operator< (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator<= (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator> (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator>= (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator== (const SharedPtr<T, T_TRAITS>& rhs) const;
                nonvirtual  bool    operator!= (const SharedPtr<T, T_TRAITS>& rhs) const;

            public:
                nonvirtual  const typename T_TRAITS::Envelope& PeekAtEnvelope () const;

            private:
                typename    T_TRAITS::Envelope  fEnvelope_;

            private:
                template    <typename T2, typename T2_TRAITS>
                friend  class   SharedPtr;
            };



///DOCS OUT OF DATE - UPDATE -
            // An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
            // from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
            // This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
            // a layer of code, and then re-constitute the SharedPtr<> part later.

            /*
             * To enable the shared_from_this () functionality - and allow recovery of the SharedPtr<T> from the T* itself, its necessary to
             * combine the T type with the SharedPtr<T> infrastructure.
             *
             * To use, just inherit your type from enable_shared_from_this<>:
             *
             *      struct  TTT : Memory::enable_shared_from_this<TTT> {
             *          string x;
             *      };
             *      using TTT_SP =  SharedPtr<TTT,SharedPtrFromThis_Traits<TTT>> ;
             *
             *
             */
            template    <typename   T>
            class   enable_shared_from_this {
            private:
                atomic<Private_::enable_shared_from_this_Traits_Helpers_::ReferenceCountType_>   fCount_;

            public:
                enable_shared_from_this ();
                virtual ~enable_shared_from_this ();

            public:
                template    <typename   RESULT_TRAITS = SharedPtrFromThis_Traits<T>>
                SharedPtr<T, RESULT_TRAITS> shared_from_this ();

            private:
                friend  class   Private_::enable_shared_from_this_Traits_Helpers_::Envelope_<T>;
            };


            /*
             * SO FAR FAILED - attempts at getting partial specialization to work. Reason todo that is so that
             *
             * then SharedPtr<T> will be the same as SharedPtr<T,SharedPtrFromThis_Traits<T>>
             */
#if     0
            template    <typename   T>
            struct  SharedPtr_Default_Traits<enable_shared_from_this<T>> :  SharedPtrFromThis_Traits<T> {
            };
#endif


        }


        namespace   Execution {
            template    <typename   T>
            void    ThrowIfNull (const Memory::SharedPtr<T>& p);
            template    <typename   T, typename T_TRAITS>
            void    ThrowIfNull (const Memory::SharedPtr<T, T_TRAITS>& p);
        }

    }
}
namespace   std {
    /**
     *  overload the std::dynamic_pointer_cast to work with Stroika SharedPtr<> as well.
     */
    template    <class TO_TYPE_T,   class FROM_TYPE_T>
    Stroika::Foundation::Memory::SharedPtr<TO_TYPE_T>   dynamic_pointer_cast (const Stroika::Foundation::Memory::SharedPtr<FROM_TYPE_T>& sp) noexcept;
}
#endif  /*_Stroika_Foundation_Memory_SharedPtr_h_*/



/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include    "SharedPtr.inl"
