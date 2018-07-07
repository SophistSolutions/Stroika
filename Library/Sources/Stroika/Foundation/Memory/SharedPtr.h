/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2018.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_SharedPtr_h_
#define _Stroika_Foundation_Memory_SharedPtr_h_ 1

#include "../StroikaPreComp.h"

#include <atomic>
#include <cstdint>
#include <type_traits>

#include "../Configuration/Common.h"

/**
 *  \file
 *
 *  TODO:
 *      @todo   REDO operator== etc using non-member functions
 *              (see See coding conventions document about operator usage: Compare () and operator<, operator>, etc comments)
 *
 *      @todo   I THINK should be able todo SharedPtr::Envolpe<> template specialization for
 *              enabled_shared_from_this, which stores and copies only ONE POINTER> That could
 *              make for much faster atomic copies etc (maybe losing the spinlock).
 *
 *              Not totally sure how do this safely, but add to list of things todo cuz it
 *              would probably greatly improvement many important benchmarks, like the
 *              “Test Simple Struct With Strings Filling And Copying”
 *
 *      @todo   See if fDeleteCounter_ can be somehow inferred from other data to save space/copying. It's hard cuz
 *              of the multiple inheritence case (so comparing counter == fPtr not exactly right always).
 *
 *      @todo   See if I can transparently add (optional traits) locker, to make it threadsafe.
 *              (at least copying envelope safe)
 *
 *      @todo   CLEAR DOCS!!! - once we have stuff stable...
 *          o       CAREFULLY writeup differences between this class and shared_ptr<>
 *                  +   I DONT BELIEVE weak_ptr<T> makes sense, and seems likely to generate bugs in multithreaded
 *                      applications. Maybe I'm missing something. Ask around a bit...
 *                      FOR THE MOST PART.
 *
 *                      There are specific (rare) cases where weak_ptr IS important, and I wnat to find (TODO)
 *                      SOME way to implemnet athat (e.g. PHRDB:: shared DB stuff).
 *
 *          o       BETTER DOCUMENT - USE ShaerdPtrBase stuff in other module
 *          o       Cleanup documentation, especially about the purpose/point, and how to use.
 */

namespace Stroika::Foundation {
    namespace Memory {

        /**
         *  Common defines for all SharedPtr<T> templates. Probably not a good idea to use this directly (impl detail and subject to change).
         */
        struct SharedPtrBase {
            /**
             * Note - though we COULD use a smaller reference count type (e.g. uint32_t - for 64bit machines).
             */
            using ReferenceCountType = unsigned int;
        };

        namespace Private_ {
            // This is used to wrap/combine the shared pointer with the counter.
            struct ReferenceCounterContainerType_ {
                atomic<SharedPtrBase::ReferenceCountType> fCount;
                bool                                      fDeleteCounter_;
                ReferenceCounterContainerType_ ();
                ReferenceCounterContainerType_ (const ReferenceCounterContainerType_&) = delete;
                ReferenceCounterContainerType_ (bool deleteCounter);
                ReferenceCounterContainerType_& operator= (const ReferenceCounterContainerType_&) = delete;
            };
        }

        template <typename T>
        class enable_shared_from_this;

        namespace Private_ {
            template <typename T>
            class Envelope_;
        }

        /**
         *
         * SIMILAR TO std::shared_ptr<> with these exceptions/notes:
         *
         *       >  Doesnt support weak ptr.
         *
         *       >  Emprically appears faster than std::shared_ptr<> (probably due to block
         *          allocation of envelope and not suppoprting weak_ptr)
         *
         *  This class is for keeping track of a data structure with reference counts,
         *  and disposing of that structure when the reference count drops to zero.
         *  Copying one of these Shared<T> just increments the referce count,
         *  and destroying/overwriting one decrements it.
         *
         *  You can have a ptr having a nullptr value, and it can be copied.
         *  (Implementation detail - the reference count itself is NEVER nil except upon
         *  failure of alloction of memory in ctor and then only valid op on class is
         *  destruction). You can access the value with GetPointer () but this is not
         *  advised - only if it may be legitimately nullptr do you want to do this.
         *  Generaly just use ptr-> to access the data, and this will do the
         *  RequireNotNull (POINTER) for you.
         *
         *         This class can be enourmously useful in implementing letter/envelope -
         *    type data structures - see String, or Shapes, for examples.
         *
         *
         *  Example Usage
         *  <code>
         *      {
         *          SharedPtr<int>  p (new int ());
         *          *p = 3;
         *          // 'when 'p' goes out of scope - the int will be automatically deleted
         *      }
         *  </code>
         *
         *  SharedPtr<T> is a simple utility class - very much akin to the C++11 class
         *  std::shared_ptr<T>. SharedPtr<T> contains the following basic differences:
         *
         *  <li>
         *      There is no std::weak_ptr - or at least if there is - we must document it clearly
         *      how/why via extra sharedPTR tmeplate arg(to be worked out)
         *  </li>
         *  <li>
         *      There is an extra template T_TRAITS that allows for solving special problems that
         *      come up with shared_ptr<> - namely recovering the
         *      'shared' version of 'T' when only given a plain copy of 'T'
         *  </li>
         *
         *  Otherwise, the intention is that they should operate very similarly, and SharedPtr<T>
         *  should work with most classes that expect shared_ptr<T> (so long
         *  as they are templated, and not looking for the particular type name 'shared_ptr').
         *
         *  TODO: CHECK EXACT API DIFFERENCES WITH shared_ptr - BUT - they should be reasonably small -
         *  neglecting the weak_ptr stuff
         *
         *  @see    @SharedPtrBase module for how to do much FANCIER SharedPtr<> usage
         */
        template <typename T>
        class SharedPtr : public SharedPtrBase {
        public:
            using element_type = T;

        private:
            using Envelope_ = Private_::Envelope_<T>;

        public:
            /**
             */
            SharedPtr () noexcept;
            SharedPtr (nullptr_t) noexcept;
            template <typename T2, typename SFINAE = enable_if_t<is_convertible<T2*, T*>::value, void>>
            explicit SharedPtr (T2* from);
            SharedPtr (const SharedPtr<T>& from) noexcept;
            SharedPtr (SharedPtr<T>&& from) noexcept;
            template <typename T2, typename SFINAE = enable_if_t<is_convertible<T2*, T*>::value, void>>
            SharedPtr (const SharedPtr<T2>& from) noexcept;
            template <typename T2, typename SFINAE = enable_if_t<is_convertible<T2*, T*>::value, void>>
            SharedPtr (SharedPtr<T2>&& from) noexcept;

        private:
            explicit SharedPtr (const Envelope_& from) noexcept;

        private:
            template <typename T2>
            static Envelope_ mkEnvelope_ (T2* from, enable_if_t<is_convertible<T2*, Private_::ReferenceCounterContainerType_*>::value>* = 0);
            template <typename T2>
            static Envelope_ mkEnvelope_ (T2* from, enable_if_t<!is_convertible<T2*, Private_::ReferenceCounterContainerType_*>::value>* = 0);

        public:
            nonvirtual SharedPtr<T>& operator= (const SharedPtr<T>& rhs) noexcept;
            nonvirtual SharedPtr<T>& operator= (SharedPtr<T>&& rhs) noexcept;

        public:
            ~SharedPtr ();

        public:
            /**
                 */
            nonvirtual bool IsNull () const noexcept;

        public:
            /**
             *  Requires that the pointer is non-nullptr. You can call SharedPtr<T,T_TRAITS>::get ()
             *  which whill return null without asserting if the pointer is allowed to be null.</p>
             */
            nonvirtual T& GetRep () const noexcept;

        public:
            /**
             *  \em Note - this CAN NOT return nullptr (because -> semantics are typically invalid for a logically null pointer)
             */
            nonvirtual T* operator-> () const noexcept;

        public:
            /**
             */
            nonvirtual T& operator* () const noexcept;

        public:
            /**
             *  Note - this CAN return nullptr
             */
            nonvirtual operator T* () const noexcept;

        public:
            /**
             *  Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Just return the pointed to object, with no
             *  asserts about it being non-null.</p>
             */
            nonvirtual T* get () const noexcept;

        public:
            /**
             *  Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer nullptr, but first return the
             *  pre-existing pointer value. Note - if there were more than one references to the underlying object, its not destroyed.
             *
             *  NO - Changed API to NOT return old pointer, since COULD have been destroyed, and leads to buggy coding.
             *  If you want the pointer before release, explicitly call get () first!!!
             */
            nonvirtual void release () noexcept;

        public:
            /**
             *  Synonymn for SharedPtr<T,T_TRAITS>::release ()
             */
            nonvirtual void clear () noexcept;

        public:
            /**
             *  Mimic the 'get' API of the std::auto_ptr&lt;T&gt; class. Make this pointer 'p', but first return the
             *  pre-existing pointer value. Unreference any previous value. Note - if there were more than one references
             *  to the underlying object, its not destroyed.
             */
            nonvirtual void reset (T* p = nullptr);

        public:
            /**
             *  Similar to SharedPtr<T2> () CTOR - which does base type. NB couldn't call this dynamic_cast -
             *  thats a reserved word.
             *
             *  NOTE - THIS RETURNS NULLPTR NOT THROWING - if dynamic_cast<> fails - that is pointer dynamoic_cast not reference
             */
            template <typename T2>
            nonvirtual SharedPtr<T2> Dynamic_Cast () const noexcept;

        public:
            /**
             */
            nonvirtual void swap (SharedPtr<T>& rhs);

        public:
            /**
             *  Returns true iff reference count of owned pointer is 1 (false if 0 or > 1)
             */
            nonvirtual bool IsUnique () const noexcept;

        public:
            /**
             *  Alias for IsUnique()
             */
            nonvirtual bool unique () const noexcept;

        public:
            /**
             *  I used to keep this available only for debugging, but I've found a few
             *  cases where its handy outside the debugging context so not its awlays available (it has
             *  no cost to keep available).</p>
             */
            nonvirtual ReferenceCountType CurrentRefCount () const noexcept;

        public:
            /**
             * Alias for CurrentRefCount()
             */
            nonvirtual ReferenceCountType use_count () const noexcept;

        public:
            nonvirtual bool operator< (const SharedPtr<T>& rhs) const noexcept;
            nonvirtual bool operator<= (const SharedPtr<T>& rhs) const noexcept;
            nonvirtual bool operator> (const SharedPtr<T>& rhs) const noexcept;
            nonvirtual bool operator>= (const SharedPtr<T>& rhs) const noexcept;
            nonvirtual bool operator== (const SharedPtr<T>& rhs) const noexcept;
            nonvirtual bool operator!= (const SharedPtr<T>& rhs) const noexcept;

        public:
            nonvirtual bool operator== (nullptr_t) const noexcept;
            nonvirtual bool operator!= (nullptr_t) const noexcept;

        public:
            /**
             *  \brief return true iff *this != nullptr
             */
            nonvirtual explicit operator bool () const noexcept;

        private:
            Envelope_ fEnvelope_;

        private:
            template <typename T2>
            friend class SharedPtr;

        private:
            template <typename T2>
            friend class enable_shared_from_this;
        };

        /**
         *  An OPTIONAL class you can mix into 'T', and use with SharedPtr<>. If the 'T' used in SharedPtr<T> inherits
         *  from this, then you can re-constitute a SharedPtr<T> from it's T* (since the count is pulled along-side).
         *  This is sometimes handy if you wish to take a SharedPtr<> object, and pass the underlying pointer through
         *  a layer of code, and then re-constitute the SharedPtr<> part later.
         *
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
         *  This is like the std::enable_shared_from_this - making your type inherit from it, allows you to recover the
         *  underlying SharedPtr<> given a plain C++ pointer to T.
         *
         */
        template <typename T>
        class enable_shared_from_this : public Private_::ReferenceCounterContainerType_ {
        public:
            constexpr enable_shared_from_this ();
            constexpr enable_shared_from_this (const enable_shared_from_this& src);

        public:
            enable_shared_from_this& operator= (const enable_shared_from_this&) = delete;

        public:
            ~enable_shared_from_this () = default;

        public:
            /**
             */
            nonvirtual SharedPtr<T> shared_from_this ();

        private:
            template <typename T2>
            friend class SharedPtr;
        };

        /**
         */
        template <typename T, typename... ARG_TYPES>
        SharedPtr<T> MakeSharedPtr (ARG_TYPES&&... args);

        /**
         *  Callers can always use EITHER shared_ptr or SharedPtr. But this define tells which is probably faster
         *  for the most part. Often types, users will want to define a typedef which selects
         *  the faster implementation.
         *
         *  \note   As of 2015-10-21, and version v2.0a109, it appears on windows/VS2k13, SharedPtr is about 30% faster,
         *          and GCC 4.9 on ubuntu its about 3% faster (not enough to be significant). This requires more testing
         *          though.
         *
         *  \note   As of 2016-06-09, and version v2.0a148, it appears on windows/VS2k13, SharedPtr is about 10% faster,
         *          and GCC 5.3 on ubuntu its a wash. For now - switch so we default to shared_ptr on gcc and SharedPtr using
         *          vs2k15
         *
         *  \note   As of 2016-07-09, and version v2.0a156 (after SpinLock fence/lockless blockallocation changes), it appears on windows/VS2k13,
         *          SharedPtr is still about 10% faster
         *
         *  \note   As of 2017-10-13 - Stroika v2.0a217, and Visual Studio.Net 2017 (15.4.0) - SharedPtr<> remains about 15% faster than shared_ptr<> 
         *          on windows.
         */
#if defined(_MSC_VER)
        constexpr bool kSharedPtr_IsFasterThan_shared_ptr = true;
#else
        constexpr bool kSharedPtr_IsFasterThan_shared_ptr = false;
#endif
    }

    namespace Execution {
        template <typename T>
        void ThrowIfNull (const Memory::SharedPtr<T>& p);
    }
}
namespace std {
    /**
     *  overload the std::dynamic_pointer_cast to work with Stroika SharedPtr<> as well.
     *
     *  This returns an empty SharedPtr (no throw) if the type cannot be converted with dynamic_cast<>.
     */
    template <typename TO_TYPE_T, typename FROM_TYPE_T>
    Stroika::Foundation::Memory::SharedPtr<TO_TYPE_T> dynamic_pointer_cast (const Stroika::Foundation::Memory::SharedPtr<FROM_TYPE_T>& sp) noexcept;

    /**
     *  overload the std::atomic_load_explicit/atomic_load to work with Stroika SharedPtr<> as well.
     */
    template <typename T>
    Stroika::Foundation::Memory::SharedPtr<T> atomic_load (const Stroika::Foundation::Memory::SharedPtr<T>* copyFrom);
    template <typename T>
    Stroika::Foundation::Memory::SharedPtr<T> atomic_load_explicit (const Stroika::Foundation::Memory::SharedPtr<T>* copyFrom, memory_order);

    /**
     *  overload the std::atomic_store_explicit/atomic_store to work with Stroika SharedPtr<> as well.
     */
    template <typename T>
    void atomic_store (Stroika::Foundation::Memory::SharedPtr<T>* storeTo, Stroika::Foundation::Memory::SharedPtr<T> o);
    template <typename T>
    void atomic_store_explicit (Stroika::Foundation::Memory::SharedPtr<T>* storeTo, Stroika::Foundation::Memory::SharedPtr<T> o, memory_order);
}
#endif /*_Stroika_Foundation_Memory_SharedPtr_h_*/

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "SharedPtr.inl"
