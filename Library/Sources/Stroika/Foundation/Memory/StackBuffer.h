/*
 * Copyright(c) Sophist Solutions, Inc. 1990-2022.  All rights reserved
 */
#ifndef _Stroika_Foundation_Memory_StackBuffer_h_
#define _Stroika_Foundation_Memory_StackBuffer_h_ 1

#include "../StroikaPreComp.h"

#include "../Configuration/Common.h"
#include "../Configuration/Concepts.h"
#include "../Configuration/TypeHints.h"

#include "Common.h"

/**
 *  \file
 *
 *  \version    <a href="Code-Status.md#Beta">Beta</a>
 */

namespace Stroika::Foundation::Memory {

    using std::byte;

    /**
     *  \brief Store variable sized array on the stack (\see also InlineBuffer<T,BUF_SIZE>)
     * 
     *  Typically, StackBuffer<> combines the performance of using a stack buffer (inline array on stack) to store arrays with
     *  the safety and flexability of using the free store (malloc).
     *
     *  Think of it as a hybrid between std::vector<> and std::array - with functionality like
     *  std::vector, but performance more like std::array (except where array always declared on the stack).
     *
     *  Since StackBuffer is just intended for local stack use, the objects cannot be copied or moved.
     * 
     *  \note About Alloca support
     *      Considered the possability of somehow implementing this via alloca. The problem lies with the
     *      interaction between this class, and the compiler generated frame allocations. I think these
     *      are fundementally inconsistent. At least likely hard to reconcile.
     *      So PROBABLY not possible, but enuf of a possible win to leave open the possability.
     *
     *  All allocated objects are default initialized, unless they are allocated through a call to resize_uninitialized(), or
     *  the constructor with the argument eUninitialized
     *
     *  \note The default BUF_SIZE (size allocated on the stack) will vary by type T, and platform. Its
     *        intended to be a tradeoff between avoiding malloc, and avoiding other costs (like Windows _chkstk)
     *
     *  \par Example Usage
     *      @see Samples/SimpleService project
     *      \code
     *          Memory::StackBuffer<byte> useKey{keyLen};  // no need to default initialize cuz done automatically
     *          (void)::memcpy (useKey.data (), key.begin (), min (keyLen, key.size ()));
     *      \endcode
     * OR
     *      \code
     *          Memory::StackBuffer<byte> useKey{Memory::eUninitiialized, keyLen};
     *          (void)::memset (useKey.data (), 0, keyLen);
     *          (void)::memcpy (useKey.data (), key.begin (), min (keyLen, key.size ()));
     *      \endcode
     * 
     *  \todo   MAYBE - add constexpr member saying kInlineControlSupported_ = true for ??? maybe gcc and msvc;
     *          Maybe define:
     *              #if defined(COMPILER_GCC) && defined(NDEBUG)
     *              #define ALWAYS_INLINE inline __attribute__((__always_inline__))
     *              #elif defined(COMPILER_MSVC) && defined(NDEBUG)
     *              #define ALWAYS_INLINE __forceinline
     *              #else
     *              #define ALWAYS_INLINE inline
     *              #endif
     *          and declare ALL methods that could allocate memory with alloca and then in allocate check
     *          if (kInlineControlSupported_) {
     *              alloca()
     *          }
     *          else {
     *              new bytes[] - or whatever it does now
     *          }
     *          // and simialar in delete. MAYBE have kMaxBytesAlloca and trakc how much we allocate and then switch to new
     *          // in whcih case need flag for DTOR to delete.
     *          (note see other note about alloca - I think this is not possible)
     *
     *  \note   Future implementations of StackBuffer will REQUIRE they are actually allocated on the stack, and probably use alloca()
     *
     *  \note   \em Thread-Safety   <a href="Thread-Safety.md#C++-Standard-Thread-Safety">C++-Standard-Thread-Safety</a>
     *
     *  \note   We do not provide an operator[] overload because this creates ambiguity with the operator* overload.
     *
     *  \note   Implementation Note - we store the store the 'capacity' in a union (fCapacityOfFreeStoreAllocation_) overlapping with fInlinePreallocatedBuffer_ if its > BUF_SIZE, and pin it at the minimum
     *          to BUF_SIZE
     *
     */
    template <
        typename T = std::byte,
#if qPlatform_Windows
        size_t BUF_SIZE = (((2 * 1024) / sizeof (T)) == 0 ? 1 : ((2 * 1024) / sizeof (T)))
#else
        size_t BUF_SIZE = ((4096 / sizeof (T)) == 0 ? 1 : (4096 / sizeof (T)))
#endif
        >
    class StackBuffer {
    public:
        /**
         */
        using value_type = T;

    public:
        /**
         */
        using pointer       = T*;
        using const_pointer = const T*;

    public:
        /**
         */
        using iterator       = T*;
        using const_iterator = const T*;

    public:
        /**
         */
        using reference       = T&;
        using const_reference = const T&;

    public:
        /**
         *  StackBuffer::StackBuffer (size_t) specifies the initial size - like StackBuffer::StackBuffer {} followed by resize (n); 
         *  StackBuffer::default-ctor creates a zero-sized stack buffer (so resize with resize, or push_back etc).
         */
        StackBuffer ();
        explicit StackBuffer (size_t nElements);
        StackBuffer (UninitializedConstructorFlag, size_t nElements);
        StackBuffer (const StackBuffer& src) = delete;
        StackBuffer (StackBuffer&& src)      = delete;
        template <typename ITERATOR_OF_T, enable_if_t<Configuration::IsIterator_v<ITERATOR_OF_T>, char>* = nullptr>
        StackBuffer (ITERATOR_OF_T start, ITERATOR_OF_T end);
        ~StackBuffer ();

    public:
        /** 
         */
        nonvirtual StackBuffer& operator= (const StackBuffer& rhs) = delete;

    public:
        /**
         *  \brief returns the same value as data () - a live pointer to the start of the buffer.
         * 
         *  \note This was changed from non-explicit to explicit in Stroika v3.0d1
         */
        nonvirtual explicit operator const T* () const;
        nonvirtual explicit operator T* ();

    public:
        /**
         */
        nonvirtual iterator       begin ();
        nonvirtual const_iterator begin () const;

    public:
        /**
         */
        nonvirtual iterator       end ();
        nonvirtual const_iterator end () const;

    public:
        /**
         *  \brief returns a (possibly const) pointer to the start of the live buffer data. This return value can be invalidated
         *         by any changes in size/capacity of the stackbuffer (but not by other changes, like at).
         */
        nonvirtual pointer       data ();
        nonvirtual const_pointer data () const;

    public:
        /**
         */
        nonvirtual reference       at (size_t i);
        nonvirtual const_reference at (size_t i) const;

    public:
        /**
         */
        reference       operator[] (size_t i) { return at (i); }
        const_reference operator[] (size_t i) const { return at (i); }

    public:
        /**
         *  Returns the 'size' the StackBuffer can be resized up to without any additional memory allocations.
         *  This always returns a value at least as large as the BUF_SIZE template parameter.
         *
         *  @see reserve
         */
        nonvirtual size_t capacity () const;

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve.
         *
         *  @see capacity
         *
         *  \req  (newCapacity >= size ());
         *  \ens  ((nElements <= BUF_SIZE && capacity () == BUF_SIZE) or (nElements > BUF_SIZE and nElements == capacity ()));
         */
        nonvirtual void reserve (size_t newCapacity);

    public:
        /**
         *  Provide a hint as to how much (contiguous) space to reserve. Like @reserve () but this will not shrink the buffer.
         *
         *  @see reserve
         */
        nonvirtual void ReserveAtLeast (size_t newCapacityAtLeast);

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer in ELEMENTS (not necessarily in bytes).
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual size_t GetSize () const;

    public:
        /**
         *  Returns the number of (constructed) elements in the buffer.
         *
         *  @see GetSize ();        // alias
         *  @see capacity ();
         *
         *  \ensure size () <= capacity ();
         */
        nonvirtual size_t size () const;

    public:
        /**
         *  Grow or shrink the buffer. The 'size' is the number of constructed elements, and this function automatically
         *  assures the capacity is maintained at least as large as the size.
         *
         *  If resize () causes the list to grow, the new elements are default-initialized()
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void resize (size_t nElements);

    public:
        /**
         * \brief same as resize (), except leaves newly created elements uninitialized (requires is_trivially_copyable_v<T>) 
         *
         *  \req is_trivially_copyable_v<T>
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void resize_uninitialized (size_t nElements);

    public:
        /**
         *  Grow the buffer to at least nElements in size (wont shrink). The 'size' is the number of constructed elements,
         *  and this function automatically assures the capacity is maintained at least as large as the size.
         *
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void GrowToSize (size_t nElements);

    public:
        /**
         * \brief same as GrowToSize (), except leaves newly created elements uninitialized (requires is_trivially_copyable_v<T>)
         *
         *  \req is_trivially_copyable_v<T>
         *  \ensure GetSize () <= capacity ();
         */
        nonvirtual void GrowToSize_uninitialized (size_t nElements);

    public:
        nonvirtual void push_back (Configuration::ArgByValueType<T> e);

    public:
        /**
         */
        nonvirtual void clear ();

    private:
        nonvirtual void reserve_ (size_t nElements);

#if qDebug
    private:
        static constexpr byte kGuard1_[8] = {
            byte{0x45},
            byte{0x23},
            byte{0x12},
            byte{0x56},
            byte{0x99},
            byte{0x76},
            byte{0x12},
            byte{0x55},
        };
        static constexpr byte kGuard2_[8] = {
            byte{0x15},
            byte{0x32},
            byte{0xa5},
            byte{0x16},
            byte{0x11},
            byte{0x7a},
            byte{0x90},
            byte{0x10},
        };
#endif

    private:
        // note must be inline declared here since used in type definition below
        static constexpr size_t SizeInBytes_ (size_t nElts)
        {
            return sizeof (T[1]) * nElts; // not sure why return sizeof (T[nElts]); fails on vs2k17?
        }

    private:
        nonvirtual byte* LiveDataAsAllocatedBytes_ ();

    private:
        static byte* Allocate_ (size_t bytes);

    private:
        static void Deallocate_ (byte* bytes);

    private:
        size_t fSize_{};
#if qDebug
        byte fGuard1_[sizeof (kGuard1_)];
#endif
        DISABLE_COMPILER_MSC_WARNING_START (4324)
        union {
            size_t fCapacityOfFreeStoreAllocation_;                               // only valid if fLiveData_ != &fInlinePreallocatedBuffer_[0]
            alignas (T) byte fInlinePreallocatedBuffer_[SizeInBytes_ (BUF_SIZE)]; // alignas both since sometimes accessed as array of T, and sometimes as size_t
        };
        DISABLE_COMPILER_MSC_WARNING_END (4324)

#if qDebug
        byte fGuard2_[sizeof (kGuard2_)];
#endif
        T* fLiveData_{};

    private:
        nonvirtual bool UsingInlinePreallocatedBuffer_ () const;

    public:
        nonvirtual void Invariant () const noexcept;

    private:
#if qDebug
        nonvirtual void Invariant_ () const noexcept;
        nonvirtual void ValidateGuards_ () const noexcept;
#endif

    private:
        nonvirtual T*       BufferAsT_ () noexcept;
        nonvirtual const T* BufferAsT_ () const noexcept;

    private:
        static void DestroyElts_ (T* start, T* end) noexcept;
    };

}

/*
 ********************************************************************************
 ***************************** Implementation Details ***************************
 ********************************************************************************
 */
#include "StackBuffer.inl"

#endif /*_Stroika_Foundation_Memory_StackBuffer_h_*/
